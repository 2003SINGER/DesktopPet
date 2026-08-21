//
// Created by keysk on 2025/11/20.
//

#ifndef SMARTFARM_KEY_H
#define SMARTFARM_KEY_H

#include "main.h"
#define delay(ms) osDelay(ms)

uint8_t isKey1Clicked();
uint8_t isKey3Clicked();
uint8_t isKey2Clicked();
uint8_t knob();

extern uint32_t knobcount;
//0停止；1顺时针；2逆时针

#endif //SMARTFARM_KEY_H
