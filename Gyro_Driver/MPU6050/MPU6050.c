#include "gpio.h"
#include "i2c.h"
#include "MPU6050.h"
#include "ICM42688.h"
#include "math.h"
#include "stdlib.h"
MPU6050 mpu6050;
MPU6050_Init_Value mpu6050_init_value;
MPU6050_Flag mpu6050_flag;
MPU6050_ANGLE_ROW_D MPU6050_Angle;
MPU6050_ACCEL_ROW_D MPU6050_Accel;
void MPU6050_Init(void)
{
    MPU6050_W_Reg(MPU6050_PWR_MGMT_1, DEVICE_RESET_ABlE); // 复位指令
    HAL_Delay(10);
    MPU6050_W_Reg(MPU6050_PWR_MGMT_1, CLK_Intenal_8MHz); // 设置时钟源
    MPU6050_W_Reg(MPU6050_GYRO_CONFIG, dps2000);         // 设置陀螺仪量程
    MPU6050_W_Reg(MPU6050_ACCEL_CONFIG, AFS_16g);        // 设置加速度计量程
    MPU6050_W_Reg(MPU6050_CONFIG, DLPF_CFG_0);           // 设置低通滤波器
    MPU6050_W_Reg(MPU6050_SMPLRT_DIV, 0x00);             // 设置采样率
    MPU6050_W_Reg(MPU6050_PWR_MGMT_2, STBY_Enable);      // 使能所有轴
    HAL_TIM_Base_Start_IT(MPU6050_TIM);
    HAL_TIM_Base_Start(&htim13);
}

void MPU6050_R_Reg(uint8_t register_add, uint8_t *register_value)
{
    HAL_I2C_Master_Transmit(MPU6050_I2C, MPU6050_ADDW, &register_add, 1, 10); // 先发送
    HAL_I2C_Master_Receive(MPU6050_I2C, MPU6050_ADDR, register_value, 1, 10); // 后接收
}
void MPU6050_W_Reg(uint8_t register_add, uint8_t register_value)
{
    HAL_I2C_Mem_Write(MPU6050_I2C, MPU6050_ADDW, register_add, I2C_MEMADD_SIZE_8BIT, &register_value, 1, 100);
}

void MPU6050_R_Angle(MPU6050_ANGLE_ROW_D *mpu6050_angle)
{
    uint8_t Addr = MPU6050_GYRO_XOUT_H_ADD; // 陀螺仪寄存器起始地址
    int16_t temp_x, temp_y, temp_z = 0;     // 定义临时变量
    float Limit       = 5;
    float Sample_Time = 0;
    HAL_I2C_Master_Transmit(MPU6050_I2C, MPU6050_ADDW, &Addr, 1, 10);
    HAL_I2C_Master_Receive(MPU6050_I2C, MPU6050_ADDR, (uint8_t *)mpu6050_angle, 6, 10);
    Sample_Time = (float)__HAL_TIM_GetCounter(&htim13) * MPU6050_Count_Tim_LSB;
    __HAL_TIM_SetCounter(&htim13, 0);
    temp_x = (mpu6050_angle->GYRO_XOUT_H << 8) | mpu6050_angle->GYRO_XOUT_L; /*组合数据*/
    temp_y = (mpu6050_angle->GYRO_YOUT_H << 8) | mpu6050_angle->GYRO_YOUT_L;
    temp_z = (mpu6050_angle->GYRO_ZOUT_H << 8) | mpu6050_angle->GYRO_ZOUT_L;
    if (mpu6050_flag.MPU6050_GYRO_Init_Flag != 1) { // 获取初始值
        mpu6050_init_value.X_Angle_Init     = ((float)temp_x / 32767.f) * MPU6050_MAX_GYRO;
        mpu6050_init_value.Y_Angle_Init     = ((float)temp_y / 32767.f) * MPU6050_MAX_GYRO;
        mpu6050_init_value.Z_Angle_Init     = ((float)temp_z / 32767.f) * MPU6050_MAX_GYRO;
        mpu6050_flag.MPU6050_GYRO_Init_Flag = 1;
    }
    mpu6050.X_Dps = ((float)temp_x / 32767.f) * MPU6050_MAX_GYRO - mpu6050_init_value.X_Angle_Init; // 计算各轴角速度数据
    mpu6050.Y_Dps = ((float)temp_y / 32767.f) * MPU6050_MAX_GYRO - mpu6050_init_value.Y_Angle_Init;
    mpu6050.Z_Dps = ((float)temp_z / 32767.f) * MPU6050_MAX_GYRO - mpu6050_init_value.Z_Angle_Init;
    if (fabsf(mpu6050.X_Dps) < Limit) mpu6050.X_Dps = 0; // 过滤微小噪声
    if (fabsf(mpu6050.Y_Dps) < Limit) mpu6050.Y_Dps = 0;
    if (fabsf(mpu6050.Z_Dps) < Limit) mpu6050.Z_Dps = 0;

    mpu6050.X_Angle += mpu6050.X_Dps * Sample_Time; // 对角速度积分求角度
    mpu6050.Y_Angle += mpu6050.Y_Dps * Sample_Time;
    mpu6050.Z_Angle += mpu6050.Z_Dps * Sample_Time;

    if (mpu6050.X_Angle > 180) { // 确定显示范围
        mpu6050.X_Angle = -180;
    } else if (mpu6050.X_Angle < -180) {
        mpu6050.X_Angle = 180;
    }

    // if (mpu6050.Y_Angle > 180) {
    //     mpu6050.Y_Angle = -180;
    // } else if (mpu6050.Y_Angle < -180) {
    //     mpu6050.Y_Angle = 180;
    // }

    if (mpu6050.Z_Angle > 360) {
        mpu6050.Z_Angle = 0;
    } else if (mpu6050.Z_Angle < 0) {
        mpu6050.Z_Angle = 360;
    }
}

void MPU6050_R_Accel(MPU6050_ACCEL_ROW_D *mpu6050_accel)
{

    uint8_t Addr = MPU6050_ACCEL_XOUT_H_ADD; // 加速度寄存器起始地址
    int16_t temp_x, temp_y, temp_z = 0;      // 定义临时变量

    HAL_I2C_Master_Transmit(MPU6050_I2C, MPU6050_ADDW, &Addr, 1, 10);
    HAL_I2C_Master_Receive(MPU6050_I2C, MPU6050_ADDR, (uint8_t *)mpu6050_accel, 6, 10);
    temp_x = (mpu6050_accel->ACCEL_XOUT_H << 8) | mpu6050_accel->ACCEL_XOUT_L; /*组合数据*/
    temp_y = (mpu6050_accel->ACCEL_YOUT_H << 8) | mpu6050_accel->ACCEL_YOUT_L;
    temp_z = (mpu6050_accel->ACCEL_ZOUT_H << 8) | mpu6050_accel->ACCEL_ZOUT_L;
    if (mpu6050_flag.MPU6050_Accel_Init_Flag != 1) { // 获取初始值
        mpu6050_init_value.X_Accel_Init      = ((float)temp_x / 32767.f) * MPU6050_MAX_Accel;
        mpu6050_init_value.Y_Accel_Init      = ((float)temp_y / 32767.f) * MPU6050_MAX_Accel;
        mpu6050_init_value.Z_Accel_Init      = ((float)temp_z / 32767.f) * MPU6050_MAX_Accel;
        mpu6050.X_Angle                      = atan2f(mpu6050_init_value.Y_Accel_Init, mpu6050_init_value.Z_Accel_Init) * 180.f / PI;
        mpu6050.Y_Angle                      = atan2f(-mpu6050_init_value.X_Accel_Init, sqrtf(mpu6050_init_value.Y_Accel_Init * mpu6050_init_value.Y_Accel_Init + mpu6050_init_value.Z_Accel_Init * mpu6050_init_value.Z_Accel_Init)) * 180.f / PI;
        mpu6050_flag.MPU6050_Accel_Init_Flag = 1;
    }
    mpu6050.X_Accel = ((float)temp_x / 32767.f) * MPU6050_MAX_Accel; // 计算数据
    mpu6050.Y_Accel = ((float)temp_y / 32767.f) * MPU6050_MAX_Accel;
    mpu6050.Z_Accel = ((float)temp_z / 32767.f) * MPU6050_MAX_Accel;

    mpu6050.X_Accel_To_Angle = atan2f(mpu6050.Y_Accel, mpu6050.Z_Accel) * 180.f / PI;
    mpu6050.Y_Accel_To_Angle = atan2f(-mpu6050.X_Accel, sqrtf(mpu6050.Y_Accel * mpu6050.Y_Accel + mpu6050.Z_Accel * mpu6050.Z_Accel)) * 180.f / PI;
}
