#include "spi.h"
#include "gpio.h"
#include "OLED.h"
#include "tim.h"
#include "math.h"
#include "stdlib.h"
#include "delay.h"
// #include "Get_Key.h"
#include "Motor.h"
// #include "adc.h"
#include <stdbool.h>
#define MT6701_CS  GPIO_PIN_4
uint8_t Receive_Arry[3];
uint8_t tx_buffer[3]={0x11,0x22,0x33};
 uint16_t Absolute_Angle;
 float Angle_Change;
 int16_t Cycle_Change;

 uint8_t Flag_First_Get_Data=0;
 uint8_t Flag_First_Get_Absolute_Angle=0;
 uint8_t Falg_First_Velocity_Measure=0;
 uint16_t Last_Data;
 int16_t Now_Data;
 uint16_t Angle_Data;
 int32_t Data_Change=0;
 int32_t Last_Data_Change=0;
 float Actual_velocity;
//上升沿(高电平)读取数据,下降沿(低电平)交换数据

void MT6701_ON(void)//拉低电平,开启通讯
{
	HAL_GPIO_WritePin(GPIOA,MT6701_CS,GPIO_PIN_RESET);
}

void MT6701_OFF(void)//拉高电平,关闭通讯
{
	HAL_GPIO_WritePin(GPIOA,MT6701_CS,GPIO_PIN_SET);
}


void MT6701_ReceiveData(void)//接收一次MT6701传输的24位数据，顺时针旋转角度减少
{
	MT6701_ON();
	HAL_SPI_Receive_DMA(&hspi1,Receive_Arry,3);
}

void MT6701_Get_Absolute_Angle()//从24位数据中提取前14位角度编码,并将其转换为0~360°赋予返回值
{
		if(Flag_First_Get_Data==1)
		{
		Angle_Data=Receive_Arry[0]<<8;
		Angle_Data|=Receive_Arry[1];
		Angle_Data>>=2;
		if(Angle_Data-Last_Data>16000)
		{
		Data_Change-=(16383-Angle_Data+Last_Data);
		Last_Data=Angle_Data;
		}
		else if (Last_Data-Angle_Data>16000)
		{
		Data_Change+=(16383-Last_Data+Angle_Data);
		Last_Data=Angle_Data;
		}
		else if (Last_Data>Angle_Data)
		{
		Data_Change-=(Last_Data-Angle_Data);
		Last_Data=Angle_Data;
		}
		else if (Angle_Data>=Last_Data)
		{
		Data_Change+=(Angle_Data-Last_Data);
		Last_Data=Angle_Data;
		}
		
		}
		else if(Flag_First_Get_Data==0)
		{
		Angle_Data=Receive_Arry[0]<<8;
		Angle_Data|=Receive_Arry[1];
		Angle_Data>>=2;
		Last_Data=Angle_Data;
		Flag_First_Get_Data=1;
		}
}

 void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{	
	MT6701_OFF();
	MT6701_Get_Absolute_Angle();
	MT6701_ReceiveData();
}
void Motor_Velocity_Measure(void)
{
 if(Falg_First_Velocity_Measure==1)
 {
  Actual_velocity=(float)(Last_Data_Change-Data_Change)*300/16383;
  Last_Data_Change=Data_Change;
 }
 else if (Falg_First_Velocity_Measure==0)
 {
	Last_Data_Change=Data_Change;
	Falg_First_Velocity_Measure=1;
 }
}
void Enable_Velocity_Measure(void)
{
    HAL_TIM_Base_Start_IT(motor.Measure_tim);
}


