#include "gpio.h"
#include "tim.h"
#include "stdio.h"
#include "Get_Key.h"
#include "Motor.h"
#include "MT6701.h"
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{ 
   
    if(htim==motor.IT_tim)
	{
        Motor_Driver();
    }
    else if (htim==motor.Measure_tim)
    {
        Motor_Velocity_Measure();
    }
    else if (htim==Key_tim)
    {
        Key_Scan();
    }
}



