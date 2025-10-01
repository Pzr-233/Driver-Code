#include "gpio.h"
#include "usart.h"
#include "TMC2209.h"
#include "delay.h" 
MicrostepMode step_factor;
static uint8_t TMC2209_Transmit_Buffer[8];
static uint8_t TMC2209_Receive_Buffer[9];
static uint32_t IHOLD_IRUN_Value=IHOLD_IRUN_Default_Value;
static int32_t VACTUAL_Value;
typedef unsigned char UCHAR;
 

void TMC2209_Init(void)
{    
    Disable_TMC2209(Slave_Address0);
    TMC2209_Write_Reg(Slave_Address0,GCONF,0x000001CF);// Step 1: 设置GCONF (0x00), 值 0x0000 0088 (pdn_disable=1, mstep_reg_select=1)
    // TMC2209_Write_Reg(Slave_Address0,IHOLD_IRUN,0x00081F05);// Step 2: 设置IHOLD_IRUN (0x10), 值 0x00040808 (IRUN=24, IHOLD=8, IHOLDDELAY=4)
    TMC2209_Write_Reg(Slave_Address0,CHOPCONF,0x0400280C);// Step 4: 设置CHOPCONF (0x6C), 值 0x10000053 (mres=256, intpol=1, TOFF=5)
    // TMC2209_Write_Reg(Slave_Address0,VACTUAL,0);
    TMC2209_Write_Reg(Slave_Address0,TPWMTHRS,500);// Step 6: 设置TPWMTHRS (0x13), 值 500 (速度阈值)
    TMC2209_Read_Reg(Slave_Address0,TPWMTHRS);
    Enable_StealthChop(Slave_Address0);
    Enable_TMC2209(Slave_Address0);
}

void TMC2209_Write_IHOLD_IRN(uint8_t Slave_Address,uint8_t IHOLD,uint8_t IRUN)
{
    IHOLD=IHOLD*31/100;
    IRUN=IRUN*31/100;
    IHOLD_IRUN_Value|=IRUN;
    IHOLD_IRUN_Value<<=8;
    IHOLD_IRUN_Value|=IHOLD;
    TMC2209_Write_Reg(Slave_Address,IHOLD_IRUN,IHOLD_IRUN_Value);
}

void TMC2209_Set_Motor_Velocity(uint8_t Slave_Address,int32_t Velocity)
{
VACTUAL_Value=(Velocity/60)*200*step_factor;
TMC2209_Write_Reg(Slave_Address,VACTUAL,VACTUAL_Value);
}
void TMC2209_Set_Motor_MICRO(uint8_t Slave_Address,MicrostepMode MICRO)
{
    step_factor=MICRO;
    // TMC2209_Write_Reg(Slave_Address,IHOLD_IRUN,);
}

void Enable_TMC2209(uint8_t Slave_Address)
{
    if(Slave_Address==Slave_Address0)
    {
    HAL_GPIO_WritePin(GPIOA,ENN,GPIO_PIN_RESET);
    }
}
void Disable_TMC2209(uint8_t Slave_Address)
{
    if(Slave_Address==Slave_Address0)
    {
    HAL_GPIO_WritePin(GPIOA,ENN,GPIO_PIN_SET);
    }
}
void Enable_StealthChop(uint8_t Slave_Address)
{
    if(Slave_Address==Slave_Address0)
    {
    HAL_GPIO_WritePin(GPIOA,SPREAD,GPIO_PIN_RESET);
    }
}
void Enable_SpreadCycle(uint8_t Slave_Address)
{
    if(Slave_Address==Slave_Address0)
    {
    HAL_GPIO_WritePin(GPIOA,SPREAD,GPIO_PIN_SET);
    }
}
// 计算 CRC 校验和
void swuart_calcCRC(UCHAR* datagram, UCHAR datagramLength)
{
    int i, j;
    UCHAR* crc = datagram + (datagramLength - 1); // CRC 存储在数据包的最后一个字节
    UCHAR currentByte;
    *crc = 0; // 初始化 CRC 为 0
    for (i = 0; i < (datagramLength - 1); i++) { // 处理除最后一个字节外的所有字节
        currentByte = datagram[i];
        for (j = 0; j < 8; j++) { // 处理当前字节的每一位
            if ((*crc >> 7) ^ (currentByte & 0x01)) {
                *crc = (*crc << 1) ^ 0x07; // 如果 CRC 高位与当前字节最低位异或结果为 1，则 CRC 左移并异或多项式 0x07
            } else {
                *crc = (*crc << 1); // 否则，CRC 仅左移
            }
            currentByte = currentByte >> 1; // 当前字节右移处理下一位
        }
    }
}

void TMC2209_Write_Reg(uint8_t Slave_Address,uint8_t Register_Address,uint32_t Data)
{
        Register_Address|=0x80;
        TMC2209_Transmit_Buffer[0]=sync_and_reserved;
        TMC2209_Transmit_Buffer[1]=Slave_Address;
        TMC2209_Transmit_Buffer[2]=Register_Address;
        TMC2209_Transmit_Buffer[3]=Data>>24;
        TMC2209_Transmit_Buffer[4]=(Data&0x00FF0000)>>16;
        TMC2209_Transmit_Buffer[5]=(Data&0x0000FF00)>>8;
        TMC2209_Transmit_Buffer[6]=Data&0xFF;
        swuart_calcCRC(TMC2209_Transmit_Buffer,8);
        HAL_UART_Transmit(&huart2,TMC2209_Transmit_Buffer,8,10);
        HAL_UART_Transmit(&TMC2209_uart,TMC2209_Transmit_Buffer,8,10);
    
   
}

void TMC2209_Read_Reg(uint8_t Slave_Address,uint8_t Register_Address)
{
        TMC2209_Transmit_Buffer[0]=sync_and_reserved;
        TMC2209_Transmit_Buffer[1]=Slave_Address;
        TMC2209_Transmit_Buffer[2]=Register_Address;
        swuart_calcCRC(TMC2209_Transmit_Buffer,4);
        HAL_UART_Transmit(&huart2,TMC2209_Transmit_Buffer,4,10);
        HAL_UART_Transmit(&TMC2209_uart,TMC2209_Transmit_Buffer,4,10);
        HAL_UART_Receive(&TMC2209_uart,TMC2209_Receive_Buffer,9,100);
        HAL_UART_Transmit(&huart2,TMC2209_Receive_Buffer,9,10);
}
// 设置运动速度（内部脉冲发生器）
void set_motor_speed(int32_t speed) 
{
    // 速度计算：speed = (目标速度 Hz) * 2^23 / f_CLK
    // 示例：12MHz时钟，100转/分（1.8°电机）
    // rpm = (speed * 60 * f_CLK) / (2^23 * 200)
    uint32_t vactual = (speed << 14) / 1875;  // 简化计算公式
    
    TMC2209_Write_Reg(Slave_Address0,VACTUAL,vactual);  // VACTUAL寄存器

}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
     if(huart==&TMC2209_uart)
    {
    }
}