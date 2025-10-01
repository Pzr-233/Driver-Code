#ifndef __HX711_H
#define __HX711_H
#include "tim.h"
#define CLK_POTR GPIOA
#define CLK_GPIO GPIO_PIN_0

#define DT_POTR GPIOA
#define DT_GPIO GPIO_PIN_1

#define HX711_TIM &htim2



extern uint32_t Count;
extern uint32_t Init_Count;
extern float raw_weight,Weight,filtered_weight;


typedef struct 
{
    uint32_t Init_Buffer[5];
    float Init_Data;
    float New_Data;
    float Calibration_Factor;
    uint8_t Buffer_Index;
    uint8_t Sampling_Num;
    uint8_t HX711_Init_Flag;
}HX711;

extern HX711 Scale_1;

typedef enum
{ 
    DT_Not_Ready,
    DT_Ready
}DT_Stage;

typedef struct 
{
    float q;        // 过程噪声协方差（系统噪声）
    float r;        // 测量噪声协方差（传感器噪声）
    float x;        // 估计值
    float p;        // 估计误差协方差
    float k;        // 卡尔曼增益
}KalmanFilter;
void HX711_Set_CLK_L(void);
void HX711_Set_CLK_H(void);
DT_Stage HX711_Read_DT(void);
uint32_t HX711_Get_Count(void);
void HX711_init(void);
void Kalman_Init(KalmanFilter *kf,float q,float r,float initial_value);
float Kalman_Update(KalmanFilter *kf,float measurement);
void HX711_Data_Average(void);
float HX711_Calculate_Weight(void);






#endif 