#ifndef __MOTOR_H
#define __MOTOR_H

void Genarate_Table(void);

#define M_PI 3.1415926f
#define FULL_STEPS_PER_REV 200
extern float I_P;
extern uint8_t Direction;
extern uint8_t Falg_Motor_Locking;
extern uint8_t Flag_Motor_Working;

// 细分模式定义
typedef enum {
    MICRO_4 = 4,
    MICRO_8 = 8,
    MICRO_16 = 16,
    MICRO_32 = 32,
    MICRO_64 = 64,
    MICRO_128 = 128,
    MICRO_256 = 256
} MicrostepMode;

// 控制模式定义
typedef enum {
    SPEED_CONTROL,
    POSITION_CONTROL
} ControlMode;

// PID控制器结构
typedef struct {
    float Kp;
    float Ki;
    float Kd;
    float integral;
    float prev_error;
    uint32_t last_time;
} PIDController;

// 步进电机控制结构
typedef struct {
    // 位置参数
    int32_t current_position;   // 当前位置（微步）
    int32_t target_position;    // 目标位置（微步）
    float physical_position;     // 物理位置（整步）
    
    // 运动参数
    float target_velocity;       // 目标速度 (RPM)
    float current_velocity;      // 当前速度 (RPM)
    float acceleration;          // 加速度 (RPM/s)
    
    float feedforward_factor; // 速度前馈补偿因子
    // 控制参数
    ControlMode control_mode;
    MicrostepMode step_factor;
    PIDController velocity_pid;
    PIDController position_pid;
    float current_scale;         // 电流比例 (0.0-1.0)
    
    // 硬件相关
    TIM_HandleTypeDef* pwm_tim;  // PWM定时器句柄
    TIM_HandleTypeDef* IT_tim;  //Motor定时中断句柄
    TIM_HandleTypeDef* Measure_tim; //测量定时器
    // SPI_HandleTypeDef* enc_spi;  // 编码器SPI句柄
    // ADC_HandleTypeDef* adc;      // ADC句柄
} StepperMotor;

// 全局电机实例
extern StepperMotor motor;

// 函数声明
void Motor_Init(void);
void set_microstepping(MicrostepMode mode);
void Set_Motor_Velocity(int16_t Velocity);//转/分钟
void AUTO_MICRO_Set_Motor_Velocity(uint16_t Velocity);
void Motor_Driver(void);
void Motor_Lock(float I_lock);
void Motor_UnLock(void);
void Motot_Start_Move(void);
void Set_MotorPWM_Freq(uint32_t Value);


void set_target_velocity(float rpm);
void set_target_position(int32_t position);
void apply_overmodulation(float rpm);
void low_speed_optimization(float rpm);
void update_motor_phase(void);
void position_controller_update(void);
// int32_t read_magnetic_encoder(void);
void config_deadtime(TIM_HandleTypeDef* htim, float deadtime_ns);
void pid_init(PIDController* pid, float Kp, float Ki, float Kd);
float pid_compute(PIDController* pid, float error);
// void set_phase_current(uint8_t phase, int16_t current);

#endif
