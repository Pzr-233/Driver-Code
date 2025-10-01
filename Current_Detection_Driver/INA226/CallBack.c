#include "gpio.h"
#include "i2c.h"
#include "INA226.h"
 extern uint16_t Flag; 
extern int16_t current_raw;
// #include "spi.h"
// #include "ST7735.h"
// #include "tim.h"
// #include "lvgl.h"
// #include "lv_port_disp_template.h"
// #include "Get_Key.h"

// #include "usart.h"
// #include "stdio.h"
// void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
// {
   
// }

//  void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
//  {
//     if(hspi==ST7735_SPI)
//     {   
      
//     }

//  }

 void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
 {
    if(hi2c==INA226_I2C)
    {
    HAL_I2C_Master_Receive_DMA(INA226_I2C,INA226_Dev_Address_R,Current_Data,2);
    }
 }

  void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)
  {

    if(hi2c==INA226_I2C)
    {
        current_raw = (Current_Data[0] << 8) | Current_Data[1];
    }
  }

