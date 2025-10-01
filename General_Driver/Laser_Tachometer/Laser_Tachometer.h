#ifndef __LASER_TACHOMETER_H
#define __LASER_TACHOMETER_H
#include "tim.h"
extern uint32_t pulse_count;

#define Laser_Count_TIM &htim1
#define Laser_CHECK_TIM &htim10


typedef struct 
{
    float Check_Laser_Freq;
    float Num;
    float Velocity;
    uint16_t Round_Num;
    uint32_t Count;
}Blade;
void LASER_Get_Velocity(void);
void Set_Blade_Num(uint8_t Num);
void Laser_Init(float Freq);
void Blade_Clear_To_Zero(void);

extern Blade Blade_1;

#endif