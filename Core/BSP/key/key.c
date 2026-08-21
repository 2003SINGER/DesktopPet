//
// Created by keysk on 2025/11/20.
//

#include "key.h"

#include <stdlib.h>

#include "cmsis_os2.h"
#include "tim.h"

#define IS_KEY1_PRESSED() (HAL_GPIO_ReadPin(Key1_GPIO_Port, Key1_Pin) == GPIO_PIN_RESET)
#define IS_KEY2_PRESSED() (HAL_GPIO_ReadPin(Key2_GPIO_Port, Key2_Pin) == GPIO_PIN_RESET)
#define IS_KEY3_PRESSED() (HAL_GPIO_ReadPin(Key3_GPIO_Port, Key3_Pin) == GPIO_PIN_RESET)
#define KEY_DEBOUNCE_TIME 30

//osSemaphoreWait(KeySemaphoreHandle, osWaitForever);

uint8_t isKey1Clicked() {
  if (IS_KEY1_PRESSED()) {
    osDelay(KEY_DEBOUNCE_TIME);
    if (IS_KEY1_PRESSED()) {
      return 1;
    }
  }
  return 0;
}

uint8_t isKey2Clicked() {
  if (IS_KEY2_PRESSED()) {
    osDelay(KEY_DEBOUNCE_TIME);
    if (IS_KEY2_PRESSED()) {
      return 1;
    }
  }
  return 0;
}

uint8_t isKey3Clicked() {
  if (IS_KEY3_PRESSED()) {
    osDelay(KEY_DEBOUNCE_TIME);
    if (IS_KEY3_PRESSED()) {
      return 1;
    }
  }
  return 0;
}

uint32_t knobcount=-1;

uint8_t knob() {
  uint32_t temp;
  temp=__HAL_TIM_GET_COUNTER(&htim1);
  if (knobcount==-1||knobcount==temp) {
    knobcount=temp;
    return 0;
  }else if (abs(knobcount-temp)<=200) {
    if (knobcount<temp) {
      return 2;
    }else {
      return 1;
    }
  }else {
    if (knobcount>temp) {
      return 2;
    }else {
      return 1;
    }
  }
}