#include "gpio.h"
#include "MT6701.h"
#include "Tim.h"
#include "math.h"
// #include "Get_Key.h"
#include <stdlib.h> 
#include "Motor.h"
#include "IT_Freq.h"
#include "I_Samping.h"
StepperMotor motor;
static uint8_t STEP_Flag=0;
static uint32_t idx=0;
static uint32_t Falg=0;
static uint32_t Velocity_IT_Freq=0;
uint8_t Direction=0;
float I_P=0.2;
uint32_t ARR=0;

static float sine_table_256[256]; 
static float I_Lock_max=0.15;
uint8_t Falg_Motor_Locking=0;
uint8_t Flag_Motor_Working=0;

void Genarate_Table(void)
{
 for(uint16_t i=0;i<256;i++)
 {
  sine_table_256[i]=32767.0f*sinf((i+1)*M_PI/(2.0f*256.0f));
 } 
}


void Set_MotorPWM_Freq(uint32_t Value)
{
    __HAL_TIM_SET_PRESCALER(&htim1,1-1);
    __HAL_TIM_SET_AUTORELOAD(&htim1,(APB1_CLOCK_FREQ/Value)-1);
    ARR=__HAL_TIM_GET_AUTORELOAD(&htim1);
}

void Motor_Driver(void)
{
    if(Direction==0)
    { 
        Falg++;
        if(STEP_Flag==0)
        {
        __HAL_TIM_SET_COMPARE(motor.pwm_tim, TIM_CHANNEL_1, I_P*sine_table_256[idx]*ARR/32767.f);  // AH PWM
        __HAL_TIM_SET_COMPARE(motor.pwm_tim, TIM_CHANNEL_2, 0);     // AL off
        __HAL_TIM_SET_COMPARE(motor.pwm_tim, TIM_CHANNEL_3, I_P*sine_table_256[(256-256/motor.step_factor)-idx]*ARR/32767.f);  // BH PWM
        __HAL_TIM_SET_COMPARE(motor.pwm_tim, TIM_CHANNEL_4, 0);     // BL off
        idx+=256/motor.step_factor;
        if(Falg==motor.step_factor)
        {
            STEP_Flag=1;
            idx=0;
        }
        }


        else if(STEP_Flag==1)
        {
        
        __HAL_TIM_SET_COMPARE(motor.pwm_tim, TIM_CHANNEL_1, I_P*sine_table_256[(256-256/motor.step_factor)-idx]*ARR/32767.f);  // AH PWM
        __HAL_TIM_SET_COMPARE(motor.pwm_tim, TIM_CHANNEL_2, 0);     // AL off
        __HAL_TIM_SET_COMPARE(motor.pwm_tim, TIM_CHANNEL_4, I_P*sine_table_256[idx]*ARR/32767.f);  // BL PWM
        __HAL_TIM_SET_COMPARE(motor.pwm_tim, TIM_CHANNEL_3, 0);     // BH off
        idx+=256/motor.step_factor;
        if(Falg==2*motor.step_factor)
        {
            STEP_Flag=2;
            idx=0;
        }
        }


         else if(STEP_Flag==2)
        {
        __HAL_TIM_SET_COMPARE(motor.pwm_tim, TIM_CHANNEL_2, I_P*sine_table_256[idx]*ARR/32767.f);  // AL PWM
        __HAL_TIM_SET_COMPARE(motor.pwm_tim, TIM_CHANNEL_1, 0);     // AH off
        __HAL_TIM_SET_COMPARE(motor.pwm_tim, TIM_CHANNEL_4, I_P*sine_table_256[(256-256/motor.step_factor)-idx]*ARR/32767.f);  // BL PWM
        __HAL_TIM_SET_COMPARE(motor.pwm_tim, TIM_CHANNEL_3, 0);     // BH off
        idx+=256/motor.step_factor;

        if(Falg==3*motor.step_factor)
        {
            STEP_Flag=3;
            idx=0;
        }
        }


         else if(STEP_Flag==3)
        {
        __HAL_TIM_SET_COMPARE(motor.pwm_tim, TIM_CHANNEL_2, I_P*sine_table_256[(256-256/motor.step_factor)-idx]*ARR/32767.f);  // AL PWM
        __HAL_TIM_SET_COMPARE(motor.pwm_tim, TIM_CHANNEL_1, 0);     // AH off
        __HAL_TIM_SET_COMPARE(motor.pwm_tim, TIM_CHANNEL_3, I_P*sine_table_256[idx]*ARR/32767.f);  // BH PWM
        __HAL_TIM_SET_COMPARE(motor.pwm_tim, TIM_CHANNEL_4, 0);     // BL off
        idx+=256/motor.step_factor;
        if(Falg==4*motor.step_factor)
        {
            STEP_Flag=0;
            Falg=0;      
            idx=0;
        }
        }
        }   
    else if (Direction==1)
    {
        Falg++;
        if(STEP_Flag==0)
        {
        __HAL_TIM_SET_COMPARE(motor.pwm_tim, TIM_CHANNEL_2, I_P*sine_table_256[idx]*ARR/32767.f);  // AL PWM
        __HAL_TIM_SET_COMPARE(motor.pwm_tim, TIM_CHANNEL_1, 0);     // AH off
       
        __HAL_TIM_SET_COMPARE(motor.pwm_tim, TIM_CHANNEL_3, I_P*sine_table_256[(256-256/motor.step_factor)-idx]*ARR/32767.f);  // BH PWM
        __HAL_TIM_SET_COMPARE(motor.pwm_tim, TIM_CHANNEL_4, 0);     // BL off
        
        idx+=256/motor.step_factor;
        if(Falg==motor.step_factor)
        {
            Start_I_Sampling();//2350 A_MAX
            STEP_Flag=1;
            idx=0;
        }
        }


         else if(STEP_Flag==1)
        {
        __HAL_TIM_SET_COMPARE(motor.pwm_tim, TIM_CHANNEL_2, I_P*sine_table_256[(256-256/motor.step_factor)-idx]*ARR/32767.f);  // AL PWM
        __HAL_TIM_SET_COMPARE(motor.pwm_tim, TIM_CHANNEL_1, 0);     // AH off
        __HAL_TIM_SET_COMPARE(motor.pwm_tim, TIM_CHANNEL_4, I_P*sine_table_256[idx]*ARR/32767.f);  // BL PWM
        __HAL_TIM_SET_COMPARE(motor.pwm_tim, TIM_CHANNEL_3, 0);     // BH off
        
        idx+=256/motor.step_factor;
        if(Falg==2*motor.step_factor)
        {
            // Start_I_Sampling();//1950 
            STEP_Flag=2;
            idx=0;
        }
        }


         else if(STEP_Flag==2)
        {
        __HAL_TIM_SET_COMPARE(motor.pwm_tim, TIM_CHANNEL_1, I_P*sine_table_256[idx]*ARR/32767.f);  // AH PWM
        __HAL_TIM_SET_COMPARE(motor.pwm_tim, TIM_CHANNEL_2, 0);     // AL off
        __HAL_TIM_SET_COMPARE(motor.pwm_tim, TIM_CHANNEL_4, I_P*sine_table_256[(256-256/motor.step_factor)-idx]*ARR/32767.f);  // BL PWM
        __HAL_TIM_SET_COMPARE(motor.pwm_tim, TIM_CHANNEL_3, 0);     // BH off
        idx+=256/motor.step_factor;

        if(Falg==3*motor.step_factor)
        {
            Start_I_Sampling();//1750 A_MIN
            STEP_Flag=3;
            idx=0;
        }
        }


         else if(STEP_Flag==3)
        {
        __HAL_TIM_SET_COMPARE(motor.pwm_tim, TIM_CHANNEL_1, I_P*sine_table_256[(256-256/motor.step_factor)-idx]*ARR/32767.f);  // AH PWM
        __HAL_TIM_SET_COMPARE(motor.pwm_tim, TIM_CHANNEL_2, 0);     // AL off
        __HAL_TIM_SET_COMPARE(motor.pwm_tim, TIM_CHANNEL_3, I_P*sine_table_256[idx]*ARR/32767.f);  // BH PWM
        __HAL_TIM_SET_COMPARE(motor.pwm_tim, TIM_CHANNEL_4, 0);     // BL off
        idx+=256/motor.step_factor;
        if(Falg==4*motor.step_factor)
        {
            // Start_I_Sampling();//2160
            STEP_Flag=0;
            Falg=0;      
            idx=0;
        }
        }
    }
}
void Motor_Init(void) {
    // 3. 电流校准
    // 实际应用中需要根据硬件实现电流校准
    // calibrate_current_sensors();
    // 4. 电机参数初始化
    motor.physical_position = 0.0f;
    motor.current_position = 0;
    motor.target_position = 0;
    motor.target_velocity = 0.0f;
    motor.current_velocity = 0.0f;
    motor.acceleration = 3000.0f; // 默认3000 RPM/s加速度
    motor.current_scale = 1.5f;   // 80%最大电流
    
    // 5. 设置硬件句柄
    motor.pwm_tim = &htim1;  // TIM1用于PWM
    motor.IT_tim =&htim2;
    motor.Measure_tim=&htim3;
    // motor.enc_spi = &hspi1;  // SPI1用于编码器
    // motor.adc = &hadc1;      // ADC1用于电流检测  
    // 7. 初始设置为128细分
    set_microstepping(MICRO_8);
    // 8. 配置死区时间（500ns）    
    // 9. 启动PWM
 
    HAL_TIM_PWM_Start(motor.pwm_tim, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(motor.pwm_tim, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(motor.pwm_tim, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(motor.pwm_tim, TIM_CHANNEL_4);


    // 11. 启用H桥
    // HAL_GPIO_WritePin(ENABLE_GPIO_Port, ENABLE_Pin, GPIO_PIN_SET);
}
void Motot_Start_Move(void)
{
    Flag_Motor_Working=1;
    HAL_TIM_Base_Start_IT(motor.IT_tim);
}
void Set_Motor_Velocity(int16_t Velocity) //转/分钟
{
    if(Velocity>=0)
    {
        Direction=1;
    }
    else
    {
        Direction=0;
    }
    Velocity=abs(Velocity);
    Velocity_IT_Freq=(Velocity*FULL_STEPS_PER_REV*motor.step_factor)/60;
    Set_Motor_IT_Freq(APB2,motor.IT_tim,Velocity_IT_Freq);
}
void AUTO_MICRO_Set_Motor_Velocity(uint16_t Velocity)
{
    if(Velocity<=1)
    {   set_microstepping(MICRO_256);
        Velocity=1;
        Velocity_IT_Freq=(Velocity*FULL_STEPS_PER_REV*motor.step_factor)/60;
        I_P=0.2;
        Set_TIM_IT_Freq(APB1,motor.IT_tim,Velocity_IT_Freq);
    }
    else if ((Velocity>1)&(Velocity<=93))
    {
        set_microstepping(MICRO_256);
        Velocity_IT_Freq=(Velocity*FULL_STEPS_PER_REV*motor.step_factor)/60;
        I_P=0.1;
        Set_TIM_IT_Freq(APB1,motor.IT_tim,Velocity_IT_Freq);
    }
    else if ((Velocity>93)&(Velocity<=187))
    {
        set_microstepping(MICRO_128);
        Velocity_IT_Freq=(Velocity*FULL_STEPS_PER_REV*motor.step_factor)/60;
        I_P=0.3;
        Set_TIM_IT_Freq(APB1,motor.IT_tim,Velocity_IT_Freq);
    }
    else if ((Velocity>187)&(Velocity<=375))
    {
        set_microstepping(MICRO_64);
        Velocity_IT_Freq=(Velocity*FULL_STEPS_PER_REV*motor.step_factor)/60;
        I_P=0.5;
        Set_TIM_IT_Freq(APB1,motor.IT_tim,Velocity_IT_Freq);

    }
    else if ((Velocity>375)&(Velocity<=750))
    {
        set_microstepping(MICRO_32);
        Velocity_IT_Freq=(Velocity*FULL_STEPS_PER_REV*motor.step_factor)/60;
        I_P=1.2;
        Set_TIM_IT_Freq(APB1,motor.IT_tim,Velocity_IT_Freq);

    }
    else if ((Velocity>750)&(Velocity<=1500))
    {
        set_microstepping(MICRO_16);
        Velocity_IT_Freq=(Velocity*FULL_STEPS_PER_REV*motor.step_factor)/60;
        I_P=1.4;
        Set_TIM_IT_Freq(APB1,motor.IT_tim,Velocity_IT_Freq);

    }
    else if ((Velocity>1500)&(Velocity<=3000))
    {
        set_microstepping(MICRO_8);
        Velocity_IT_Freq=(Velocity*FULL_STEPS_PER_REV*motor.step_factor)/60;
        I_P=1.6;
        Set_TIM_IT_Freq(APB1,motor.IT_tim,Velocity_IT_Freq);
    }
   
}

void Motor_Lock(float I_lock)
{
    HAL_TIM_Base_Stop_IT(&htim2);
    Flag_Motor_Working=0;
    Falg_Motor_Locking=1;
    __HAL_TIM_SET_COMPARE(motor.pwm_tim, TIM_CHANNEL_1, (I_lock*I_Lock_max/100)*ARR);  // AH PWM
    __HAL_TIM_SET_COMPARE(motor.pwm_tim, TIM_CHANNEL_2, 0);     // AL off
    __HAL_TIM_SET_COMPARE(motor.pwm_tim, TIM_CHANNEL_3, (I_lock*I_Lock_max/100)*ARR);  // BH PWM
    __HAL_TIM_SET_COMPARE(motor.pwm_tim, TIM_CHANNEL_4, 0);     // BL off
}
void Motor_UnLock(void)
{
    // if(Falg_Motor_Locking==1)
    // {
    __HAL_TIM_SET_COMPARE(motor.pwm_tim, TIM_CHANNEL_1, ARR);  // AH PWM
    __HAL_TIM_SET_COMPARE(motor.pwm_tim, TIM_CHANNEL_2, ARR);     // AL off
    __HAL_TIM_SET_COMPARE(motor.pwm_tim, TIM_CHANNEL_3, ARR);  // BH PWM
    __HAL_TIM_SET_COMPARE(motor.pwm_tim, TIM_CHANNEL_4, ARR);     // BL off
    Falg_Motor_Locking=0;
    // }
}

// 设置目标位置
void set_target_position(int32_t position) {
    // 切换到位置控制模式
    motor.control_mode = POSITION_CONTROL;
    
    // 设置新的目标位置（保持当前细分设置）
    motor.target_position = position;
    
    // 根据距离自动优化细分模式
    int32_t distance = abs(position - motor.current_position);
    
    if (distance < 1000) {
        // 短距离运动使用高细分
        set_microstepping(MICRO_256);
    } else if (distance < 5000) {
        set_microstepping(MICRO_128);
    } else if (distance > 20000) {
        // 长距离运动使用低细分
        set_microstepping(MICRO_16);
    } else {
        // 默认中等细分
        set_microstepping(MICRO_64);
    }
    
    // 如果是大距离移动，准备梯形加速曲线
    if (distance > 5000) {
        // 计算到达目标所需的加速度曲线
        float distance_revs = (float)distance / (motor.step_factor * 200.0f);
        float speed = sqrtf(2.0f * motor.acceleration * distance_revs);
        
        // 限制最大速度
        const float MAX_SPEED = 2000.0f;
        speed = (speed > MAX_SPEED) ? MAX_SPEED : speed;
        
        // 设置起始速度
        motor.target_velocity = speed;
    }
}
// 高速优化（过调制技术）
void apply_overmodulation(float rpm) {
    // 设置高速模式PID参数
    pid_init(&motor.velocity_pid, 0.5f, 0.1f, 0.02f);
    
    // 应用电压前馈补偿
    float feedforward = rpm * 0.03f; // 比例因子需根据电机调整
    
    // 过调制处理（超过PWM可输出范围时提高有效电压）
    if (rpm > 800.0f) {
        motor.current_scale = 1.15f;
    } 
    else if (rpm > 1200.0f) {
        motor.current_scale = 1.3f;
    }
    motor.feedforward_factor = feedforward;
}

// 低速优化（平稳运行）
void low_speed_optimization(float rpm) {
    // 设置低速PID参数
    pid_init(&motor.velocity_pid, 0.8f, 0.05f, 0.01f);
    
    // 提高PWM频率至40kHz（减少电流纹波）
    uint32_t timer_clock = HAL_RCC_GetPCLK1Freq();
    uint16_t prescaler = 0;
    uint16_t period = (timer_clock / prescaler) / 40000 - 1; // 40kHz
    __HAL_TIM_SET_PRESCALER(motor.pwm_tim, prescaler);
    __HAL_TIM_SET_AUTORELOAD(motor.pwm_tim, period);
    __HAL_TIM_SET_COUNTER(motor.pwm_tim, 0);
    
    // 添加随机抖动抗谐振
    static uint32_t jitter_seed = 0;
    jitter_seed = jitter_seed * 1103515245 + 12345;
    int16_t jitter = (jitter_seed % 5) - 2; // ±2个微步的抖动
    
    // 更新目标位置（添加小扰动）
    motor.target_position += jitter;
}

// 更新电机相位（双向电流）
void update_motor_phase(void) {
    // 计算当前微步角度（弧度）
    uint32_t step = motor.current_position % motor.step_factor;
    float angle = 2 * M_PI * step / motor.step_factor;
    
    // 计算两相正弦值（±1.0范围）
    float sinA = sinf(angle);
    float sinB = cosf(angle);  // B相正交90度
    
    // 计算A相PWM和方向
    uint16_t pwmA;
    uint8_t dirA;
    if (sinA >= 0) {
        pwmA = (uint16_t)(fabsf(sinA) * 4095 * motor.current_scale);
        dirA = 0;
    } else {
        pwmA = (uint16_t)(fabsf(sinA) * 4095 * motor.current_scale);
        dirA = 1;
    }
    
    // 计算B相PWM和方向
    uint16_t pwmB;
    uint8_t dirB;
    if (sinB >= 0) {
        pwmB = (uint16_t)(fabsf(sinB) * 4095 * motor.current_scale);
        dirB = 0;
    } else {
        pwmB = (uint16_t)(fabsf(sinB) * 4095 * motor.current_scale);
        dirB = 1;
    }
    
    // 死区安全限幅（1%-99%占空比）
    const uint16_t MIN_PWM = 41;      // ≈1%
    const uint16_t MAX_PWM = 4054;    // ≈99%
    pwmA = (pwmA < MIN_PWM) ? MIN_PWM : (pwmA > MAX_PWM) ? MAX_PWM : pwmA;
    pwmB = (pwmB < MIN_PWM) ? MIN_PWM : (pwmB > MAX_PWM) ? MAX_PWM : pwmB;
    
    // 设置A相驱动
    if (dirA == 0) {
        // 正向模式：高侧PWM, 低侧强制低电平
        __HAL_TIM_SET_COMPARE(motor.pwm_tim, TIM_CHANNEL_1, pwmA);  // A+高侧
        __HAL_TIM_SET_COMPARE(motor.pwm_tim, TIM_CHANNEL_2, 0);     // A-低侧
    } else {
        // 负向模式：低侧PWM, 高侧强制低电平
        __HAL_TIM_SET_COMPARE(motor.pwm_tim, TIM_CHANNEL_1, 0);     // A+高侧
        __HAL_TIM_SET_COMPARE(motor.pwm_tim, TIM_CHANNEL_2, pwmA); // A-低侧
    }
    
    // 设置B相驱动
    if (dirB == 0) {
        // 正向模式：高侧PWM, 低侧强制低电平
        __HAL_TIM_SET_COMPARE(motor.pwm_tim, TIM_CHANNEL_3, pwmB);  // B+高侧
        __HAL_TIM_SET_COMPARE(motor.pwm_tim, TIM_CHANNEL_4, 0);     // B-低侧
    } else {
        // 负向模式：低侧PWM, 高侧强制低电平
        __HAL_TIM_SET_COMPARE(motor.pwm_tim, TIM_CHANNEL_3, 0);     // B+高侧
        __HAL_TIM_SET_COMPARE(motor.pwm_tim, TIM_CHANNEL_4, pwmB);  // B-低侧
    }
    
    // 更新物理位置
    motor.physical_position = (float)motor.current_position / motor.step_factor;
}
// 位置控制器更新
void position_controller_update(void) {
    // 读取实际位置
    int32_t actual_position=0;//= read_magnetic_encoder();
    
    // 位置误差
    int32_t error = motor.target_position - actual_position;
    
    // PID控制
    float control = pid_compute(&motor.position_pid, (float)error);
    
    // 更新目标位置
    motor.target_position = actual_position + (int32_t)control;
}
// 死区时间配置
void config_deadtime(TIM_HandleTypeDef* htim, float deadtime_ns) {
    // 计算死区时间计数（72MHz系统时钟）
    float clk_period = 13.888f; // ns
    uint16_t deadtime_ticks = (uint16_t)(deadtime_ns / clk_period);
    
    // 配置死区寄存器
    htim->Instance->BDTR &= ~TIM_BDTR_DTG;
    
    // 根据数值范围选择配置模式
    if (deadtime_ticks <= 127) {
        htim->Instance->BDTR |= deadtime_ticks;
    } 
    else if (deadtime_ticks <= 252) {
        deadtime_ticks = deadtime_ticks / 2 + 64;
        htim->Instance->BDTR |= deadtime_ticks;
    } 
    else if (deadtime_ticks <= 504) {
        deadtime_ticks = deadtime_ticks / 8 + 224;
        htim->Instance->BDTR |= deadtime_ticks;
    } 
    else {
        // 超过最大死区时间（约1000ns）
        htim->Instance->BDTR |= 254; // 最大死区时间
    }
    
    // 启用死区和自动输出使能
    htim->Instance->BDTR |= TIM_BDTR_MOE | TIM_BDTR_AOE;
}
// 设置细分模式
void set_microstepping(MicrostepMode MICRO) {
    // 保存当前物理位置
    // float current_physical_pos = motor.physical_position;
    // 更新细分因子
    Falg=0;   
    motor.step_factor = MICRO;
    // 转换位置，保持物理位置不变
    // motor.current_position = (int32_t)(current_physical_pos * mode);
    // motor.target_position = (int32_t)(current_physical_pos * mode);
    // motor.physical_position = current_physical_pos;
}

// PID控制器初始化
void pid_init(PIDController* pid, float Kp, float Ki, float Kd) {
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    pid->integral = 0.0f;
    pid->prev_error = 0.0f;
    pid->last_time = HAL_GetTick();
}

// PID计算函数
float pid_compute(PIDController* pid, float error)
 {
    uint32_t now = HAL_GetTick();
    float dt = (now - pid->last_time) / 1000.0f; // 转换为秒
    if (dt <= 0) dt = 0.001f; // 避免除零
    
    // 比例项
    float I_P = pid->Kp * error;
    
    // 积分项（带抗饱和）
    pid->integral += error * dt;
    // 积分限幅
    const float max_integral = 100.0f;
    if (pid->integral > max_integral) pid->integral = max_integral;
    else if (pid->integral < -max_integral) pid->integral = -max_integral;
    float i = pid->Ki * pid->integral;
    
    // 微分项
    float d = pid->Kd * (error - pid->prev_error) / dt;
    
    // 保存状态
    pid->prev_error = error;
    pid->last_time = now;
    
    return (I_P + i + d);
}

