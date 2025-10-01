#ifndef __IT_Freq__
#define __IT_Freq__

#define APB1 1
#define APB2 2
#define APB1_CLOCK_FREQ 84000000
#define APB2_CLOCK_FREQ 84000000

void Set_TIM_IT_Freq(uint8_t APBX,TIM_HandleTypeDef *htim,uint32_t Value);
void Set_PWM_Freq(uint8_t APBX,TIM_HandleTypeDef *htim,uint32_t Value);
void Set_Motor_IT_Freq(uint8_t APBX,TIM_HandleTypeDef *htim,uint32_t Value);

#endif