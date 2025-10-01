#include "gpio.h"
#include "tim.h"
#include "Get_Key.h"
#include "ST7735.h"
#include "HX711.h"
#include "Laser_Tachometer.h"
// #include "usart.h"
// #include "stdio.h"
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    
    if(htim==Key_tim)
    {
       Key_Scan();
    }
    else if(htim==Laser_Count_TIM)
    {
    Blade_1.Round_Num++;
    }
    else if(htim==Laser_CHECK_TIM)
    {
    LASER_Get_Velocity();
    }
    else if(htim==HX711_TIM)
    {
        Scale_1.Init_Buffer[Scale_1.Buffer_Index]=HX711_Get_Count();
        Scale_1.Buffer_Index++;
        Scale_1.Buffer_Index%=5;
        if(Scale_1.HX711_Init_Flag==0)
        {
            Scale_1.Sampling_Num++;
            if(Scale_1.Sampling_Num>=20)
            {
            Scale_1.HX711_Init_Flag=1;
            HX711_Data_Average();
            }
          
        }
    }
    
}


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
 }

// void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
// {

// }
