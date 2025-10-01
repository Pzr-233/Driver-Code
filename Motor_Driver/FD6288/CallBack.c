#include "gpio.h"
// #include "spi.h"
// #include "ST7735.h"
#include "tim.h"
#include "FD6288.h"
// #include "lvgl.h"
// #include "lv_port_disp_template.h"
#include "Get_Key.h"
#include "synchronous_rectification.h"
#include "usart.h"
// #include "stdio.h"
extern uint8_t OLED_Flag;
extern uint16_t Test_Flag_1;
extern uint16_t Test_Flag_2;
extern uint16_t Test_Flag_3;
extern float Time[];
extern uint32_t Count_New;
extern uint32_t Count_Old;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
 
     if (htim == Key_tim)
    {
        Kry_Scan();
    }
        else if (htim==&htim9)
    {
         OLED_Flag=1;
    }
    else if(htim==&htim5)
    {
        FD6288_1.Step++;
        if(FD6288_1.Step>6)
        {
        FD6288_1.Step=1;
        }
        FD6288_Driver();
    }
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if(huart==&huart6)
    {
    FD6288_Receive_Data(&receive_data,sizeof(receive_data));
    }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart==&huart6)
    {

    }
}

 void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if(htim==&htim2)
    {
        Test_Flag_1++;
        Test_Flag_1%=12;
        Count_Old=Count_New;
        Count_New=__HAL_TIM_GET_COUNTER(&htim2);
        Time[Test_Flag_1]=2.f/((Count_New-Count_Old)*0.000001f);
        // else if(Test_Flag_1>1000)
        // {
        //     Test_Flag_2=1;
        // }
        // if(__HAL_TIM_GET_FLAG(&htim2,TIM_FLAG_CC1))
        // {
        // Count_Old=Count_New;
        // Count_New=__HAL_TIM_GET_COMPARE(&htim2,TIM_cha)
        // }
    }



}   


//  void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
//  {
//     if(hspi==ST7735_SPI)
//     {   
//        if(ST7735_Mode==Mode_Fill_Color) 
//        {
//             if(Y_Now_Flag<Y_All_Flag)
//             {
//                 Y_Now_Flag++;
//                 HAL_SPI_Transmit_DMA(ST7735_SPI,X_Buffer,X_Len*2);
//             }
//             else
//             {
//                 ST7735_Mode=0;
//                 Y_Now_Flag=0;
//                 ST7735_Close_SPI();
//             }
//        }
//        else if (ST7735_Mode==Mode_Single_Data)
//        {
//         ST7735_Mode=0;
//         ST7735_Close_SPI();
//         // lv_disp_flush_ready(disp_drv_extern);
//        }
//     }

//  }

