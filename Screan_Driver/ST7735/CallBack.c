#include "gpio.h"
#include "spi.h"
#include "ST7735.h"

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    #if(ST7735_Use_Lvgl==1) 
    if(htim==&htim9)
    {
       lv_tick_inc(1);
    }
    #endif
    #if(ST7735_Use_Tim==1) 
     if (htim==ST7735_Tim)
    {
        if(HAL_SPI_GetState(ST7735_SPI)==HAL_SPI_STATE_READY) 
        {
        st7735.ST7735_State=1;
        }
    }
    #endif
}

 void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
 {
    if(hspi==ST7735_SPI)
    {   
       if(st7735.ST7735_Mode==Mode_Fill_Color) 
       {
            if(st7735.Y_Now_Flag<st7735.Y_All_Flag)
            {
                st7735.Y_Now_Flag++;
                HAL_SPI_Transmit_DMA(ST7735_SPI,st7735.X_Buffer,st7735.X_Len*2);
            }
            else
            {
                st7735.ST7735_Mode=0;
                st7735.Y_Now_Flag=0;
                ST7735_Close_SPI();
            }
       }
       else if (st7735.ST7735_Mode==Mode_Single_Data)
       {
        st7735.ST7735_Mode=0;
        ST7735_Close_SPI();
        #if(ST7735_Use_Lvgl==1)
        lv_disp_flush_ready(disp_drv_extern);
        #endif
       }
    }
 }

