#ifndef __W25N01_H
#define __W25N01_H
/*W25N01数据结构
W25N01共128M,共分为了1204个块(Block 128KB),每块包含64页(Page 2KB 2048字节);
W25N01最小写入单位为页(2KB),如需修改该页部分数据则需重写整页;
W25N01最小擦除单位为块(128KB),不支持页级或子块擦除,擦除后所有页数据变为0xFF;
W25N01物理读取(0x13)最小单位是整页（2KB），逻辑读取(0x03)最小单位是1字节;
块地址10位(指定到块时需输入16位,高10位为块地址(0x00-0x3FF),低6位为空);
页地址6位(指定到页时需输入16位,高10位为块地址(0x00-0x3FF),低6位为块内偏移即页地址(0x00-0x3F));
列地址12位(指定到列时需输入16位,高12位为页内偏移(0x000-0x83F共2112字节),低4位为空);
列读取需要先用页读取(0x13)将整页2KB加载至缓冲区,再用列读取(0x03)读出指定列
*/

#include "spi.h"

#define W25N01_SPI &hspi1
#define W25N01_CS_Port GPIOA
#define W25N01_CS GPIO_PIN_4

typedef struct 
{
uint8_t State;
uint8_t *Data_Buffer;
uint8_t Bit_Busy;
uint8_t State_Reg1_Value;
uint8_t State_Reg2_Value;
uint8_t State_Reg3_Value;
uint16_t Bad_Block_Num;
}W25N01;

extern W25N01 W25N01_01;
//Operation Code
#define W25_Dev_RESET 0xFF
#define W25_Dev_ID    0x9F
#define W25_Read_Status_Register 0x0F
#define W25_Write_Status_Register 0x1F
#define W25_Write_Enable 0x06
#define W25_Write_Disable 0x04
#define W25_BBM 0xA1
#define W25_Read_BBM_LUT 0xA5
#define W25_Block_Erase 0xD8
#define W25_Progrom_Data_Load 0x02          //发送数据到芯片内部缓冲区，最大2KB(一页)
#define W25_Random_Program_Data_Load 0x84
#define W25_Program_Execute 0x10            //将芯片内部缓冲区的数据写入物理内存中
#define W25_Page_Data_Read 0x13             //将芯片内的数据加载到缓冲区
#define W25_Read 0x03                       //在缓冲区内读取数据
#define W25_Fast_Read 0x0B
#define W25_Fast_Read_4B_Addr 0x0C
#define W25_Last_ECC_failure_Page_Addr 0xA9

//W25N01 State
#define Sta_W25_Read_Data 0x01
#define Sta_W25_Write_Page 0x02
#define Sta_W25_Single_Data 0x03
#define Sta_W25_Write_Buffer 0x04

//Statue Register Address
#define State_Reg_1 0xA0
#define State_Reg_2 0xB0
#define State_Reg_3 0xC0

//Statue Register Value
#define Reg1_Def_Value 0b00000000
#define Reg2_Def_Value 0b00011000
#define Reg3_Def_Value 0b00000000

#define Buffer_Read 0x01
#define Continuous_Read 0x00
void W25N01_Init(void);


void W25N01_Write_Single_Command(uint8_t Cmd);
void W25N01_Start_SPI(void);
void W25N01_Close_SPI(void);
uint32_t W25N_ReadID(void);
void W25N01_RESET(void);
void W25N01_Write_Enable(void);
void W25N01_Write_Disable(void);

void W25N01_Set_Read_Mode(uint8_t Mode);
void W25N01_Read_State_Reg(uint8_t Reg_Addr);
void W25N01_Write_To_Page(uint16_t Block_Addr,uint8_t Page_Addr);
void W25N01_Read_Buffer(uint16_t Column_Addr,uint8_t *Data_Buffer,uint16_t Len);
void W25N01_Load_Data_To_Buffer(uint16_t Block_Addr,uint8_t Page_Addr);
void W25N01_Write_Buffer(uint16_t Column_Addr,uint8_t *Data_Buffer,uint16_t Len);
void W25N01_Erase_Block(uint16_t Block_Addr);
uint8_t W25N01_isDev_Busy(void);
void W25N01_Check_Bad_Block(void);
uint32_t W25N_ReadID(void);
void W25N01_Scan_All_Chip(void);
void W25N01_Close_Write_Protect(void);

#endif // !__W25N_H01