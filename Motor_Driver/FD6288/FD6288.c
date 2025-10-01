#include "gpio.h"
#include "tim.h"
#include "FD6288.h"
#include "usart.h"
FD6288 FD6288_1={
    .CCR=60,
    .Step=1,
};
Receive_Data receive_data;
Transmit_Data transmit_data={
    .Direction=1,
    .Freq=500,
    .Velocity=50,
};
void FD6288_Init(void)
{   
#if(Driver_Mode==Slow_decrease)
 SET_FD6288_Tim_1_CH1_Polarity_H();
 SET_FD6288_Tim_1_CH2_Polarity_L();
 SET_FD6288_Tim_1_CH3_Polarity_H();
 SET_FD6288_Tim_1_CH4_Polarity_L();
 SET_FD6288_Tim_2_CH3_Polarity_H();
 SET_FD6288_Tim_2_CH4_Polarity_L();
#endif
HAL_TIM_PWM_Start(FD6288_Tim_1,TIM_CHANNEL_1);
HAL_TIM_PWM_Start(FD6288_Tim_1,TIM_CHANNEL_2);
HAL_TIM_PWM_Start(FD6288_Tim_1,TIM_CHANNEL_3);
HAL_TIM_PWM_Start(FD6288_Tim_1,TIM_CHANNEL_4);
HAL_TIM_PWM_Start(FD6288_Tim_2,TIM_CHANNEL_3);
HAL_TIM_PWM_Start(FD6288_Tim_2,TIM_CHANNEL_4);
__HAL_TIM_SetCounter(FD6288_Tim_1,0);
__HAL_TIM_SetCounter(FD6288_Tim_2,0);

    // HAL_TIM_OC_Start(FD6288_Tim_1,TIM_CHANNEL_1);


}
void FD6288_Driver(void)
{
#if(Driver_Mode==Slow_decrease)
switch (FD6288_1.Step)
    {
    case 1 :
        SET_Phase_A_H(FD6288_1.CCR);
        SET_Phase_B_L();
        SET_Phase_C_Floating();
    break;
    case 2 :
        SET_Phase_A_H(FD6288_1.CCR);
        SET_Phase_C_L();
        SET_Phase_B_Floating();
    break;
    case 3 :
        SET_Phase_B_H(FD6288_1.CCR);
        SET_Phase_C_L();
        SET_Phase_A_Floating();
    break;
    case 4 :
        SET_Phase_B_H(FD6288_1.CCR);
        SET_Phase_A_L();
        SET_Phase_C_Floating();
    break;
    case 5 :
        SET_Phase_C_H(FD6288_1.CCR);
        SET_Phase_A_L();
        SET_Phase_B_Floating();
    break;
    case 6 :
        SET_Phase_C_H(FD6288_1.CCR);
        SET_Phase_B_L();
        SET_Phase_A_Floating();
    default:
    break;
    }
#elif(Driver_Mode==Qucik_decrease)
switch (FD6288_1.Step)
    {
    case 1 :
        SET_Phase_A_H(FD6288_1.CCR);
        SET_Phase_B_L(FD6288_1.CCR);
        SET_Phase_C_Floating();
    break;
    case 2 :
        SET_Phase_A_H(FD6288_1.CCR);
        SET_Phase_C_L(FD6288_1.CCR);
        SET_Phase_B_Floating();
    break;
    case 3 :
        SET_Phase_B_H(FD6288_1.CCR);
        SET_Phase_C_L(FD6288_1.CCR);
        SET_Phase_A_Floating();
    break;
    case 4 :
        SET_Phase_B_H(FD6288_1.CCR);
        SET_Phase_A_L(FD6288_1.CCR);
        SET_Phase_C_Floating();
    break;
    case 5 :
        SET_Phase_C_H(FD6288_1.CCR);
        SET_Phase_A_L(FD6288_1.CCR);
        SET_Phase_B_Floating();
    break;
    case 6 :
        SET_Phase_C_H(FD6288_1.CCR);
        SET_Phase_B_L(FD6288_1.CCR);
        SET_Phase_A_Floating();
    default:
    break;
    }
#endif
}

void FD6288_Receive_Data(Receive_Data *Data,uint8_t Size)
{
    HAL_UART_Abort(&huart6);
    HAL_HalfDuplex_EnableReceiver(&huart6);
    HAL_UART_Receive_DMA(&huart6, (uint8_t *)Data, Size);
}

void FD6288_Transmit_Data(Transmit_Data *Data,uint8_t Size)
{
    HAL_UART_Abort(&huart6);
    HAL_HalfDuplex_EnableTransmitter(&huart6);
    HAL_UART_Transmit_DMA(&huart6,(uint8_t *)Data,Size); 
}