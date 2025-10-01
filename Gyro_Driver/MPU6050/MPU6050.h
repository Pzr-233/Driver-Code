#ifndef __MPU6050_H_
#define __MPU6050_H_
#include "i2c.h"
#include "tim.h"
/*从MPU6050中读取出的数据为三轴角速度与三轴的加速度,需对角速度积分得加速度*/
// MPU6050基本配置
#define MPU6050_I2C  &hi2c1                 // MPU6050的i2c接口
#define MPU6050_ADD  0x68                   // MPU6050地址：AD0接低电平地址为0x68，接高电平为0x69
#define MPU6050_ADDR (MPU6050_ADD << 1) | 1 // 读地址0xD1
#define MPU6050_ADDW (MPU6050_ADD << 1) | 0 // 写地址0xD0
#define MPU6050_TIM &htim12

/*定义MPU6050陀螺仪与加速度计量程*/
#define MPU6050_MAX_Accel     16.f   // 单位:+-g
#define MPU6050_MAX_GYRO      2000.f // 单位+-dps
#define MPU6050_Count_Tim_LSB 0.000001f  //定时器计数LSB,用于确定积分间隔

// MPU6050寄存器地址
#define MPU6050_PWR_MGMT_1   0x6B // 电源管理1
#define MPU6050_SMPLRT_DIV   0x19 // 采样率分频
#define MPU6050_CONFIG       0x1A // 配置
#define MPU6050_GYRO_CONFIG  0x1B // 陀螺仪配置
#define MPU6050_ACCEL_CONFIG 0x1C // 加速度计配置
#define MPU6050_INT_ENABLE   0x38 // 中断使能
#define MPU6050_PWR_MGMT_2   0x6C // 电源管理2
#define MPU6050_FIFO_EN      0x23 // FIFO使能
#define MPU6050_WHO_AM_I     0x75 // 芯片ID:0x68

// 加速度数据寄存器地址
#define MPU6050_ACCEL_XOUT_H_ADD 0x3B
#define MPU6050_ACCEL_XOUT_L_ADD 0x3C
#define MPU6050_ACCEL_YOUT_H_ADD 0x3D
#define MPU6050_ACCEL_YOUT_L_ADD 0x3E
#define MPU6050_ACCEL_ZOUT_H_ADD 0x3F
#define MPU6050_ACCEL_ZOUT_L_ADD 0x40

// 陀螺仪数据寄存器地址
#define MPU6050_GYRO_XOUT_H_ADD 0x43
#define MPU6050_GYRO_XOUT_L_ADD 0x44
#define MPU6050_GYRO_YOUT_H_ADD 0x45
#define MPU6050_GYRO_YOUT_L_ADD 0x46
#define MPU6050_GYRO_ZOUT_H_ADD 0x47
#define MPU6050_GYRO_ZOUT_L_ADD 0x48

// MPU6050寄存器配置值
typedef enum {
    CLKSEL_pos             = 0,                  // CLKSEL起始位
    CLKSEL_Mask            = 0x07 << CLKSEL_pos, // CLKSEL位掩码
    CLK_Intenal_8MHz       = 0 << CLKSEL_pos,    // 使用内部8MHz晶振
    CLK_External_32p768KHZ = 4 << CLKSEL_pos,    // 使用外部32.768KHz晶振
    CLK_External_19p2MHz   = 5 << CLKSEL_pos,    // 使用外部19.2MHz晶振

    TEMP_DIS_pos  = 3,                    // TEMP_DIS起始位
    TEMP_DIS_Mask = 0x01 << TEMP_DIS_pos, // TEMP_DIS位掩码
    TEMP_DIS      = 1 << TEMP_DIS_pos,    // 关闭温度传感器
    TEMP_ABLE     = 0 << TEMP_DIS_pos,    // 开启温度传感器

    CYCLE_pos  = 5,                 // CYCLE起始位
    CYCLE_Mask = 0x01 << CYCLE_pos, // CYCLE位掩码
    CYCLE_ABLE = 1 << CYCLE_pos,    // 开启循环模式
    CYCLE_DIS  = 0 << CYCLE_pos,    // 关闭循环模式

    SLEEP_pos  = 6,                 // SLEEP起始位
    SLEEP_Mask = 0x01 << SLEEP_pos, // SLEEP位掩码
    SLEEP_IN   = 1 << SLEEP_pos,    // 进入睡眠模式
    SLEEP_OUT  = 0 << SLEEP_pos,    // 退出睡眠模式

    DEVICE_RESET_pos  = 7,                        // DEVICE_RESET起始位
    DEVICE_RESET_Mask = 0x01 << DEVICE_RESET_pos, // DEVICE_RESET位掩码
    DEVICE_RESET_ABlE = 1 << DEVICE_RESET_pos,    // 复位设备
    DEVICE_RESET_DIS  = 0 << DEVICE_RESET_pos,    // 不复位设备
} MPU6050_PWR_MGMT_1_Value;                       // 电源管理1寄存器配置,使用时需相或组合而成

typedef enum {
    STBY_Dis    = 0x3F,     // 关闭所有轴的测量
    STBY_Enable = 0x00,     // 开启所有轴的测量
} MPU6050_PWR_MGMT_2_Value; // 电源管理2寄存器配置

typedef enum {
    DLPF_CFG_pos = 0,                 // DLPF_CFG起始位
    DLPF_CFG_0   = 0 << DLPF_CFG_pos, // Accel_SFreq=1KHz,A_BW=260Hz,Gyro_SFreq=8KHZ,G_BW=256Hz
    DLPF_CFG_1   = 1 << DLPF_CFG_pos, // Accel_SFreq=1KHz,A_BW=184Hz,Gyro_SFreq=1KHZ,G_BW=188Hz
    DLPF_CFG_2   = 2 << DLPF_CFG_pos, // Accel_SFreq=1KHz,A_BW=94Hz,Gyro_SFreq=1KHZ,G_BW=98Hz
    DLPF_CFG_3   = 3 << DLPF_CFG_pos, // Accel_SFreq=1KHz,A_BW=44Hz,Gyro_SFreq=1KHZ,G_BW=42Hz
    DLPF_CFG_4   = 4 << DLPF_CFG_pos, // Accel_SFreq=1KHz,A_BW=21Hz,Gyro_SFreq=1KHZ,G_BW=20Hz
    DLPF_CFG_5   = 5 << DLPF_CFG_pos, // Accel_SFreq=1KHz,A_BW=10Hz,Gyro_SFreq=1KHZ,G_BW=10Hz
    DLPF_CFG_6   = 6 << DLPF_CFG_pos, // Accel_SFreq=1KHz,A_BW=5Hz,Gyro_SFreq=1KHZ,G_BW=5Hz
    DLPF_CFG_7   = 7 << DLPF_CFG_pos, // Accel_SFreq=1KHz,A_BW=reserved,Gyro_SFreq=8KHZ,G_BW=reserved
} MPU6050_CONFIG_Value;               // CONFIG寄存器配置值,Accel_SFreq:加速度输出频率,A_BW:加速度带宽,Gyro_SFreq:陀螺仪输出频率,G_BW:陀螺仪带宽

typedef enum {
    SMPLRT_DIV_pos = 0,     // SMPLRT_DIV值为8位无符号数,其值根据需求选定
} MPU6050_SMPLRT_DIV_Value; // 采样率配置,采样率=陀螺仪输出率/(1+SMPLRT_DIV),SMPLRT_DIV为8位

typedef enum {
    dps_pos = 3,            // dps起始位
    dps250  = 0 << dps_pos, // 量程:+-250dps
    dps500  = 1 << dps_pos, // 量程:+-500dps
    dps1000 = 2 << dps_pos, // 量程:+-1000dps
    dps2000 = 3 << dps_pos, // 量程:+-2000dps

    xyz_g_st_pos = 5,                 // xyz_g_st_pos起始位
    xyz_g_st     = 7 << xyz_g_st_pos, // 开启XYZ三轴陀螺仪自检

} MPU6050_GYRO_CONFIG_Value; // 陀螺仪量程配置项,角速度的单位dps:Degrees Per Second,100 dps每秒100度的速度在旋转

typedef enum {
    accel_HPF_pos = 0,                  // HPF(高通滤波器)起始位
    HPF_Reset     = 0 << accel_HPF_pos, // HPF复位状态,即禁用
    HPF_5         = 1 << accel_HPF_pos, // Cut-off Frequenyc=5HZ
    HPF_2p5       = 2 << accel_HPF_pos, // Cut-off Frequenyc=2.5HZ
    HPF_1p25      = 3 << accel_HPF_pos, // Cut-off Frequenyc=1.25HZ
    HPF_0p63      = 4 << accel_HPF_pos, // Cut-off Frequenyc=0.63HZ
    HPF_Hold      = 7 << accel_HPF_pos, // Cut-off Frequenyc=0HZ,保持当前值

    AFS_SEL_pos = 3,                // AFS_SEL起始位
    AFS_2g      = 0 << AFS_SEL_pos, // 加速度量程:+-2g
    AFS_4g      = 1 << AFS_SEL_pos, // 加速度量程:+-4g
    AFS_8g      = 2 << AFS_SEL_pos, // 加速度量程:+-8g
    AFS_16g     = 3 << AFS_SEL_pos, // 加速度量程:+-16g

    xyz_a_st_pos = 5,                 // xyz_a_st_pos起始位
    xyz_a_st     = 7 << xyz_g_st_pos, // 开启XYZ三轴加速度计自检

} MPU6050_ACCEL_CONFIG_Value; // 加速度寄存器配置项,使用时需用两项相或组合而成

typedef struct
{
    uint8_t ACCEL_XOUT_H; // 寄存器地址:0x3B
    uint8_t ACCEL_XOUT_L; // 寄存器地址:0x3C
    uint8_t ACCEL_YOUT_H; // 寄存器地址:0x3D
    uint8_t ACCEL_YOUT_L; // 寄存器地址:0x3E
    uint8_t ACCEL_ZOUT_H; // 寄存器地址:0x3F
    uint8_t ACCEL_ZOUT_L; // 寄存器地址:0x40
} MPU6050_ACCEL_ROW_D;    // 接收的加速度原始数据

typedef struct
{
    uint8_t GYRO_XOUT_H; // 寄存器地址:0x43
    uint8_t GYRO_XOUT_L; // 寄存器地址:0x44
    uint8_t GYRO_YOUT_H; // 寄存器地址:0x45
    uint8_t GYRO_YOUT_L; // 寄存器地址:0x46
    uint8_t GYRO_ZOUT_H; // 寄存器地址:0x47
    uint8_t GYRO_ZOUT_L; // 寄存器地址:0x48
} MPU6050_ANGLE_ROW_D;   // 接收的陀螺仪原始数据

typedef struct
{
    float X_Angle;//横滚角
    float Y_Angle;//俯仰角
    float Z_Angle;//Z偏航角
    float X_Dps;//X轴角速度
    float Y_Dps;//Y轴角速度
    float Z_Dps;//Z轴角速度
    float X_Accel;//X轴加速度
    float Y_Accel;//Y轴加速度
    float Z_Accel;//Z轴加速度
    float X_Accel_To_Angle;//静态加速度得来得横滚角,用于数据融合
    float Y_Accel_To_Angle;//静态加速度得来得俯仰角,用于数据融合

    float X_Fusion_Angle;//数据融合后的横滚角
    float Y_Fusion_Angle;//数据融合后的俯仰角

} MPU6050; // 角度与加速度

typedef struct
{
    float X_Angle_Init;
    float Y_Angle_Init;
    float Z_Angle_Init;
    float X_Accel_Init;
    float Y_Accel_Init;
    float Z_Accel_Init;
} MPU6050_Init_Value; // 角度与加速度初始值

typedef struct MPU6050 {
    uint8_t MPU6050_GYRO_Init_Flag;
    uint8_t MPU6050_Accel_Init_Flag;
    uint8_t MPU6050_Timing_Flag;
} MPU6050_Flag;

void MPU6050_W_Reg(uint8_t register_add, uint8_t register_value);  // 向MPU6050寄存器写数据
void MPU6050_R_Reg(uint8_t register_add, uint8_t *register_value); // 向MPU6050寄存器读数据
void MPU6050_R_Angle(MPU6050_ANGLE_ROW_D *mpu6050);                // 向MPU6050读角度
void MPU6050_R_Accel(MPU6050_ACCEL_ROW_D *mpu6050);                // 向MPU6050读加速度
void MPU6050_Init(void);

extern MPU6050 mpu6050;
extern MPU6050_ANGLE_ROW_D MPU6050_Angle;
extern MPU6050_ACCEL_ROW_D MPU6050_Accel;
extern MPU6050_Flag mpu6050_flag;

#endif // !