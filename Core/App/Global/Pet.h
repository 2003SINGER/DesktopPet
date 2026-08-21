//
// Created by 2003SINGER on 2026/2/22.
//

#ifndef DESKTOPPET_PET_H
#define DESKTOPPET_PET_H

void State_Init();

typedef struct {
    float temperature;       // 温度（单位：摄氏度）
    float humidity;          // 湿度（单位：百分比，0-100）
    int temperatureint;
    int temperaturefloat;
    int humidityint;
    int humidityfloat;
} ENVIRONMENTState;

typedef enum {
    NORMAL=0,
    FAST=1
}PETMODE;

typedef enum {
    SAD=0,
    HAPPY=1,
    SATISFIED=2,
    HUNGER=3,
    TIRED=4,
    ENERGETIC=5,
    COMMON=6
}PETMIND;

typedef struct {
    uint8_t Happiness;
    uint8_t Energy;
    uint8_t Satiety;
    PETMODE PetMode;
    PETMIND PetMind;
} PETState;

extern ENVIRONMENTState EnvironmentState;
extern PETState PetState;

extern uint8_t happiness[100];
extern uint8_t satiety[100];
extern uint8_t energy[100];
extern uint8_t humidity[100];
extern uint8_t temperature[100];
extern uint8_t timepointer;
extern uint8_t timepointercounter;

void PetLimit();
#endif //DESKTOPPET_PET_H