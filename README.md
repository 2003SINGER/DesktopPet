# DesktopPet · STM32 桌面宠物

> 2025 冬季寒假 · 电子科协软件练习项目
>
> ⚠️ **本项目为 FreeRTOS 试手练习，很多地方属于探索性质，仅供展示与学习，不保证工程严谨性。**

一个跑在 STM32F103 上的"电子桌面宠物"：读取环境温湿度、用旋钮和按键交互、在 128×64 OLED 上显示状态，并通过一个简化的"心情状态机"模拟宠物的快乐 / 饱食 / 精力变化。底层用 FreeRTOS + CMSIS-RTOS V2 把原本前后台的程序改造成了多任务结构。

---

## 硬件清单

| 模块 | 型号 / 说明 | 连接 |
| --- | --- | --- |
| 主控 | **STM32F103C8T6**（Cortex-M3，LQFP48） | — |
| 显示屏 | 128×64 I2C OLED（驱动地址 `0x7A`） | I2C1：`PB6`(SCL) / `PB7`(SDA) |
| 温湿度 | AHT20（I2C） | 同 I2C1（与 OLED 共用总线） |
| 旋钮 | EC11 编码器 | TIM1：`PA8`(CH1) / `PA9`(CH2) |
| 蜂鸣器 | 有源/无源蜂鸣器（PWM 驱动） | TIM4_CH4：`PB9` |
| 按键 | Key1 / Key2 / Key3（下降沿中断） | `PB12` / `PB13` / `PB15`（EXTI） |
| RGB 指示灯 | Blue / Green / Red（GPIO 输出） | `PA6` / `PA7` / `PB0` |
| 调试串口 | USART3 | `PB10`(TX) / `PB11`(RX) |
| 调试下载 | SWD | `PA13`(SWDIO) / `PA14`(SWCLK) |
| 晶振 | 8 MHz 外部晶振 | `PD0`(OSC_IN) / `PD1`(OSC_OUT) |

系统节拍（HAL 时基）由 **TIM3** 提供；Flash 最后一页（`0x0800F800`）用于掉电保存宠物状态。

---

## 软件架构（RTOS）

内核：**FreeRTOS Kernel V10.3.1** + **CMSIS-RTOS V2**（`osThreadNew` / `osMutexNew` / `osSemaphoreNew` 等），抢占式调度，1 ms tick。

### 任务（按优先级从高到低）

| 任务 | 优先级 | 职责 |
| --- | --- | --- |
| `InputTask` | High | 旋钮编码器 + 按键；按键通过信号量从中断唤醒 |
| `PetTask` | AboveNormal | 宠物状态机：每秒衰减 快乐/饱食/精力，更新心情，写 Flash，PWM 驱动蜂鸣器 |
| `SensorTask` | Normal | 每 1 s 读一次 AHT20 温湿度，写入全局 `EnvironmentState` |
| `ScreenTask` | Low | OLED 渲染：清帧 → `Petframe()` → 刷屏（I2C 加互斥量） |
| `BLETask` | Low | ⚠️ **空桩**，暂未实现 |

### 任务间通信

- **`i2c1Mutex`**（互斥量）：保护共享的 I2C1 总线，`SensorTask`（读 AHT20）与 `ScreenTask`（刷 OLED）互斥访问。
- **`KeySemaphore`**（二进制信号量）：按键 EXTI 中断里 `osSemaphoreRelease`，`InputTask` 里 `osSemaphoreAcquire` 阻塞等待——典型的"中断 → 任务"异步唤醒。
- **`BLEQueue`**（消息队列）：已创建但**暂未使用**，为后续 BLE 预留。
- 温湿度、宠物状态、页面等主要数据流目前走**全局变量**（`PetState` / `EnvironmentState` / `Page`），非消息队列传递。

---

## 目录结构

```
DesktopPet/
├── Core/
│   ├── App/                 # 应用层（非 CubeMX 生成）
│   │   ├── Global/          # 全局状态：Pet.h / Page.h
│   │   └── Tasks/           # 5 个任务 + 蜂鸣器回调
│   ├── BSP/                 # 板级驱动：oled / aht20 / key / knob
│   ├── Inc/  Src/           # CubeMX 生成 + 用户代码（main.c / freertos.c / gpio.c ...）
│   └── Inc/FreeRTOSConfig.h # FreeRTOS 内核配置
├── Drivers/                 # STM32 HAL + CMSIS
├── Middlewares/Third_Party/FreeRTOS/   # FreeRTOS 内核 + CMSIS-RTOS V2
├── cmake/                   # 工具链文件（gcc-arm-none-eabi）
├── DesktopPet.ioc           # CubeMX 工程
├── CMakeLists.txt / CMakePresets.json
└── .gitignore               # 已忽略构建目录与 .workbuddy-ai/
```

---

## 开发环境

- **工具链**：`arm-none-eabi-gcc`（CMake 通过 `cmake/gcc-arm-none-eabi.cmake` 引用）
- **构建系统**：CMake + Ninja
- **IDE**：CLion（或任意支持 CMake 的编辑器）
- **代码生成**：STM32CubeMX 6.x（`.ioc` 可重新生成初始化代码）
- **烧录/调试**：ST-Link（SWD），配合 OpenOCD 或 STM32CubeProgrammer

### 编译

```bash
# 配置（Debug 预设，输出到 build/Debug）
cmake --preset Debug

# 构建
cmake --build --preset Debug
```

产物位于 `build/Debug/`，包含 `DesktopPet.elf` 与 `DesktopPet.bin`。

> Release 预设：`cmake --preset Release` / `cmake --build --preset Release`

### 烧录

```bash
# 例：用 OpenOCD 烧录（需自行配置 stlink 接口）
openocd -f interface/stlink-v2.cfg -f target/stm32f1x.cfg \
  -c "program build/Debug/DesktopPet.bin verify reset exit 0x08000000"
```

或用 STM32CubeProgrammer / CLion 的 Run/Debug 配置通过 ST-Link 下载。

---

## 已知问题 / TODO（练习遗留）

- 🖼️ **暂无任何美术资源**：宠物形象未绘制，`INTERACTION` 页面目前为空。
- 📡 `BLETask`、FreeRTOS 软件定时器、`BLEQueue` 队列均为预留空桩，未实现。
- 🔗 任务间主数据流使用全局变量共享，非严格的消息队列通信，扩展时要注意并发安全。
- 🐛 `CMakeLists.txt` 中任务源文件写为 `BleTask.c`，磁盘实际为 `BLETask.c`，依赖 Windows 大小写不敏感才能编译；换 Linux/macOS 工具链需修正。

---

## License

学习 / 展示用途，未指定明确开源协议。引用或二次开发请注明来源。
