#include "Servo.h"
#include "myMath.h"
Servo_t Servo[SERVO_NUM] = {
    [SERVO_1] = {.htim = SERVO_TIM1, .channel = SERVO_1_CHANNEL, .angle = 0, .velocity = 0},
};
void Servo_Init(servoID_e servoNum)
{
    __HAL_TIM_SET_PRESCALER(SERVO_TIM1, SERVO_TIM_FREQ / 1000000 - 1);
    __HAL_TIM_SET_AUTORELOAD(SERVO_TIM1, 20000 - 1);
    HAL_TIM_PWM_Start(SERVO_TIM1, Servo[servoNum].channel);
}

void Servo_SetAngle(servoID_e servoNum, uint8_t angle)
{
    uint32_t pulseWidth = 0;
    if (angle > 180)
        angle = 180;
    pulseWidth = 500 + ((uint32_t)angle * 2000) / 180;
    Servo[servoNum].angle = angle;
    __HAL_TIM_SET_COMPARE(SERVO_TIM1, Servo[servoNum].channel, pulseWidth);
}

void Servo_SetVelocity(servoID_e servoNum, int8_t velocity)
{
    uint32_t pulseWidth = 0;
    velocity = rangeInt(velocity, -100, 100);
    pulseWidth = 1500 - (velocity * 1000) / 100;
    Servo[servoNum].velocity = velocity;
    __HAL_TIM_SET_COMPARE(SERVO_TIM1, Servo[servoNum].channel, pulseWidth);
}

uint8_t Servo_GetAngle(servoID_e servoNum)
{
    return Servo[servoNum].angle;
}

int8_t Servo_GetVelocity(servoID_e servoNum)
{
    return Servo[servoNum].velocity;
}