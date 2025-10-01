#include "gpio.h"
#include "HX711.h"
#include "delay.h"
#include "IT_Freq.h"
uint32_t Count;
uint32_t Init_Count;
uint32_t Sum;

HX711 Scale_1;

void HX711_init(void)
{
    Scale_1.Sampling_Num=0;
    Scale_1.HX711_Init_Flag=0;
    Scale_1.Init_Data=0;
    Scale_1.Calibration_Factor=0.010353;
    HX711_Set_CLK_L();
    Set_TIM_IT_Freq(APB2,HX711_TIM,10);
    HAL_TIM_Base_Start_IT(HX711_TIM);
    Scale_1.Init_Data=Scale_1.Init_Buffer[0];
} 

void HX711_Data_Average(void)
{
    Scale_1.Init_Data=0;
   for(uint8_t i=0;i<5;i++)
   {
    Scale_1.Init_Data+=Scale_1.Init_Buffer[i];
   }
    Scale_1.Init_Data=Scale_1.Init_Data/5.f;
}

void HX711_Set_CLK_L(void)
{
    HAL_GPIO_WritePin(CLK_POTR,CLK_GPIO,GPIO_PIN_RESET);//PA0 时钟线 PA1 信号输入
}

void HX711_Set_CLK_H(void)
{
    HAL_GPIO_WritePin(CLK_POTR,CLK_GPIO,GPIO_PIN_SET);
}

DT_Stage HX711_Read_DT(void)
{
    if(HAL_GPIO_ReadPin(DT_POTR,DT_GPIO)==GPIO_PIN_SET)
    {
        return DT_Not_Ready;
    }
    else return DT_Ready;
}

void Kalman_Init(KalmanFilter *kf,float q,float r,float initial_value)
{
    kf->q=q;
    kf->r=r;
    kf->x=initial_value;
    kf->p=1.0f;
}

float Kalman_Update(KalmanFilter *kf,float measurement)
{
    kf->p = kf->p + kf->q;

    // 更新步骤
    kf->k = kf->p / (kf->p + kf->r);
    kf->x = kf->x + kf->k * (measurement - kf->x);
    kf->p = (1 - kf->k) * kf->p;

    return kf->x;
}
uint32_t HX711_Get_Count(void)
{
    if (HX711_Read_DT()!=DT_Ready)
    {
     return 0;
    }
    else{
    Count=0;
    for(uint8_t i=0;i<24;i++)
        {
            HX711_Set_CLK_H();
            delay_us(1);
            Count=Count<<1;
            HX711_Set_CLK_L();
            delay_us(1);
            if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_1)==GPIO_PIN_SET) Count++;
        }
    HX711_Set_CLK_H();
    delay_us(1);
    Count=Count^0x800000;
    HX711_Set_CLK_L();    
    delay_us(1);
    return Count;
    } 
}

float HX711_Calculate_Weight(void)
{
    float Weight=0;
    if(Scale_1.HX711_Init_Flag==0)
    {
    return 0;
    }
    Weight=-((float)Scale_1.Init_Buffer[Scale_1.Buffer_Index]-Scale_1.Init_Data)*Scale_1.Calibration_Factor;
    return Weight;
    
}