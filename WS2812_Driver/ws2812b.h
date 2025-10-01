#ifndef  __WS2812B_H
#define  __WS2812B_H
#include "gpio.h"

#define WS2812_Port GPIOB
#define WS2812_GPIO GPIO_PIN_2
#define WS2812B_Height()  HAL_GPIO_WritePin(WS2812_Port,WS2812_GPIO,GPIO_PIN_SET)
#define WS2812B_Low()  HAL_GPIO_WritePin(WS2812_Port,WS2812_GPIO,GPIO_PIN_RESET)
#define WS2812B_Delay_Long_ns() { uint32_t count=10;while(count--);}
#define WS2812B_Delay_Short_ns() { }

void WS2812B_Set_Color(uint32_t RGB);

#endif // 
