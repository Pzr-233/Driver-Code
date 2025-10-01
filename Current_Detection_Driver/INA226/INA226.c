#include "gpio.h"
#include "i2c.h"
#include "INA226.h"
uint8_t Current_Data[2];
void INA226_Write_Data(uint8_t Mem_Address,uint16_t Value)
{   
    uint8_t Data[3];
    Data[0]=Mem_Address;
    Data[1]=Value>>8;
    Data[2]=Value&0xFF;
    HAL_I2C_Master_Transmit(INA226_I2C, INA226_Dev_Address_W, Data,3, 100); // 设置指针
}

void INA226_Read_Data(uint8_t Mem_Address)
{    
    HAL_I2C_Master_Transmit_DMA(INA226_I2C, INA226_Dev_Address_W, &Mem_Address,1); // 设置指针
}


void INA226_Init(void)
{   
    INA226_Write_Data(0x00,0x4127);
    INA226_Write_Data(0x05,0x20C9);
    HAL_Delay(10); // 等待配置生效


}