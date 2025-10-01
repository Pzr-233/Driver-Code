#include "gpio.h"
#include "tim.h"
#include "IT_Freq.h"
#include "Laser_Tachometer.h"
Blade Blade_1;

void Laser_Init(float Freq)
{
    Blade_1.Check_Laser_Freq=Freq+10.f;
    Blade_1.Num=3.f;
    Set_TIM_IT_Freq(APB2,Laser_CHECK_TIM,Freq);
    __HAL_TIM_SetCounter(Laser_Count_TIM,0);
    __HAL_TIM_SET_AUTORELOAD(Laser_Count_TIM,Blade_1.Num-1);
    HAL_TIM_Base_Start_IT(Laser_CHECK_TIM);
    HAL_TIM_Base_Start_IT(Laser_Count_TIM);
    Blade_1.Round_Num=0;
}

void LASER_Get_Velocity(void)
{
    if(Blade_1.Round_Num<10)
    {
    Blade_1.Count++;
    }
    else if(Blade_1.Round_Num>=10)
    {
        Blade_1.Velocity=Blade_1.Round_Num*Blade_1.Check_Laser_Freq*60.f/Blade_1.Count;
        __HAL_TIM_SetCounter(Laser_Count_TIM,0);
        Blade_1.Count=0;
        Blade_1.Round_Num=0;
    }
}

void Blade_Clear_To_Zero(void)
{
    Blade_1.Velocity=0;
}
void Set_Blade_Num(uint8_t Num)
{
    Blade_1.Num=(float)Num;
}