#ifndef __SYNCHROUS_RECTIFICATION_H
#define __SYNCHROUS_RECTIFICATION_H
#include "gpio.h"
#include "tim.h"


#define FULL_STEPS_PER_REV 200

#define Motor_Tim  &htim1
#define Motor_Phase_A TIM_CHANNEL_1
#define Motor_Phase_B TIM_CHANNEL_2



#define Pi 3.1415926f
#define Sine_MAX_Value 32767.f

typedef struct 
{
    float K_p;
    float K_i;
    float K_d;
}PID;

typedef enum {
    MICRO_4 = 4,
    MICRO_8 = 8,
    MICRO_16 = 16,
    MICRO_32 = 32,
    MICRO_64 = 64,
    MICRO_128 = 128,
    MICRO_256 = 256
} MicrostepMode;

typedef struct 
{
    uint8_t Flag;
    uint8_t Direction;
    uint16_t Step_Count;
    uint16_t Buffer_idx;
    uint16_t ARR;
    float Row_CCR;
    uint16_t Center_Value;
    uint8_t  Phase_Flag;
    int32_t Velocity;
    PID I_PID;
    MicrostepMode step_factor;
}Motor;


extern Motor Motor_1;
void synchronous_rectification(void);
void Motor_Init(void);
void Set_MotorPWM_Freq(uint32_t V1alue);
void Caculate_Row_CCR(float K_p);
void Genarate_Table(void);
void Set_Motor_Velocity(int16_t Velocity);
void set_microstepping(MicrostepMode MICRO);


#endif