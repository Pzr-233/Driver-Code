#include "gpio.h"
#include "spi.h"
#include "W25N01.h"
#include "usart.h"
W25N01 W25N01_01;
uint8_t BBM_Table[128];
uint8_t W25N01_Page_Buffer_1[2048];
// uint8_t W25N01_Page_Buffer_2[2048];

void W25N01_Init(void)
{
    W25N01_RESET();
    while(W25N01_isDev_Busy());
    W25N01_Write_Enable();
    while(W25N01_isDev_Busy());
    W25N01_Close_Write_Protect();
    W25N01_01.State_Reg2_Value=Reg2_Def_Value;
    W25N01_01.State_Reg3_Value=Reg3_Def_Value;
}

void W25N01_Start_SPI(void)
{
    HAL_GPIO_WritePin(W25N01_CS_Port,W25N01_CS,GPIO_PIN_RESET);
}
void W25N01_Close_SPI(void)
{
    HAL_GPIO_WritePin(W25N01_CS_Port,W25N01_CS,GPIO_PIN_SET);
}
void W25N01_Write_Single_Command(uint8_t Cmd)
{
    while(W25N01_isDev_Busy());
    W25N01_Start_SPI();
    HAL_SPI_Transmit(W25N01_SPI,&Cmd,1,10);
    W25N01_Close_SPI();
}
void W25N01_Write_Enable(void)
{   
     W25N01_Write_Single_Command(W25_Write_Enable);
}
void W25N01_Write_Disable(void)
{
    W25N01_Write_Single_Command(W25_Write_Disable);
}
void W25N01_RESET(void)
{
    W25N01_Write_Single_Command(W25_Dev_RESET);
}

void W25N01_Erase_Block(uint16_t Block_Addr)//块地址
{
    W25N01_Write_Enable();
    W25N01_01.State=Sta_W25_Single_Data;
    uint8_t Temp_Buffer[4]={W25_Block_Erase,0x00};
    Block_Addr=Block_Addr<<6;
    Temp_Buffer[2]=Block_Addr>>8;
    Temp_Buffer[3]=Block_Addr&0xFF;
    while(W25N01_isDev_Busy());
    W25N01_Start_SPI();
    HAL_SPI_Transmit(W25N01_SPI,Temp_Buffer,4,10);
    W25N01_Close_SPI();
}

void W25N01_Write_Buffer(uint16_t Column_Addr,uint8_t *Data_Buffer,uint16_t Len)
{
    W25N01_Write_Enable();
    while(W25N01_isDev_Busy());
    uint8_t Temp_Buffer[3]={W25_Progrom_Data_Load};
    Temp_Buffer[1]=Column_Addr>>8;
    Temp_Buffer[2]=Column_Addr&0xFF;
    W25N01_01.State=Sta_W25_Single_Data;
    while(W25N01_isDev_Busy());
    W25N01_Start_SPI();
    HAL_SPI_Transmit(W25N01_SPI,Temp_Buffer,3,100);
    HAL_SPI_Transmit(W25N01_SPI,Data_Buffer,Len,100);
    W25N01_Close_SPI();
}
void W25N01_Load_Data_To_Buffer(uint16_t Block_Addr,uint8_t Page_Addr)
{
    uint8_t Temp_Buffer[4]={W25_Page_Data_Read,0x00};
    Block_Addr=(Block_Addr<<6)|Page_Addr;
    Temp_Buffer[2]=Block_Addr>>8;
    Temp_Buffer[3]=Block_Addr&0xFF;
    while(W25N01_isDev_Busy());
    W25N01_Start_SPI();
    HAL_SPI_Transmit(W25N01_SPI,Temp_Buffer,4,10);
    W25N01_Close_SPI();
}
void W25N01_Read_Buffer(uint16_t Column_Addr,uint8_t *Data_Buffer,uint16_t Len)
{
    uint8_t Temp_Buffer[4]={W25_Read};
    Temp_Buffer[1]=Column_Addr>>8;
    Temp_Buffer[2]=Column_Addr&0xFF;
    Temp_Buffer[3]=0x00;
    W25N01_01.State=Sta_W25_Read_Data;
    while(W25N01_isDev_Busy());
    W25N01_Start_SPI();
    HAL_SPI_Transmit(W25N01_SPI,Temp_Buffer,4,10);
    HAL_SPI_Receive(W25N01_SPI,Data_Buffer,Len,100);
    W25N01_Close_SPI();
}

void W25N01_Write_To_Page(uint16_t Block_Addr,uint8_t Page_Addr)
{
    uint8_t Temp_Buffer[4]={W25_Program_Execute,0x00};
    Block_Addr=(Block_Addr<<6)|Page_Addr;
    Temp_Buffer[2]=Block_Addr>>8;
    Temp_Buffer[3]=Block_Addr&0xFF;
    while(W25N01_isDev_Busy());
    W25N01_Start_SPI();
    HAL_SPI_Transmit(W25N01_SPI,Temp_Buffer,4,10);
    W25N01_Close_SPI();
} 

uint32_t W25N_ReadID(void)
{
    uint8_t cmd[2] = {0x9F,0x00}; // 读ID命令
    uint8_t id[3] = {0};
    W25N01_Start_SPI();
    HAL_SPI_Transmit(W25N01_SPI, cmd,2, 100);
    HAL_SPI_Receive(W25N01_SPI, id, 3, 100);
    W25N01_Close_SPI();
    return (id[0] << 16) | (id[1] << 8) | id[2]; // W25N01GV ID: 0xEFAA21
}

void W25N01_Read_State_Reg(uint8_t Reg_Addr)
{
    uint8_t Temp_Buffer[2]={W25_Read_Status_Register,Reg_Addr};
    W25N01_Start_SPI();
    HAL_SPI_Transmit(W25N01_SPI,Temp_Buffer,2, 100);
    if(Reg_Addr==State_Reg_1)
    {
    HAL_SPI_Receive(W25N01_SPI,&W25N01_01.State_Reg1_Value,1,10);
    }
    else if(Reg_Addr==State_Reg_2)
    {
    HAL_SPI_Receive(W25N01_SPI,&W25N01_01.State_Reg2_Value,1,10);
    }
    else if(Reg_Addr==State_Reg_3)
    {
    HAL_SPI_Receive(W25N01_SPI,&W25N01_01.State_Reg3_Value,1,10);
    }
    W25N01_Close_SPI();
}

void W25N01_Set_Read_Mode(uint8_t Mode)
{
    if(Mode==Buffer_Read)
    {
        W25N01_01.State_Reg2_Value|=0b00001000;
        uint8_t Temp_Buffer[3]={W25_Write_Status_Register,State_Reg_2,W25N01_01.State_Reg2_Value};
        W25N01_Start_SPI();
        HAL_SPI_Transmit(W25N01_SPI,Temp_Buffer,3,10);
        W25N01_Close_SPI();

    }
    else if(Mode==Continuous_Read)
    {
        W25N01_01.State_Reg2_Value&=0b11110111;
        uint8_t Temp_Buffer[3]={W25_Write_Status_Register,State_Reg_2,W25N01_01.State_Reg2_Value};
        W25N01_Start_SPI();
        HAL_SPI_Transmit(W25N01_SPI,Temp_Buffer,3,10);
        W25N01_Close_SPI();
    }
}

uint8_t W25N01_isDev_Busy(void)
{
    W25N01_Read_State_Reg(State_Reg_3);
    if((W25N01_01.State_Reg3_Value&0b00000001)==0)
    {
        W25N01_01.Bit_Busy=0;
    }
    else
    {
        W25N01_01.Bit_Busy=1;
    }
    return  W25N01_01.Bit_Busy;
}
void W25N01_Close_Write_Protect(void)
{
    W25N01_01.State_Reg1_Value=Reg1_Def_Value;
    uint8_t Temp_Buffer[3]={W25_Write_Status_Register,State_Reg_1,W25N01_01.State_Reg1_Value};
    W25N01_Start_SPI();
    HAL_SPI_Transmit(W25N01_SPI,Temp_Buffer,3,10);
    W25N01_Close_SPI();
}
void W25N01_Check_Bad_Block(void)
{
    uint8_t Temp_Data=0;
    uint16_t Temp_Count=0;
    for(uint16_t i=0;i<1024;i++)
    {
        W25N01_Load_Data_To_Buffer(i,0);
        while(1)
        {
            W25N01_isDev_Busy();
            if(W25N01_01.Bit_Busy==0)
            {
                break;
            }
        }
        W25N01_Read_Buffer(0,&Temp_Data,sizeof(Temp_Data));
        if(Temp_Data!=0xFF)
        {
        Temp_Count++;
        }
        }
    W25N01_01.Bad_Block_Num=Temp_Count;
}

void W25N01_Scan_All_Chip(void)
{
    W25N01_Set_Read_Mode(Continuous_Read);
    while (W25N01_isDev_Busy());
    W25N01_Load_Data_To_Buffer(0,0);
    while (W25N01_isDev_Busy());
    for(uint16_t i=0;i<65535;i++)
    {
    W25N01_Read_Buffer(0,W25N01_Page_Buffer_1,sizeof(W25N01_Page_Buffer_1));
    HAL_UART_Transmit(&huart1,W25N01_Page_Buffer_1,sizeof(W25N01_Page_Buffer_1),100);
    }
}
