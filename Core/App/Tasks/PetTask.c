#include "cmsis_os2.h"
#include "Global/Pet.h"

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_flash.h"
#include "stm32f1xx_hal_flash_ex.h"

#include "FreeRTOS.h"
#include "task.h"
#include "tim.h"
//
// Created by 2003SINGER on 2026/2/21.
//

ENVIRONMENTState EnvironmentState;
PETState PetState;

uint8_t happiness[100];
uint8_t satiety[100];
uint8_t energy[100];
uint8_t humidity[100];
uint8_t temperature[100];
uint8_t timepointer=0;
uint8_t timepointercounter=0;

void Flash_Write_Pet_Data(void);

void StartPetTask(void *argument) {
    State_Init();
    uint8_t modecount=1;
    for (;;) {
        //基本状态更新
        if (PetState.PetMode==FAST||modecount==30) {
            modecount=1;
            if (PetState.Happiness>=1) PetState.Happiness--;
            if (PetState.Satiety>=1) PetState.Satiety--;
            if (PetState.Energy>=1) PetState.Energy--;
        }else {
            modecount++;
        }

        Flash_Write_Pet_Data();

        PetLimit();
        happiness[timepointer]=PetState.Happiness;
        satiety[timepointer]=PetState.Satiety;
        energy[timepointer]=PetState.Energy;
        temperature[timepointer]=EnvironmentState.temperature;
        humidity[timepointer]=EnvironmentState.humidity;
        timepointer++;
        timepointercounter++;
        if (timepointercounter>=100) {
            timepointercounter=100;
        }
        timepointer%=100;

        //心情系统更新
        PETMIND CurrentMind=PetState.PetMind;
        if (PetState.Energy <= 30) {
            PetState.PetMind = TIRED;
        } else if (PetState.Happiness <= 30) {
            PetState.PetMind = SAD;
        } else if (PetState.Satiety <= 30) {
            PetState.PetMind = HUNGER;
        } else if (PetState.Energy >= 75) {
            PetState.PetMind = ENERGETIC;
        } else if (PetState.Satiety >= 75) {
            PetState.PetMind = SATISFIED;
        } else if (PetState.Happiness >= 75) {
            PetState.PetMind = HAPPY;
        } else {
            PetState.PetMind = COMMON;
        }
        if (CurrentMind!=PetState.PetMind) {
            switch (PetState.PetMind) {
                case ENERGETIC:
                    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);
                    __HAL_TIM_SET_AUTORELOAD(&htim4, 500);
                    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, 50);
                    osDelay(200);
                    HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_4);
                    break;
                case NORMAL:
                    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);
                    __HAL_TIM_SET_AUTORELOAD(&htim4, 2000);
                    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, 100);
                    osDelay(200);
                    HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_4);
                    break;
                case TIRED:
                    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);
                    __HAL_TIM_SET_AUTORELOAD(&htim4, 5000);
                    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, 50);
                    osDelay(200);
                    HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_4);
                    break;
            }
        }

        if (
            CurrentMind == PetState.PetMind)
            osDelay(1000);
        else {
            osDelay(800);
            CurrentMind = PetState.PetMind;
        }
    }
}

void State_Init() {
    EnvironmentState.temperature = 0;
    EnvironmentState.humidity = 0;
    uint32_t PetData = *(uint32_t *) 0x0800F800;

    int H = PetData / 1000000, S, E = PetData % 1000;
    S = PetData / 1000;
    S %= 1000;
    if (H >= 0 && H <= 100) {
        PetState.Happiness = H;
    } else PetState.Happiness = 100;
    if (S >= 0 && S <= 100) {
        PetState.Satiety = S;
    } else PetState.Satiety = 100;
    if (E >= 0 && E <= 100) {
        PetState.Energy = E;
    } else PetState.Energy = 100;

    if (PetData / 1000000000 == 1) PetState.PetMode = FAST;
    else PetState.PetMode = NORMAL;

    for (int i=0;i<100;i++) {
        happiness[i] = 100;
        satiety[i] = 100;
        energy[i] = 100;
    }
}

void PetLimit() {
    if (PetState.Happiness >100) PetState.Happiness = 100;
    if (PetState.Satiety>100) PetState.Satiety = 100;
    if (PetState.Energy>100) PetState.Energy = 100;
    if (PetState.Happiness <0) PetState.Happiness = 0;
    if (PetState.Satiety <0) PetState.Satiety = 0;
    if (PetState.Energy <0) PetState.Energy = 0;
}

void Flash_Write_Pet_Data(void) {

    uint32_t PetData=PetState.Happiness*1000000+PetState.Satiety*1000+PetState.Energy;

    if (PetState.PetMode==FAST) {
        PetData+=1000000000;
    }

    taskENTER_CRITICAL();

    HAL_FLASH_Unlock();

    FLASH_EraseInitTypeDef EraseInitStruct = {0};
    uint32_t PageError = 0;
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.PageAddress = 0x0800F800;
    EraseInitStruct.NbPages = 1;
    HAL_FLASHEx_Erase(&EraseInitStruct, &PageError);

    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, 0x0800F800, PetData);

    HAL_FLASH_Lock();

    taskEXIT_CRITICAL();
}