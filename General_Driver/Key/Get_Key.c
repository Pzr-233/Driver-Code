#include "gpio.h"
#include "tim.h"
// #include "OLED.h"
#include "Get_Key.h"
#include "IT_Freq.h"
uint8_t Key_State;
uint8_t Key_Count=0;
uint8_t Key_slect=0;
uint16_t test_falg=0;
void Key_Init(uint8_t Check_freq)
{
    Set_TIM_IT_Freq(APB2,Key_tim,Check_freq);
    HAL_TIM_Base_Start_IT(Key_tim);
}

uint8_t Read_KeyState(void)
{
    if(Key_State==Key1_Pres)
    {
        Key_State=0;
        return Key1_Pres;
    }
    else if (Key_State==Key2_Pres)
    {
        Key_State=0;
        return Key2_Pres;
    }
     else if (Key_State==Key3_Pres)
    {
        Key_State=0;
        return Key3_Pres;
    }
     else if (Key_State==Key1_Long_Pres)
    {
        Key_State=0;
        return Key1_Long_Pres;
    }
     else if (Key_State==Key2_Long_Pres)
    {
        Key_State=0;
        return Key2_Long_Pres;
    } 
    else if (Key_State==Key3_Long_Pres)
    {
        Key_State=0;
        return Key3_Long_Pres;
    }
    else
    {
        return 0;
    }
    
}

void Kry_Scan(void)
{
   
    if(Key_slect==0)
    {
        if(HAL_GPIO_ReadPin(Key_GPIO_Port,Key_1)==GPIO_PIN_RESET)
        {
            Key_slect=1;
        }
        else if(HAL_GPIO_ReadPin(Key_GPIO_Port,Key_2)==GPIO_PIN_RESET)
        {
            Key_slect=2;
        }
        else if(HAL_GPIO_ReadPin(Key_GPIO_Port,Key_3)==GPIO_PIN_RESET)
        {
            Key_slect=3;
        }
    }
    else if (Key_slect==1)
    {
           if(HAL_GPIO_ReadPin(Key_GPIO_Port,Key_1)==GPIO_PIN_RESET)
        {
            Key_Count++;
        }
        else if (HAL_GPIO_ReadPin(Key_GPIO_Port,Key_1)==GPIO_PIN_SET)
        {
            if(Key_Count<5)
            {
                Key_State=Key1_Pres;
                Key_Count=0;
                Key_slect=0;
            }
            else if(Key_Count>=5)
            {
                Key_State=Key1_Long_Pres;
                Key_Count=0;
                Key_slect=0;
            }
        }
    }
     else if (Key_slect==2)
    {
      if(HAL_GPIO_ReadPin(Key_GPIO_Port,Key_2)==GPIO_PIN_RESET)
        {
            Key_Count++;
        }
        else if (HAL_GPIO_ReadPin(Key_GPIO_Port,Key_2)==GPIO_PIN_SET)
        {
            if(Key_Count<5)
            {
                Key_State=Key2_Pres;
                Key_Count=0;
                Key_slect=0;
            }
            else if(Key_Count>=5)
            {
                Key_State=Key2_Long_Pres;
                Key_Count=0;
                Key_slect=0;
            }
        }
    } 
    else if (Key_slect==3)
    {
       if(HAL_GPIO_ReadPin(Key_GPIO_Port,Key_3)==GPIO_PIN_RESET)
        {
            Key_Count++;
        }
        else if (HAL_GPIO_ReadPin(Key_GPIO_Port,Key_3)==GPIO_PIN_SET)
        {
            if(Key_Count<5)
            {
                Key_State=Key3_Pres;
                Key_Count=0;
                Key_slect=0;
            }
             else if(Key_Count>=5)
            {
                Key_State=Key3_Long_Pres;
                Key_Count=0;
                Key_slect=0;
            }
        }
     }
}
