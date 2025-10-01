#ifndef __GT911_H_
#define __GT911_H_
#include "i2c.h"

/*GT911接口硬件*/
#define GT911_I2C      &hi2c1

#define GT911_RES_Port GPIOD
#define GT911_RES_Pin  GPIO_PIN_12

#define GT911_INT_Port GPIOD
#define GT911_INT_Pin  GPIO_PIN_11

/*GT911读写地址*/
#define GT911_ADD_W 0xBA
#define GT911_ADD_R 0xBB

/*GT911寄存器地址*/
#define GT911_Command_Addr         0x8040 // 实时命令寄存器
#define GT911_Config_Version       0x8047 // 版本号
#define GT911_Product_ID_Addr      0x8140 // 产品ID寄存器
#define X_Output_Max_Low_Byte_Addr 0x8048 // 设置X触摸范围
#define X_Output_Max_How_Byte_Addr 0x8049 // 设置X触摸范围
#define Y_Output_Max_Low_Byte_Addr 0x804A // 设置Y轴触摸范围
#define Y_Output_Max_How_Byte_Addr 0x804B // 设置Y轴触摸范围
#define Touch_Number_Addr          0x804C // 设置输出触摸点数量
#define Module_Switch1_Addr        0x804D // 设置INT触发类型,0x00:上升沿,0x01:下降沿,0x02:低电平查询,0x03:高电平查询
#define Shake_Count_Addr           0x804F // 去抖次数
#define Screen_Touch_Level_Addr    0x8053 // 触摸生效阈值,8位
#define Screen_Leave_Level_Addr    0x8054 // 触摸离开阈值,8位
#define Low_Power_Control_Addr     0x8055 // 进入低功耗模式的超时时间,4位0~15s
#define Config_Chksum_Addr         0x80FF // 检验和寄存器，从0x8047~0x80FE之间所有寄存器配置字节和的补码
#define Refresh_Rate_Addr          0x8056 // 坐标上报率:5+Nms
#define Config_Fresh_Addr          0x8100 // 写入0x01时更新配置
#define GT911_State_Register_Addr  0x814E // 状态寄存器,显示是否数据更新,显示有效触摸点个数
/*读取坐标寄存器地址*/
#define Point1_Track_ID_Addr 0x814F
#define Point1_X_L_Addr      0x8150
#define Point1_X_H_Addr      0x8151
#define Point1_Y_L_Addr      0x8152
#define Point1_Y_H_Addr      0x8153
#define Point1_Size_L_Addr   0x8154 // 触摸面积
#define Point1_Size_H_Addr   0x8155
#define Point1_Reserved_Addr 0x8156 // 保留位无效

#define Point2_Track_ID_Addr 0x8157 // 点1跟踪寄存器,跟踪ID,用于区分手指,实现多点追踪
#define Point2_X_L_Addr      0x8158
#define Point2_X_H_Addr      0x8159
#define Point2_Y_L_Addr      0x815A
#define Point2_Y_H_Addr      0x815B
#define Point2_Size_L_Addr   0x815C
#define Point2_Size_H_Addr   0x815D
#define Point2_Reserved_Addr 0x815E // 保留位无效

#define Point3_Track_ID_Addr 0x815F // 点2跟踪寄存器
#define Point3_X_L_Addr      0x8160
#define Point3_X_H_Addr      0x8161
#define Point3_Y_L_Addr      0x8162
#define Point3_Y_H_Addr      0x8163
#define Point3_Size_L_Addr   0x8164
#define Point3_Size_H_Addr   0x8165
#define Point3_Reserved_Addr 0x8166 // 保留位无效

#define Point4_Track_ID_Addr 0x8167 // 点3跟踪寄存器
#define Point4_X_L_Addr      0x8168
#define Point4_X_H_Addr      0x8169
#define Point4_Y_L_Addr      0x816A
#define Point4_Y_H_Addr      0x816B
#define Point4_Size_L_Addr   0x816C
#define Point4_Size_H_Addr   0x816D
#define Point4_Reserved_Addr 0x816E // 保留位无效

#define Point5_Track_ID_Addr 0x816F // 点4跟踪寄存器
#define Point5_X_L_Addr      0x8170
#define Point5_X_H_Addr      0x8171
#define Point5_Y_L_Addr      0x8172
#define Point5_Y_H_Addr      0x8173
#define Point5_Size_L_Addr   0x8174
#define Point5_Size_H_Addr   0x8175

typedef struct
{
    uint16_t X_Max_Size;
    uint16_t Y_Max_Size;
    uint8_t Touch_Number;
    uint8_t INT_Mode;

    uint8_t Screen_Touch_Level;
    uint8_t Screen_Leave_Level;
    uint8_t Low_Power_Control;

    uint8_t Chksum;
    uint8_t INT_Flag;
    uint32_t ID;
} GT911;

typedef struct
{
    uint8_t Point1_Track_ID;
    uint8_t Point1_X_L;
    uint8_t Point1_X_H;
    uint8_t Point1_Y_L;
    uint8_t Point1_Y_H;
    uint8_t Point1_Size_L;
    uint8_t Point1_Size_H;
    uint8_t Point1_Reserved_1;

    uint8_t Point2_Track_ID;
    uint8_t Point2_X_L;
    uint8_t Point2_X_H;
    uint8_t Point2_Y_L;
    uint8_t Point2_Y_H;
    uint8_t Point2_Size_L;
    uint8_t Point2_Size_H;
    uint8_t Point2_Reserved;

    uint8_t Point3_Track_ID;
    uint8_t Point3_X_L;
    uint8_t Point3_X_H;
    uint8_t Point3_Y_L;
    uint8_t Point3_Y_H;
    uint8_t Point3_Size_L;
    uint8_t Point3_Size_H;
    uint8_t Point3_Reserved;

    uint8_t Point4_Track_ID;
    uint8_t Point4_X_L;
    uint8_t Point4_X_H;
    uint8_t Point4_Y_L;
    uint8_t Point4_Y_H;
    uint8_t Point4_Size_L;
    uint8_t Point4_Size_H;
    uint8_t Point4_Reserved;

    uint8_t Point5_Track_ID;
    uint8_t Point5_X_L;
    uint8_t Point5_X_H;
    uint8_t Point5_Y_L;
    uint8_t Point5_Y_H;
    uint8_t Point5_Size_L;
    uint8_t Point5_Size_H;
    uint8_t Point5_Reserved;

} GT911_Touch_Point_Row;

typedef struct
{
    uint16_t Point1_X;
    uint16_t Point1_Y;
    uint16_t Point1_Size;
    uint8_t Point1_Track_ID;

    uint16_t Point2_X;
    uint16_t Point2_Y;
    uint16_t Point2_Size;
    uint8_t Point2_Track_ID;

    uint16_t Point3_X;
    uint16_t Point3_Y;
    uint16_t Point3_Size;
    uint8_t Point3_Track_ID;

    uint16_t Point4_X;
    uint16_t Point4_Y;
    uint16_t Point4_Size;
    uint8_t Point4_Track_ID;

    uint16_t Point5_X;
    uint16_t Point5_Y;
    uint16_t Point5_Size;
    uint8_t Point5_Track_ID;

} GT911_Touch_Point;

extern GT911 gt911;
extern GT911_Touch_Point touch_point;
extern GT911_Touch_Point_Row touch_point_row;
void GT911_Init(void);
void GT911_Read_Register(uint16_t register_addr, uint8_t *register_value, uint8_t Size);
void GT911_Write_Register(uint16_t register_addr, uint8_t *register_value, uint8_t Size);
void GT911_Read_Product_ID(uint32_t *buffer);
void GT911_Read_Touch_Point(void);
void GT911_Calcutate_Chksum(void);
void GT911_Read_Config(void);
void Serial_SendString(char *String);
void Serial_Printf(char *format, ...);

#endif