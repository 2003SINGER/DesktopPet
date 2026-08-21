#include "cmsis_os.h"
#include "cmsis_os2.h"
#include "main.h"
#include "tim.h"
#include "key/key.h"
#include "Global/Page.h"
#include "Global/Pet.h"
//
// Created by 2003SINGER on 2026/2/21.
//

void StartInputTask(void *argument) {
    HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL);
    for (;;) {
        if (osSemaphoreAcquire(KeySemaphoreHandle, 5) == osOK) {
            if (isKey1Clicked()) {
                HAL_GPIO_TogglePin(Blue_GPIO_Port, Blue_Pin);
            } else if (isKey2Clicked()) {
                HAL_GPIO_TogglePin(Green_GPIO_Port, Green_Pin);
            } else if (isKey3Clicked()) {
                if (osSemaphoreAcquire(KeySemaphoreHandle, 300) == osOK) {
                    if (isKey3Clicked()) {
                        //双击

                        HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);
                        __HAL_TIM_SET_AUTORELOAD(&htim4, 500);
                        __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, 10);

                        switch (Page) {
                            case MAIN:
                                if (MainPointer==FOUR) {
                                    PetState.Happiness=100;
                                    PetState.Satiety=100;
                                    PetState.Energy=100;
                                }
                            case HISTORY:
                                Page = MAIN;
                                break;
                            case INTERACTION:
                                Page = MAIN;
                        }
                        osDelay(200);
                        HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_4);
                    }
                } else {
                    //单击

                    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);
                    __HAL_TIM_SET_AUTORELOAD(&htim4, 500);
                    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, 10);

                    switch (Page) {
                        case MAIN:
                            switch (MainPointer) {
                                case ONE:
                                    Page = HISTORY;
                                    break;
                                case TWO:
                                    Page = INTERACTION;
                                    break;
                                case THREE:
                                    if (PetState.PetMode == NORMAL) PetState.PetMode = FAST;
                                    else PetState.PetMode = NORMAL;
                                    break;
                                case FOUR:
                                    PetState.Happiness +=10;
                                    PetState.Satiety -=3;
                                    PetState.Energy -=2;
                                    PetLimit();
                                    break;
                            }
                            break;
                        case HISTORY:
                            break;
                        case INTERACTION:
                            switch (InteractionPointer) {
                                case ONE:
                                    break;
                                case TWO:
                                    break;
                                case THREE:
                                    break;
                                case FOUR:
                                    break;
                            }
                    }

                    osDelay(200);
                    HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_4);
                }
            }
        }else {
            int knobstate = knob();
            switch (knobstate) {
                case 0:
                    break;
                case 1:
                    switch (Page) {
                        case MAIN:
                            MainPointer--;
                            MainPointer %= 4;
                            break;
                        case HISTORY:
                            HistoryPointer++;
                            HistoryPointer %= 5;
                            break;
                    }
                    osDelay(200);
                    knobcount = __HAL_TIM_GET_COUNTER(&htim1);
                    break;
                case 2:
                    switch (Page) {
                        case MAIN:
                            MainPointer++;
                            MainPointer %= 4;
                            break;
                    case HISTORY:
                            HistoryPointer--;
                            HistoryPointer %= 5;
                            break;
                    }
                    osDelay(200);
                    knobcount = __HAL_TIM_GET_COUNTER(&htim1);
                    break;
            }
        }
    }
}
