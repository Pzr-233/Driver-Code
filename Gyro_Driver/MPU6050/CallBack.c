#include "gpio.h"
#include "spi.h"
#include "ST7735.h"
#include "tim.h"
#include "MPU6050.h"
// #include "W25N01.h"
// #include "lvgl.h"
// #include "lv_port_disp_template.h"
#include "Get_Key.h"
extern uint16_t Flag;
// #include "usart.h"
// #include "stdio.h"

extern uint16_t Data_New;
extern uint16_t Data_Old;
extern float velocity;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    // if(htim==&htim9)
    // {
    //    lv_tick_inc(1);
    // }
     if (htim==Key_tim)
    {
        Kry_Scan();
    }
    else if (htim==MPU6050_TIM)
    {
        if(mpu6050_flag.MPU6050_Timing_Flag==0)
        {
        mpu6050_flag.MPU6050_Timing_Flag=1;
        }
    }
    // else if (htim==ST7735_Tim)
    // {
    //     if(HAL_SPI_GetState(ST7735_SPI)==HAL_SPI_STATE_READY) 
    //     {
    //     ST7735_State=1;
    //     }
    // }
}

// void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
// {
//     if(htim==&htim10)
//     {
//         Data_New=__HAL_TIM_GET_COUNTER(&htim10);
//         __HAL_TIM_SetCounter(&htim10,0);
//     }
// }

 void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
 {
    if(hspi==ST7735_SPI)
    {   
       if(ST7735_Mode==Mode_Fill_Color) 
       {
            if(Y_Now_Flag<Y_All_Flag)
            {
                Y_Now_Flag++;
                HAL_SPI_Transmit_DMA(ST7735_SPI,X_Buffer,X_Len*2);
            }
            else
            {
                ST7735_Mode=0;
                Y_Now_Flag=0;
                ST7735_Close_SPI();
            }
       }
       else if (ST7735_Mode==Mode_Single_Data)
       {
        ST7735_Mode=0;
        ST7735_Close_SPI();
        // lv_disp_flush_ready(disp_drv_extern);
       }
    }
    // else if(hspi==W25N01_SPI)
    // {
    //     if(W25N01_01.State==Sta_W25_Write_Page)
    //     {
    //     W25N01_01.State=0;
    //     HAL_SPI_Transmit_DMA(W25N01_SPI,W25N01_01.Data_Buffer,sizeof(W25N01_01.Data_Buffer));
    //     }
       
    //     else if (W25N01_01.State==Sta_W25_Write_Buffer)
    //     {
    //     W25N01_01.State=0;
    //     HAL_SPI_Transmit_DMA(W25N01_SPI,W25N01_01.Data_Buffer,sizeof(W25N01_01.Data_Buffer));
    //     }
    //     else if (W25N01_01.State==Sta_W25_Single_Data)
    //     {
    //     W25N01_Close_SPI();
    //     W25N01_01.State=0;
    //     }
    // }
 }

