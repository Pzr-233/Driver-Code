#ifndef __INA226_H
#define __INA226_H

#define INA226_I2C &hi2c1

#define INA226_Dev_Address 0x40
#define INA226_Dev_Address_R (0x40<<1)|0x01
#define INA226_Dev_Address_W (0x40<<1)|0x00

#define Config_Reg 0x00
#define Current_Reg 0x05
#define R_SHUNT 0.01          // 10mΩ采样电阻
#define MAX_CURRENT 2.0       // 最大电流2A


void INA226_Write_Data(uint8_t Mem_Address,uint16_t Value);
void INA226_Read_Data(uint8_t Mem_Address);
void INA226_Init(void);

extern uint8_t Current_Data[2];



#endif 