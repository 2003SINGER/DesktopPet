#include "cmsis_os2.h"
#include "aht20/aht20.h"
#include "Global/Pet.h"
#include "oled/oled.h"

//
// Created by 2003SINGER on 2026/2/21.
//

void StartSensorTask(void *argument) {
    osMutexAcquire(i2c1MutexHandle, osWaitForever);
    AHT20_Init();
    osMutexRelease(i2c1MutexHandle);
    for (;;) {

        osMutexAcquire(i2c1MutexHandle, osWaitForever);
        AHT20_Read(&EnvironmentState.temperature, &EnvironmentState.humidity);
        osMutexRelease(i2c1MutexHandle);
        EnvironmentState.temperatureint=EnvironmentState.temperature;
        EnvironmentState.temperaturefloat = (EnvironmentState.temperature-(float)EnvironmentState.temperatureint)*100.0f;
        EnvironmentState.humidityint=EnvironmentState.humidity;
        EnvironmentState.humidityfloat = (EnvironmentState.humidity-(float)EnvironmentState.humidityint)*100.0f;

        osDelay(1000);
    }
}