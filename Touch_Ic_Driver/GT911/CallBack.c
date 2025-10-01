#include "gpio.h"
#include "GT911.h"
#include "usart.h"
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin==GPIO_PIN_11)
    {
        if(gt911.INT_Flag==0)
        {
        gt911.INT_Flag=1;
        }
    }

}
