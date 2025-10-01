#include "gpio.h"
#include "test.h"
#include "ST7735.h"
MPU_Device MPU6050={
    .LED=LED_200ms,
    .Value1=0,
};
MPU_Device MPU6000={
    .LED=LED_500ms,
    .Value1=0,
};
void LED_200ms(void)
{   
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_SET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_RESET);
    HAL_Delay(100);

}
void LED_500ms(void)
{   
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_SET);
    HAL_Delay(250);
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_RESET);
    HAL_Delay(250);
}

void task_1(MPU_Device* this)
{
    this->LED();
}