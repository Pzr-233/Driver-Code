#ifndef __FD6288_H
#define __FD6288_H
#include "gpio.h"
#include "tim.h"

typedef struct 
{
int8_t Step;
int16_t CCR;
}FD6288;
extern FD6288 FD6288_1;

typedef struct 
{
uint16_t Velocity;
uint16_t Motor_V;
uint16_t Motor_I;
uint8_t Motor_State;
uint16_t Count_Flag;
uint16_t Count;
}Receive_Data;
extern Receive_Data receive_data;

typedef struct 
{
uint16_t Freq;
uint16_t Velocity;
uint8_t Direction;
}Transmit_Data;
extern Transmit_Data transmit_data;

#define I_Time 1048

#define  Qucik_decrease 0
#define  Slow_decrease  1
#define  Driver_Mode    Slow_decrease

#define SET_FD6288_Tim_1_CH1_Polarity_H() do{*FD6288_Tim_1.Instance->CCER&=~TIM_CCER_CC1P;}while(0)
#define SET_FD6288_Tim_1_CH1_Polarity_L() do{*FD6288_Tim_1.Instance->CCER|=TIM_CCER_CC1P;}while(0)
#define SET_FD6288_Tim_1_CH2_Polarity_H() do{*FD6288_Tim_1.Instance->CCER&=~TIM_CCER_CC2P;}while(0)
#define SET_FD6288_Tim_1_CH2_Polarity_L() do{*FD6288_Tim_1.Instance->CCER|=TIM_CCER_CC2P;}while(0)
#define SET_FD6288_Tim_1_CH3_Polarity_H() do{*FD6288_Tim_1.Instance->CCER&=~TIM_CCER_CC3P;}while(0)
#define SET_FD6288_Tim_1_CH3_Polarity_L() do{*FD6288_Tim_1.Instance->CCER|=TIM_CCER_CC3P;}while(0)
#define SET_FD6288_Tim_1_CH4_Polarity_H() do{*FD6288_Tim_1.Instance->CCER&=~TIM_CCER_CC4P;}while(0)
#define SET_FD6288_Tim_1_CH4_Polarity_L() do{*FD6288_Tim_1.Instance->CCER|=TIM_CCER_CC4P;}while(0)
#define SET_FD6288_Tim_2_CH3_Polarity_H() do{*FD6288_Tim_2.Instance->CCER&=~TIM_CCER_CC3P;}while(0)
#define SET_FD6288_Tim_2_CH3_Polarity_L() do{*FD6288_Tim_2.Instance->CCER|=TIM_CCER_CC3P;}while(0)
#define SET_FD6288_Tim_2_CH4_Polarity_H() do{*FD6288_Tim_2.Instance->CCER&=~TIM_CCER_CC4P;}while(0)
#define SET_FD6288_Tim_2_CH4_Polarity_L() do{*FD6288_Tim_2.Instance->CCER|=TIM_CCER_CC4P;}while(0)

#define FD6288_Tim_1 &htim1
#define FD6288_Tim_2 &htim3
#if(Driver_Mode==Slow_decrease)
#define SET_Phase_A_Floating() do{*FD6288_Tim_1.Instance->CCR1=0;*FD6288_Tim_1.Instance->CCR2=*FD6288_Tim_1.Instance->ARR+1;}while(0)
#define SET_Phase_B_Floating() do{*FD6288_Tim_1.Instance->CCR3=0;*FD6288_Tim_1.Instance->CCR4=*FD6288_Tim_1.Instance->ARR+1;}while(0)
#define SET_Phase_C_Floating() do{*FD6288_Tim_2.Instance->CCR3=0;*FD6288_Tim_2.Instance->CCR4=*FD6288_Tim_2.Instance->ARR+1;}while(0)

#define SET_Phase_A_H(TIM_CCR) do{*FD6288_Tim_1.Instance->CCR1=TIM_CCR;*FD6288_Tim_1.Instance->CCR2=TIM_CCR;}while(0)
#define SET_Phase_A_L()        do{*FD6288_Tim_1.Instance->CCR1=0;*FD6288_Tim_1.Instance->CCR2=0;}while(0)

#define SET_Phase_B_H(TIM_CCR) do{*FD6288_Tim_1.Instance->CCR3=TIM_CCR;*FD6288_Tim_1.Instance->CCR4=TIM_CCR;}while(0)
#define SET_Phase_B_L()        do{*FD6288_Tim_1.Instance->CCR3=0;*FD6288_Tim_1.Instance->CCR4=0;}while(0)

#define SET_Phase_C_H(TIM_CCR) do{*FD6288_Tim_2.Instance->CCR3=TIM_CCR;*FD6288_Tim_2.Instance->CCR4=TIM_CCR;}while(0)
#define SET_Phase_C_L()        do{*FD6288_Tim_2.Instance->CCR3=0;*FD6288_Tim_2.Instance->CCR4=0;}while(0)

#elif(Driver_Mode==Qucik_decrease)
#define SET_Phase_A_Floating() do{SET_FD6288_Tim_1_CH1_Polarity_H();SET_FD6288_Tim_1_CH2_Polarity_H();*FD6288_Tim_1.Instance->CCR1=0;*FD6288_Tim_1.Instance->CCR2=0;}while(0)
#define SET_Phase_B_Floating() do{SET_FD6288_Tim_1_CH3_Polarity_H();SET_FD6288_Tim_1_CH4_Polarity_H();*FD6288_Tim_1.Instance->CCR3=0;*FD6288_Tim_1.Instance->CCR4=0;}while(0)
#define SET_Phase_C_Floating() do{SET_FD6288_Tim_2_CH3_Polarity_H();SET_FD6288_Tim_2_CH4_Polarity_H();*FD6288_Tim_2.Instance->CCR3=0;*FD6288_Tim_2.Instance->CCR4=0;}while(0)

#define SET_Phase_A_H(TIM_CCR) do{SET_FD6288_Tim_1_CH1_Polarity_H();SET_FD6288_Tim_1_CH2_Polarity_L();*FD6288_Tim_1.Instance->CCR1=TIM_CCR;*FD6288_Tim_1.Instance->CCR2=TIM_CCR;}while(0)
#define SET_Phase_A_L(TIM_CCR) do{SET_FD6288_Tim_1_CH1_Polarity_L();SET_FD6288_Tim_1_CH2_Polarity_H();*FD6288_Tim_1.Instance->CCR1=I_Time;*FD6288_Tim_1.Instance->CCR2=I_Time;}while(0)
#define SET_Phase_B_H(TIM_CCR) do{SET_FD6288_Tim_1_CH3_Polarity_H();SET_FD6288_Tim_1_CH4_Polarity_L();*FD6288_Tim_1.Instance->CCR3=TIM_CCR;*FD6288_Tim_1.Instance->CCR4=TIM_CCR;}while(0)
#define SET_Phase_B_L(TIM_CCR) do{SET_FD6288_Tim_1_CH3_Polarity_L();SET_FD6288_Tim_1_CH4_Polarity_H();*FD6288_Tim_1.Instance->CCR3=I_Time;*FD6288_Tim_1.Instance->CCR4=I_Time;}while(0)
#define SET_Phase_C_H(TIM_CCR) do{SET_FD6288_Tim_2_CH3_Polarity_H();SET_FD6288_Tim_2_CH4_Polarity_L();*FD6288_Tim_2.Instance->CCR3=TIM_CCR;*FD6288_Tim_2.Instance->CCR4=TIM_CCR;}while(0)
#define SET_Phase_C_L(TIM_CCR) do{SET_FD6288_Tim_2_CH3_Polarity_L();SET_FD6288_Tim_2_CH4_Polarity_H();*FD6288_Tim_2.Instance->CCR3=I_Time;*FD6288_Tim_2.Instance->CCR4=I_Time;}while(0)
#endif
void FD6288_Init(void);
void FD6288_Driver(void);
void FD6288_Receive_Data(Receive_Data *Data,uint8_t Size);
void FD6288_Transmit_Data(Transmit_Data *Data,uint8_t Size);

#endif // !