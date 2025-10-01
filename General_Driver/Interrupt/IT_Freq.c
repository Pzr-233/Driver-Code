#include "tim.h"
#include "gpio.h"
#include "IT_Freq.h"
#include "stm32f4xx_hal_tim.h"  // 确保包含此头文件

uint8_t Duty_Value=0;

void Set_Motor_IT_Freq(uint8_t APBX,TIM_HandleTypeDef *htim,uint32_t Value)
{ 
    if(APBX==APB1)
    {
    uint32_t Period=APB1_CLOCK_FREQ/Value;
    __HAL_TIM_SET_PRESCALER(htim,0);
    __HAL_TIM_SET_AUTORELOAD(htim,Period-1);
    __HAL_TIM_SET_COUNTER(htim, 0);    // 计数器归零
    TIM2->EGR = TIM_EGR_UG;
    }    
    else if(APBX==APB2)
    {
    uint32_t Period=APB2_CLOCK_FREQ/Value;
    __HAL_TIM_SET_PRESCALER(htim,0);
    __HAL_TIM_SET_AUTORELOAD(htim,Period-1);
    __HAL_TIM_SET_COUNTER(htim, 0);    // 计数器归零
    TIM2->EGR = TIM_EGR_UG;
    }
}
void Set_TIM_IT_Freq(uint8_t APBX,TIM_HandleTypeDef *htim,uint32_t Value)
{ 
    if(APBX==APB1)
    {
        if(Value<=25000)
        {
        uint32_t Prescaler=(APB1_CLOCK_FREQ/1000000)*20;
        uint32_t Period=50000/Value;
        __HAL_TIM_SET_PRESCALER(htim,Prescaler-1);
        __HAL_TIM_SET_AUTORELOAD(htim,Period-1);
        HAL_TIM_GenerateEvent(htim, TIM_EVENTSOURCE_UPDATE); // 使配置立即生效
        }
        else if(Value>25000)
        {
         uint32_t Period=APB1_CLOCK_FREQ/Value;
        __HAL_TIM_SET_PRESCALER(htim,0);
        __HAL_TIM_SET_AUTORELOAD(htim,Period-1);
        HAL_TIM_GenerateEvent(htim, TIM_EVENTSOURCE_UPDATE); // 使配置立即生效
        }
    }

    else if(APBX==APB2)
    {
        if(Value<=25000)
        {
        uint32_t Prescaler=(APB2_CLOCK_FREQ/1000000)*20;
        uint32_t Period=50000/Value;
        __HAL_TIM_SET_PRESCALER(htim,Prescaler-1);
        __HAL_TIM_SET_AUTORELOAD(htim,Period-1);
        HAL_TIM_GenerateEvent(htim, TIM_EVENTSOURCE_UPDATE); // 使配置立即生效
        }
        else if(Value>25000)
        {
        uint32_t Period=APB2_CLOCK_FREQ/Value;
        __HAL_TIM_SET_PRESCALER(htim,0);
        __HAL_TIM_SET_AUTORELOAD(htim,Period-1);
        HAL_TIM_GenerateEvent(htim, TIM_EVENTSOURCE_UPDATE); // 使配置立即生效
        }
    }
}


void Set_PWM_Freq(uint8_t APBX,TIM_HandleTypeDef *htim,uint32_t Value)
{
    if(APBX==APB1)
    {
        if(Value>10000)
    {
        __HAL_TIM_SET_PRESCALER(htim,0);
        __HAL_TIM_SetAutoreload(htim,(APB1_CLOCK_FREQ)/Value-1);

    }
    else if((Value>1000) & (Value<=10000))
    {
        uint32_t Prescaler=(APB1_CLOCK_FREQ/1000000)/10;
        __HAL_TIM_SET_PRESCALER(htim,Prescaler-1);//10MHz
        __HAL_TIM_SetAutoreload(htim,(APB1_CLOCK_FREQ/Prescaler)/Value-1);
    }
    else if(Value<=1000)
    {
        uint32_t Prescaler=(APB1_CLOCK_FREQ/1000000)*25;
        __HAL_TIM_SET_PRESCALER(htim,Prescaler-1);//40KHz
        __HAL_TIM_SetAutoreload(htim,(APB1_CLOCK_FREQ/Prescaler)/Value-1);
    }
    }
    else if(APBX==APB2)
    {
        if(Value>10000)
    {
        __HAL_TIM_SET_PRESCALER(htim,0);
        __HAL_TIM_SetAutoreload(htim,(APB2_CLOCK_FREQ)/Value-1);

    }
    else if((Value>1000) & (Value<=10000))
    {
        uint32_t Prescaler=(APB2_CLOCK_FREQ/1000000)/10;
        __HAL_TIM_SET_PRESCALER(htim,Prescaler-1);//10MHz
        __HAL_TIM_SetAutoreload(htim,(APB2_CLOCK_FREQ/Prescaler)/Value-1);
    }
    else if(Value<=1000)
    {
        uint32_t Prescaler=(APB2_CLOCK_FREQ/1000000)*25;
        __HAL_TIM_SET_PRESCALER(htim,Prescaler-1);//40KHz
        __HAL_TIM_SetAutoreload(htim,(APB2_CLOCK_FREQ/Prescaler)/Value-1);
    }
    }
}
