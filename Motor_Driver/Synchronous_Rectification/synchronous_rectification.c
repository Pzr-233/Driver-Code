#include "gpio.h"
#include "tim.h"
#include "synchronous_rectification.h"
#include "math.h"
#include "IT_Freq.h"
#include "stdlib.h"
static float sine_table_256[256]; 
static uint32_t Velocity_IT_Freq=0;

Motor Motor_1={
    .Flag=0,
    .Phase_Flag=0,
    .Step_Count=0,
    .Buffer_idx=0,
    .Direction=0,
    .I_PID.K_p=0,
    .I_PID.K_i=0,
    .I_PID.K_d=0,
    .step_factor=MICRO_64,
};

void Motor_Init(void)
{
    Genarate_Table();
    HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Start(&htim1,TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_2);
    HAL_TIMEx_PWMN_Start(&htim1,TIM_CHANNEL_2);
    HAL_TIM_Base_Start_IT(&htim2);
    Caculate_Row_CCR(0.4);

}
void set_microstepping(MicrostepMode MICRO)
{
    Motor_1.Flag=0;   
    Motor_1.step_factor=MICRO;
}
void Set_MotorPWM_Freq(uint32_t Value)
{
    __HAL_TIM_SET_PRESCALER(&htim1,1-1);
    __HAL_TIM_SET_AUTORELOAD(&htim1,(APB1_CLOCK_FREQ/Value)-1);
    Motor_1.ARR=APB1_CLOCK_FREQ/Value;
    Motor_1.Center_Value=Motor_1.ARR/2;
}
void Set_Motor_Velocity(int16_t Velocity) //转/分钟
{
    if(Velocity>=0)
    {
        Motor_1.Direction=1;
    }
    else
    {
        Motor_1.Direction=0;
    }
    Motor_1.Velocity=Velocity;
    Velocity=abs(Velocity);
    Velocity_IT_Freq=(Velocity*FULL_STEPS_PER_REV*Motor_1.step_factor)/60;
    Set_Motor_IT_Freq(APB2,&htim2,Velocity_IT_Freq);
}
void Caculate_Row_CCR(float K_p)
{
    Motor_1.I_PID.K_p=K_p;
    Motor_1.Row_CCR=Motor_1.I_PID.K_p*Motor_1.ARR/(2.f*Sine_MAX_Value);
}

void synchronous_rectification(void)
{
#define Driver_Mode 0
#if Driver_Mode==0
    if(Motor_1.Direction==0)
        { 
            Motor_1.Step_Count++;
            switch (Motor_1.Phase_Flag)
            {
                case 0:
                __HAL_TIM_SET_COMPARE(Motor_Tim, TIM_CHANNEL_1, sine_table_256[Motor_1.Buffer_idx]*Motor_1.Row_CCR+Motor_1.Center_Value);  // AH PWM
                __HAL_TIM_SET_COMPARE(Motor_Tim, TIM_CHANNEL_2, sine_table_256[(256-256/Motor_1.step_factor)-Motor_1.Buffer_idx]*Motor_1.Row_CCR+Motor_1.Center_Value);  // BH PWM
                Motor_1.Buffer_idx+=256/Motor_1.step_factor;
                if(Motor_1.Step_Count==Motor_1.step_factor)
                {
                    Motor_1.Phase_Flag=1;
                    Motor_1.Buffer_idx=0;
                }
                break;

                case 1:
                __HAL_TIM_SET_COMPARE(Motor_Tim, TIM_CHANNEL_1, sine_table_256[(256-256/Motor_1.step_factor)-Motor_1.Buffer_idx]*Motor_1.Row_CCR+Motor_1.Center_Value);  // AH PWM
                __HAL_TIM_SET_COMPARE(Motor_Tim, TIM_CHANNEL_2, Motor_1.Center_Value-sine_table_256[Motor_1.Buffer_idx]*Motor_1.Row_CCR);  // BL PWM
                Motor_1.Buffer_idx+=256/Motor_1.step_factor;
                if(Motor_1.Step_Count==2*Motor_1.step_factor)
                {
                    Motor_1.Phase_Flag=2;
                    Motor_1.Buffer_idx=0;
                }
                break;

                case 2:
                __HAL_TIM_SET_COMPARE(Motor_Tim, TIM_CHANNEL_1, Motor_1.Center_Value-sine_table_256[Motor_1.Buffer_idx]*Motor_1.Row_CCR);  // AL PWM
                __HAL_TIM_SET_COMPARE(Motor_Tim, TIM_CHANNEL_2, Motor_1.Center_Value-sine_table_256[(256-256/Motor_1.step_factor)-Motor_1.Buffer_idx]*Motor_1.Row_CCR);  // BL PWM
                Motor_1.Buffer_idx+=256/Motor_1.step_factor;
                if(Motor_1.Step_Count==3*Motor_1.step_factor)
                {
                    Motor_1.Phase_Flag=3;
                    Motor_1.Buffer_idx=0;
                }
                break;
                case 3:
                __HAL_TIM_SET_COMPARE(Motor_Tim, TIM_CHANNEL_1, Motor_1.Center_Value-sine_table_256[(256-256/Motor_1.step_factor)-Motor_1.Buffer_idx]*Motor_1.Row_CCR);  // AL PWM
                __HAL_TIM_SET_COMPARE(Motor_Tim, TIM_CHANNEL_2, sine_table_256[Motor_1.Buffer_idx]*Motor_1.Row_CCR+Motor_1.Center_Value);  // BH PWM
                Motor_1.Buffer_idx+=256/Motor_1.step_factor;
                if(Motor_1.Step_Count==4*Motor_1.step_factor)
                {
                    Motor_1.Phase_Flag=0;
                    Motor_1.Step_Count=0;      
                    Motor_1.Buffer_idx=0;
                }
                break;
            }
        }

        // else if (Motor_1.Direction==1)
        // {
        //     Motor_1.Step_Count++;
        //     if(Motor_1.Phase_Flag==0)
        //     {
        //     __HAL_TIM_SET_COMPARE(Motor_Tim, TIM_CHANNEL_2, I_P*sine_table_256[Motor_1.Buffer_idx]*ARR/32767.f);  // AL PWM
        //     __HAL_TIM_SET_COMPARE(Motor_Tim, TIM_CHANNEL_1, 0);     // AH off
        //     __HAL_TIM_SET_COMPARE(Motor_Tim, TIM_CHANNEL_3, I_P*sine_table_256[(256-256/Motor_1.step_factor)-Motor_1.Buffer_idx]*ARR/32767.f);  // BH PWM
        //     __HAL_TIM_SET_COMPARE(Motor_Tim, TIM_CHANNEL_4, 0);     // BL off
            
        //     Motor_1.Buffer_idx+=256/Motor_1.step_factor;
        //     if(Motor_1.Step_Count==Motor_1.step_factor)
        //     {
        //         Start_I_Sampling();//2350 A_MAX
        //         Motor_1.Phase_Flag=1;
        //         Motor_1.Buffer_idx=0;
        //     }
        //     }


        //     else if(Motor_1.Phase_Flag==1)
        //     {
        //     __HAL_TIM_SET_COMPARE(Motor_Tim, TIM_CHANNEL_2, I_P*sine_table_256[(256-256/Motor_1.step_factor)-Motor_1.Buffer_idx]*ARR/32767.f);  // AL PWM
        //     __HAL_TIM_SET_COMPARE(Motor_Tim, TIM_CHANNEL_1, 0);     // AH off
        //     __HAL_TIM_SET_COMPARE(Motor_Tim, TIM_CHANNEL_4, I_P*sine_table_256[Motor_1.Buffer_idx]*ARR/32767.f);  // BL PWM
        //     __HAL_TIM_SET_COMPARE(Motor_Tim, TIM_CHANNEL_3, 0);     // BH off
            
        //     Motor_1.Buffer_idx+=256/Motor_1.step_factor;
        //     if(Motor_1.Step_Count==2*Motor_1.step_factor)
        //     {
        //         // Start_I_Sampling();//1950 
        //         Motor_1.Phase_Flag=2;
        //         Motor_1.Buffer_idx=0;
        //     }
        //     }


        //     else if(Motor_1.Phase_Flag==2)
        //     {
        //     __HAL_TIM_SET_COMPARE(Motor_Tim, TIM_CHANNEL_1, I_P*sine_table_256[Motor_1.Buffer_idx]*ARR/32767.f);  // AH PWM
        //     __HAL_TIM_SET_COMPARE(Motor_Tim, TIM_CHANNEL_2, 0);     // AL off
        //     __HAL_TIM_SET_COMPARE(Motor_Tim, TIM_CHANNEL_4, I_P*sine_table_256[(256-256/Motor_1.step_factor)-Motor_1.Buffer_idx]*ARR/32767.f);  // BL PWM
        //     __HAL_TIM_SET_COMPARE(Motor_Tim, TIM_CHANNEL_3, 0);     // BH off
        //     Motor_1.Buffer_idx+=256/Motor_1.step_factor;

        //     if(Motor_1.Step_Count==3*Motor_1.step_factor)
        //     {
        //         Start_I_Sampling();//1750 A_MIN
        //         Motor_1.Phase_Flag=3;
        //         Motor_1.Buffer_idx=0;
        //     }
        //     }


        //     else if(Motor_1.Phase_Flag==3)
        //     {
        //     __HAL_TIM_SET_COMPARE(Motor_Tim, TIM_CHANNEL_1, I_P*sine_table_256[(256-256/Motor_1.step_factor)-Motor_1.Buffer_idx]*ARR/32767.f);  // AH PWM
        //     __HAL_TIM_SET_COMPARE(Motor_Tim, TIM_CHANNEL_2, 0);     // AL off
        //     __HAL_TIM_SET_COMPARE(Motor_Tim, TIM_CHANNEL_3, I_P*sine_table_256[Motor_1.Buffer_idx]*ARR/32767.f);  // BH PWM
        //     __HAL_TIM_SET_COMPARE(Motor_Tim, TIM_CHANNEL_4, 0);     // BL off
        //     Motor_1.Buffer_idx+=256/Motor_1.step_factor;
        //     if(Motor_1.Step_Count==4*Motor_1.step_factor)
        //     {
        //         // Start_I_Sampling();//2160
        //         Motor_1.Phase_Flag=0;
        //         Motor_1.Step_Count=0;      
        //         Motor_1.Buffer_idx=0;
        //     }
        //     }
        // }
#else
 if(Motor_1.Direction==0)
        { 
            Motor_1.Step_Count++;
            switch (Motor_1.Phase_Flag)
            {
                case  Motor_1.Phase_Flag==0:
                __HAL_TIM_SET_COMPARE(Motor_Tim,TIM_CHANNEL_1,)
                __HAL_TIM_SET_COMPARE(Motor_Tim, TIM_CHANNEL_1, sine_table_256[Motor_1.Buffer_idx]*Motor_1.Row_CCR);  // AH PWM
                __HAL_TIM_SET_COMPARE(Motor_Tim, TIM_CHANNEL_2, 0);     // AL off
                __HAL_TIM_SET_COMPARE(Motor_Tim, TIM_CHANNEL_3, sine_table_256[(256-256/Motor_1.step_factor)-Motor_1.Buffer_idx]*Motor_1.Row_CCR);  // BH PWM
                __HAL_TIM_SET_COMPARE(Motor_Tim, TIM_CHANNEL_4, 0);     // BL off
                Motor_1.Buffer_idx+=256/Motor_1.step_factor;
                if(Motor_1.Step_Count==Motor_1.step_factor)
                {
                    Motor_1.Phase_Flag=1;
                    Motor_1.Buffer_idx=0;
                }
                break;

                case  Motor_1.Phase_Flag==1:
                __HAL_TIM_SET_COMPARE(Motor_Tim, TIM_CHANNEL_1, sine_table_256[(256-256/Motor_1.step_factor)-Motor_1.Buffer_idx]*Motor_1.Row_CCR);  // AH PWM
                __HAL_TIM_SET_COMPARE(Motor_Tim, TIM_CHANNEL_2, 0);     // AL off
                __HAL_TIM_SET_COMPARE(Motor_Tim, TIM_CHANNEL_4, sine_table_256[Motor_1.Buffer_idx]*Motor_1.Row_CCR);  // BL PWM
                __HAL_TIM_SET_COMPARE(Motor_Tim, TIM_CHANNEL_3, 0);     // BH off
                Motor_1.Buffer_idx+=256/Motor_1.step_factor;
                if(Motor_1.Step_Count==2*Motor_1.step_factor)
                {
                    Motor_1.Phase_Flag=2;
                    Motor_1.Buffer_idx=0;
                }
                break;

                case Motor_1.Phase_Flag==2:
                __HAL_TIM_SET_COMPARE(Motor_Tim, TIM_CHANNEL_2, sine_table_256[Motor_1.Buffer_idx]*Motor_1.Row_CCR);  // AL PWM
                __HAL_TIM_SET_COMPARE(Motor_Tim, TIM_CHANNEL_1, 0);     // AH off
                __HAL_TIM_SET_COMPARE(Motor_Tim, TIM_CHANNEL_4, sine_table_256[(256-256/Motor_1.step_factor)-Motor_1.Buffer_idx]*Motor_1.Row_CCR);  // BL PWM
                __HAL_TIM_SET_COMPARE(Motor_Tim, TIM_CHANNEL_3, 0);     // BH off
                Motor_1.Buffer_idx+=256/Motor_1.step_factor;
                if(Motor_1.Step_Count==3*Motor_1.step_factor)
                {
                    Motor_1.Phase_Flag=3;
                    Motor_1.Buffer_idx=0;
                }
                break;
                case Motor_1.Phase_Flag==3:
                __HAL_TIM_SET_COMPARE(Motor_Tim, TIM_CHANNEL_2, sine_table_256[(256-256/Motor_1.step_factor)-Motor_1.Buffer_idx]*Motor_1.Row_CCR);  // AL PWM
                __HAL_TIM_SET_COMPARE(Motor_Tim, TIM_CHANNEL_1, 0);     // AH off
                __HAL_TIM_SET_COMPARE(Motor_Tim, TIM_CHANNEL_3, sine_table_256[Motor_1.Buffer_idx]*Motor_1.Row_CCR);  // BH PWM
                __HAL_TIM_SET_COMPARE(Motor_Tim, TIM_CHANNEL_4, 0);     // BL off
                Motor_1.Buffer_idx+=256/Motor_1.step_factor;
                if(Motor_1.Step_Count==4*Motor_1.step_factor)
                {
                    Motor_1.Phase_Flag=0;
                    Motor_1.Step_Count=0;      
                    Motor_1.Buffer_idx=0;
                }
                break;
            }
        }

        else if (Motor_1.Direction==1)
        {
            Motor_1.Step_Count++;
            if(Motor_1.Phase_Flag==0)
            {
            __HAL_TIM_SET_COMPARE(Motor_Tim, TIM_CHANNEL_2, I_P*sine_table_256[Motor_1.Buffer_idx]*ARR/32767.f);  // AL PWM
            __HAL_TIM_SET_COMPARE(Motor_Tim, TIM_CHANNEL_1, 0);     // AH off
            __HAL_TIM_SET_COMPARE(Motor_Tim, TIM_CHANNEL_3, I_P*sine_table_256[(256-256/Motor_1.step_factor)-Motor_1.Buffer_idx]*ARR/32767.f);  // BH PWM
            __HAL_TIM_SET_COMPARE(Motor_Tim, TIM_CHANNEL_4, 0);     // BL off
            
            Motor_1.Buffer_idx+=256/Motor_1.step_factor;
            if(Motor_1.Step_Count==Motor_1.step_factor)
            {
                Start_I_Sampling();//2350 A_MAX
                Motor_1.Phase_Flag=1;
                Motor_1.Buffer_idx=0;
            }
            }


            else if(Motor_1.Phase_Flag==1)
            {
            __HAL_TIM_SET_COMPARE(Motor_Tim, TIM_CHANNEL_2, I_P*sine_table_256[(256-256/Motor_1.step_factor)-Motor_1.Buffer_idx]*ARR/32767.f);  // AL PWM
            __HAL_TIM_SET_COMPARE(Motor_Tim, TIM_CHANNEL_1, 0);     // AH off
            __HAL_TIM_SET_COMPARE(Motor_Tim, TIM_CHANNEL_4, I_P*sine_table_256[Motor_1.Buffer_idx]*ARR/32767.f);  // BL PWM
            __HAL_TIM_SET_COMPARE(Motor_Tim, TIM_CHANNEL_3, 0);     // BH off
            
            Motor_1.Buffer_idx+=256/Motor_1.step_factor;
            if(Motor_1.Step_Count==2*Motor_1.step_factor)
            {
                // Start_I_Sampling();//1950 
                Motor_1.Phase_Flag=2;
                Motor_1.Buffer_idx=0;
            }
            }


            else if(Motor_1.Phase_Flag==2)
            {
            __HAL_TIM_SET_COMPARE(Motor_Tim, TIM_CHANNEL_1, I_P*sine_table_256[Motor_1.Buffer_idx]*ARR/32767.f);  // AH PWM
            __HAL_TIM_SET_COMPARE(Motor_Tim, TIM_CHANNEL_2, 0);     // AL off
            __HAL_TIM_SET_COMPARE(Motor_Tim, TIM_CHANNEL_4, I_P*sine_table_256[(256-256/Motor_1.step_factor)-Motor_1.Buffer_idx]*ARR/32767.f);  // BL PWM
            __HAL_TIM_SET_COMPARE(Motor_Tim, TIM_CHANNEL_3, 0);     // BH off
            Motor_1.Buffer_idx+=256/Motor_1.step_factor;

            if(Motor_1.Step_Count==3*Motor_1.step_factor)
            {
                Start_I_Sampling();//1750 A_MIN
                Motor_1.Phase_Flag=3;
                Motor_1.Buffer_idx=0;
            }
            }


            else if(Motor_1.Phase_Flag==3)
            {
            __HAL_TIM_SET_COMPARE(Motor_Tim, TIM_CHANNEL_1, I_P*sine_table_256[(256-256/Motor_1.step_factor)-Motor_1.Buffer_idx]*ARR/32767.f);  // AH PWM
            __HAL_TIM_SET_COMPARE(Motor_Tim, TIM_CHANNEL_2, 0);     // AL off
            __HAL_TIM_SET_COMPARE(Motor_Tim, TIM_CHANNEL_3, I_P*sine_table_256[Motor_1.Buffer_idx]*ARR/32767.f);  // BH PWM
            __HAL_TIM_SET_COMPARE(Motor_Tim, TIM_CHANNEL_4, 0);     // BL off
            Motor_1.Buffer_idx+=256/Motor_1.step_factor;
            if(Motor_1.Step_Count==4*Motor_1.step_factor)
            {
                // Start_I_Sampling();//2160
                Motor_1.Phase_Flag=0;
                Motor_1.Step_Count=0;      
                Motor_1.Buffer_idx=0;
            }
            }
        }
#endif  
    


}

void Motor_Driver(void)
{






}


void Genarate_Table(void)
{
 for(uint16_t i=0;i<256;i++)
 {
  sine_table_256[i]=Sine_MAX_Value*sinf((i+1)*Pi/(2.0f*256.0f));
 } 
}
