#ifndef __GET_KEY_H
#define __GET_KEY_H
#include "gpio.h"
#include "tim.h"
#define Key_GPIO_Port GPIOB
#define Key_1 GPIO_PIN_13
#define Key_2 GPIO_PIN_14
#define Key_3 GPIO_PIN_15
#define Key_tim &htim11

#define Key1_Pres 1
#define Key2_Pres 2
#define Key3_Pres 3

#define Key1_Long_Pres 4
#define Key2_Long_Pres 5
#define Key3_Long_Pres 6

void Key_Init(uint8_t Check_freq);
uint8_t Read_KeyState(void);
void Kry_Scan(void);

 
extern uint16_t test_falg;
extern uint8_t Key_Count;




#endif

