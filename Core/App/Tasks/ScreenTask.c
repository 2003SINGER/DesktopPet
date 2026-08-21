#include <stdio.h>

#include "cmsis_os2.h"
#include "oled/oled.h"
#include "Global/Page.h"
#include "Global/Pet.h"
//
// Created by 2003SINGER on 2026/2/21.
//
PAGE Page;
void Petframe();
void MAINPRINT(int SparkleCounter);
void HISTORYPRINT(int SparkleCounter);
POINTER MainPointer;
POINTER InteractionPointer;
POINTER HistoryPointer;
int Sparkle(int temp);

void StartScreenTask(void *argument) {
    osDelay(40);
    OLED_Init();
    Page=HISTORY;
    MainPointer=THREE;
    InteractionPointer=ONE;
    HistoryPointer=ONE;
    for (;;) {
        OLED_NewFrame();
        Petframe();

        osMutexAcquire(i2c1MutexHandle, osWaitForever);
        OLED_ShowFrame();
        osMutexRelease(i2c1MutexHandle);
    }
}

 void Petframe() {
    //0,2,4,6,8,10不亮
    static int SparkleCounter=1;
    switch (Page) {
        case MAIN:
            MAINPRINT(SparkleCounter);
            break;
        case HISTORY:
            HISTORYPRINT(SparkleCounter);
            break;
        case INTERACTION:
            break;
    }
    SparkleCounter++;
    SparkleCounter%=10;
}

int Sparkle(int temp) {
    if (temp>=6) return 0;
    return 1;
}

void MAINPRINT(int SparkleCounter) {

            if (MainPointer==TWO&&Sparkle(SparkleCounter)) {
                OLED_DrawRectangle(0,1,20,0,OLED_COLOR_REVERSED);
                OLED_DrawRectangle(0,5,20,0,OLED_COLOR_REVERSED);
                OLED_DrawRectangle(0,9,20,0,OLED_COLOR_REVERSED);
            }else {
                OLED_DrawRectangle(0,1,20,0,OLED_COLOR_NORMAL);
                OLED_DrawRectangle(0,5,20,0,OLED_COLOR_NORMAL);
                OLED_DrawRectangle(0,9,20,0,OLED_COLOR_NORMAL);
            }
            if (Sparkle(SparkleCounter)&&MainPointer==ONE) {
                OLED_DrawRectangle(28,1,PetState.Happiness,0,OLED_COLOR_REVERSED);
                OLED_DrawRectangle(28,5,PetState.Satiety,0,OLED_COLOR_REVERSED);
                OLED_DrawRectangle(28,9,PetState.Energy,0,OLED_COLOR_REVERSED);
            }else {
                OLED_DrawRectangle(28,1,PetState.Happiness,0,OLED_COLOR_NORMAL);
                OLED_DrawRectangle(28,5,PetState.Satiety,0,OLED_COLOR_NORMAL);
                OLED_DrawRectangle(28,9,PetState.Energy,0,OLED_COLOR_NORMAL);
            }

            OLED_DrawRectangle(58,1,0,0,OLED_COLOR_REVERSED);
            OLED_DrawRectangle(58,5,0,0,OLED_COLOR_REVERSED);
            OLED_DrawRectangle(58,9,0,0,OLED_COLOR_REVERSED);

            OLED_DrawRectangle(102,1,0,0,OLED_COLOR_REVERSED);
            OLED_DrawRectangle(102,5,0,0,OLED_COLOR_REVERSED);
            OLED_DrawRectangle(102,9,0,0,OLED_COLOR_REVERSED);

            char msg[50];
            sprintf(msg,"湿:");
            OLED_PrintString(0,16,msg,&font12x12,OLED_COLOR_NORMAL);
            sprintf(msg,"温:");
            OLED_PrintString(0,31,msg,&font12x12,OLED_COLOR_NORMAL);
            sprintf(msg,"%d.%d",EnvironmentState.humidityint,EnvironmentState.humidityfloat);
            OLED_PrintString(17,16,msg,&font12x12,OLED_COLOR_NORMAL);
            sprintf(msg,"%d.%d",EnvironmentState.temperatureint,EnvironmentState.temperaturefloat);
            OLED_PrintString(17,31,msg,&font12x12,OLED_COLOR_NORMAL);

            switch (PetState.PetMode){
                case NORMAL:
                    OLED_DrawFilledRectangle(0,45,20,19,OLED_COLOR_NORMAL);
                    sprintf(msg,"常");
                    OLED_PrintString(3,46,msg,&font16x16,OLED_COLOR_REVERSED);
                    if (Sparkle(SparkleCounter)&&MainPointer==THREE) {
                        OLED_DrawFilledRectangle(0,45,20,19,OLED_COLOR_NORMAL);
                    }
                    break;
                case FAST:
                    OLED_DrawFilledRectangle(0,45,20,19,OLED_COLOR_NORMAL);
                    sprintf(msg,"极");
                    OLED_PrintString(3,46,msg,&font16x16,OLED_COLOR_REVERSED);
                    if (Sparkle(SparkleCounter)&&MainPointer==THREE) {
                        OLED_DrawFilledRectangle(0,45,20,19,OLED_COLOR_NORMAL);
                    }
                    break;
            }

            switch (PetState.PetMind) {
                case HAPPY:
                    sprintf(msg,"喜");
                    break;
                case SAD:
                    sprintf(msg,"悲");
                    break;
                case SATISFIED:
                    sprintf(msg,"饱");
                    break;
                case HUNGER:
                    sprintf(msg,"饿");
                    break;
                case ENERGETIC:
                    sprintf(msg,"奋");
                    break;
                case TIRED:
                    sprintf(msg,"累");
                    break;
                case COMMON:
                    sprintf(msg,"普");
                    break;
            }
            OLED_PrintString(25,46,msg,&font16x16,OLED_COLOR_NORMAL);
            OLED_DrawFilledRectangle(57,63,64,1,OLED_COLOR_NORMAL);
            if (Sparkle(SparkleCounter)&&MainPointer==FOUR) OLED_DrawFilledRectangle(57,63,64,1,OLED_COLOR_REVERSED);
}

void HISTORYPRINT(int SparkleCounter) {
    OLED_DrawRectangle(0, 11, 0, 51, OLED_COLOR_NORMAL);
    OLED_DrawRectangle(0, 63, 127, 0, OLED_COLOR_NORMAL);

    // OLED_DrawRectangle(0,1,20,0,OLED_COLOR_NORMAL);
    // OLED_DrawRectangle(0,5,20,0,OLED_COLOR_NORMAL);
    // OLED_DrawRectangle(0,9,20,0,OLED_COLOR_NORMAL);

    char msg[50];
    switch (HistoryPointer) {
        case ONE:
            sprintf(msg,"%d",PetState.Happiness);
            OLED_PrintString(110,30,msg,&font12x12,OLED_COLOR_NORMAL);

            OLED_DrawRectangle(28,1,PetState.Happiness,0,OLED_COLOR_NORMAL);
            OLED_DrawRectangle(58,1,0,0,OLED_COLOR_REVERSED);
            OLED_DrawRectangle(102,1,0,0,OLED_COLOR_REVERSED);
            for (int i = timepointer, j = timepointercounter; j >=1; j--) {
                i+=99;
                i%=100;
                OLED_SetPixel(j,62-(happiness[i]/2),NORMAL);
            }
            break;
        case TWO:
            sprintf(msg,"%d",PetState.Satiety);
            OLED_PrintString(110,30,msg,&font12x12,OLED_COLOR_NORMAL);

            OLED_DrawRectangle(28,5,PetState.Satiety,0,OLED_COLOR_NORMAL);
            OLED_DrawRectangle(58,5,0,0,OLED_COLOR_REVERSED);
            OLED_DrawRectangle(102,5,0,0,OLED_COLOR_REVERSED);
            for (int i = timepointer, j = timepointercounter; j >=1; j--) {
                i+=99;
                i%=100;
                OLED_SetPixel(j,62-(satiety[i]/2),NORMAL);
            }
            break;
        case THREE:
            sprintf(msg,"%d",PetState.Energy);
            OLED_PrintString(110,30,msg,&font12x12,OLED_COLOR_NORMAL);

            OLED_DrawRectangle(28,9,PetState.Energy,0,OLED_COLOR_NORMAL);
            OLED_DrawRectangle(58,9,0,0,OLED_COLOR_REVERSED);
            OLED_DrawRectangle(102,9,0,0,OLED_COLOR_REVERSED);
            for (int i = timepointer, j = timepointercounter; j >=1; j--) {
                i+=99;
                i%=100;
                OLED_SetPixel(j,62-(energy[i]/2),NORMAL);
            }
            break;
        case FOUR:
            sprintf(msg,"湿");
            OLED_PrintString(110,20,msg,&font12x12,OLED_COLOR_NORMAL);
            sprintf(msg,"%d",EnvironmentState.humidityint);
            OLED_PrintString(110,40,msg,&font12x12,OLED_COLOR_NORMAL);
            for (int i = timepointer, j = timepointercounter; j >=1; j--) {
                i+=99;
                i%=100;
                OLED_SetPixel(j,62-(humidity[i]/2),NORMAL);
            }
            break;
        case FIVE:
            sprintf(msg,"温");
            OLED_PrintString(110,40,msg,&font12x12,OLED_COLOR_NORMAL);
            sprintf(msg,"%d",EnvironmentState.temperatureint);
            OLED_PrintString(110,20,msg,&font12x12,OLED_COLOR_NORMAL);
            for (int i = timepointer, j = timepointercounter; j >=1; j--) {
                i+=99;
                i%=100;
                OLED_SetPixel(j,62-(temperature[i]),NORMAL);
            }
            break;
    }
}
