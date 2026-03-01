#ifndef __SERVO_H__
#define __SERVO_H__
#include "tim.h"
// 接线说明：红线接电源，棕线接地，黄线pwm信号
#define SERVO_TIM1      &htim1
#define SERVO_TIM_FREQ  180000000
#define SERVO_1_CHANNEL TIM_CHANNEL_1
#define SERVO_2_CHANNEL TIM_CHANNEL_2
#define SERVO_3_CHANNEL TIM_CHANNEL_3
#define SERVO_4_CHANNEL TIM_CHANNEL_4

typedef struct
{
    TIM_HandleTypeDef *htim; // 定时器句柄
    uint32_t channel;        // 定时器通道
    uint8_t angle;           // 当前角度
    uint8_t velocity;        // 当前速度
} Servo_t;

typedef enum {
    SERVO_1,
    // SERVO_2,
    // SERVO_3,
    // SERVO_4,
    SERVO_NUM,
} servoID_e;

void Servo_Init(servoID_e servoNum);
void Servo_SetAngle(servoID_e servoNum, uint8_t angle);
void Servo_SetVelocity(servoID_e servoNum, int8_t velocity);
uint8_t Servo_GetAngle(servoID_e servoNum);
int8_t Servo_GetVelocity(servoID_e servoNum);
#endif // !