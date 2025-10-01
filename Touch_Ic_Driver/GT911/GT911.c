#include "gpio.h"
#include "GT911.h"
#include "usart.h"
#include "string.h"
#include "stdio.h"
#include <stdarg.h>
#include "malloc.h"
uint8_t GT911_Config_Buffer[186];
uint8_t Data_Row[40];
GT911 gt911 = {
    .X_Max_Size         = 800,
    .Y_Max_Size         = 480,
    .Touch_Number       = 5,
    .INT_Mode           = 0x00,
    .Screen_Touch_Level = 10,
    .Screen_Leave_Level = 10,
    .Low_Power_Control  = 15,
    .INT_Flag           = 0,
};

uint8_t gt9x_cfg_tab[] = {
    /* 0x8047 */
    0x64,       // 版本号
    0x20, 0x03, // X触摸尺寸
    0xE0, 0x01, // Y触摸尺寸
    0x05,       // 输出点个数
    0x48,       // INT触发沿
    0x00,       // Reserved
    0x01,       // 去抖次数
    0x08,       // 窗口滤波系数
    0x28,       // 大面积触摸点个数
    0x05,       // 噪声消除值
    0x50, 0x32, 0x0F, 0x05,
    /* 0x8057 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x18, 0x1A, 0x20, 0x14, 0x89, 0x2A, 0x0B, 0x17, 0x15,
    /* 0x8067 */ 0x31, 0x0D, 0x00, 0x00, 0x03, 0xBA, 0x04, 0x2D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x64, 0x32,
    /* 0x8077 */ 0x00, 0x00, 0x00, 0x0F, 0x94, 0x94, 0xC5, 0x02, 0x07, 0x00, 0x00, 0x04, 0x8D, 0x13, 0x00, 0x5C,
    /* 0x8087 */ 0x1E, 0x00, 0x3C, 0x30, 0x00, 0x29, 0x4C, 0x00, 0x1F, 0x78, 0x00, 0x1F, 0x00, 0x00, 0x00, 0x00,
    /* 0x8097 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* 0x80A7 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* 0x80B7 */ 0x16, 0x14, 0x12, 0x10, 0x0E, 0x0C, 0x0A, 0x08, 0x06, 0x04, 0x02, 0xFF, 0xFF, 0xFF, 0x00, 0x00,
    /* 0x80C7 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02,
    /* 0x80D7 */ 0x04, 0x06, 0x08, 0x0A, 0x0F, 0x10, 0x12, 0x16, 0x18, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22,
    /* 0x80E7 */ 0x24, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* 0x80F7 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
GT911_Touch_Point touch_point;
GT911_Touch_Point_Row touch_point_row;
uint8_t state = 0;

void GT911_Read_Product_ID(uint32_t *ID)
{
    uint16_t Addr = GT911_Product_ID_Addr;
    Addr          = (Addr >> 8) | (Addr << 8);
    state         = HAL_I2C_Master_Transmit(GT911_I2C, GT911_ADD_W, (uint8_t *)&Addr, 2, 10);
    HAL_I2C_Master_Receive(GT911_I2C, GT911_ADD_R, (uint8_t *)ID, 4, 100);
    if (state == HAL_OK) {
        HAL_UART_Transmit(&huart4, (uint8_t *)"HAL_OK!\n", sizeof("HAL_OK!"), 10);
        HAL_UART_Transmit(&huart4, (uint8_t *)ID, 4, 100);
        HAL_UART_Transmit(&huart4, (uint8_t *)"\n", 2, 10);
    } else if (state == HAL_ERROR) {
        HAL_UART_Transmit(&huart4, (uint8_t *)"HAL_ERROR!", sizeof("HAL_ERROR!"), 10);
    } else if (state == HAL_BUSY) {
        HAL_UART_Transmit(&huart4, (uint8_t *)"HAL_BUSY!", sizeof("HAL_BUSY!"), 10);
    } else if (state == HAL_TIMEOUT) {
        HAL_UART_Transmit(&huart4, (uint8_t *)"HAL_TIMEOUT!", sizeof("HAL_TIMEOUT!"), 10);
    }
}

void GT911_Read_Register(uint16_t register_addr, uint8_t *register_value, uint8_t Size)
{
    uint16_t Addr = (register_addr >> 8) | (register_addr << 8);
    HAL_I2C_Master_Transmit(GT911_I2C, GT911_ADD_W, (uint8_t *)&Addr, 2, 100);
    HAL_I2C_Master_Receive(GT911_I2C, GT911_ADD_R, register_value, Size, 1000);
}
void GT911_Write_Register(uint16_t register_addr, uint8_t *register_value, uint8_t Size)
{
    uint8_t temp_buffer[Size + 2];
    temp_buffer[0] = register_addr >> 8;
    temp_buffer[1] = register_addr & 0xFF;
    memcpy(&temp_buffer[2], register_value, Size);
    HAL_I2C_Master_Transmit(GT911_I2C, GT911_ADD_W, temp_buffer, Size + 2, 1000);
}

void GT911_Read_Touch_Point(void)
{
    GT911_Touch_Point_Row Touch_Point_Row = {0};
    uint8_t state_value                   = 0;
    GT911_Read_Register(GT911_State_Register_Addr, &state_value, 1);
    if ((state_value & 0x80) != 0) {
        state_value &= 0x0F;
        if (state_value != 0) {
            HAL_UART_Transmit(&huart4, &state_value, 1, 100);
            GT911_Read_Register(Point1_Track_ID_Addr, (uint8_t *)&Touch_Point_Row, state_value * 8);
            touch_point.Point1_X        = (Touch_Point_Row.Point1_X_H << 8) | (Touch_Point_Row.Point1_X_L & 0xFF);
            touch_point.Point1_Y        = (Touch_Point_Row.Point1_Y_H << 8) | (Touch_Point_Row.Point1_Y_L & 0xFF);
            touch_point.Point1_Size     = (Touch_Point_Row.Point1_Size_H << 8) | (Touch_Point_Row.Point1_Size_L & 0xFF);
            touch_point.Point1_Track_ID = (Touch_Point_Row.Point1_Track_ID);

            touch_point.Point2_X        = (Touch_Point_Row.Point2_X_H << 8) | (Touch_Point_Row.Point2_X_L & 0xFF);
            touch_point.Point2_Y        = (Touch_Point_Row.Point2_Y_H << 8) | (Touch_Point_Row.Point2_Y_L & 0xFF);
            touch_point.Point2_Size     = (Touch_Point_Row.Point2_Size_H << 8) | (Touch_Point_Row.Point2_Size_L & 0xFF);
            touch_point.Point2_Track_ID = (Touch_Point_Row.Point2_Track_ID);

            touch_point.Point3_X        = (Touch_Point_Row.Point3_X_H << 8) | (Touch_Point_Row.Point3_X_L & 0xFF);
            touch_point.Point3_Y        = (Touch_Point_Row.Point3_Y_H << 8) | (Touch_Point_Row.Point3_Y_L & 0xFF);
            touch_point.Point3_Size     = (Touch_Point_Row.Point3_Size_H << 8) | (Touch_Point_Row.Point3_Size_L & 0xFF);
            touch_point.Point3_Track_ID = (Touch_Point_Row.Point3_Track_ID);

            touch_point.Point4_X        = (Touch_Point_Row.Point4_X_H << 8) | (Touch_Point_Row.Point4_X_L & 0xFF);
            touch_point.Point4_Y        = (Touch_Point_Row.Point4_Y_H << 8) | (Touch_Point_Row.Point4_Y_L & 0xFF);
            touch_point.Point4_Size     = (Touch_Point_Row.Point4_Size_H << 8) | (Touch_Point_Row.Point4_Size_L & 0xFF);
            touch_point.Point4_Track_ID = (Touch_Point_Row.Point4_Track_ID);

            touch_point.Point5_X        = (Touch_Point_Row.Point5_X_H << 8) | (Touch_Point_Row.Point5_X_L & 0xFF);
            touch_point.Point5_Y        = (Touch_Point_Row.Point5_Y_H << 8) | (Touch_Point_Row.Point5_Y_L & 0xFF);
            touch_point.Point5_Size     = (Touch_Point_Row.Point5_Size_H << 8) | (Touch_Point_Row.Point5_Size_L & 0xFF);
            touch_point.Point5_Track_ID = (Touch_Point_Row.Point5_Track_ID);
            Serial_Printf("Num:%d ", state_value);
            Serial_Printf("(%d,%d)", touch_point.Point1_X, touch_point.Point1_Y);
            Serial_Printf("(%d,%d)", touch_point.Point2_X, touch_point.Point2_Y);
            Serial_Printf("(%d,%d)", touch_point.Point3_X, touch_point.Point3_Y);
            Serial_Printf("(%d,%d)", touch_point.Point4_X, touch_point.Point4_Y);
            Serial_Printf("(%d,%d)\n", touch_point.Point5_X, touch_point.Point5_Y);
        }
    }
    uint8_t Clear = 0x00;
    GT911_Write_Register(GT911_State_Register_Addr, &Clear, 1);
}

void GT911_Calcutate_Chksum(void)
{
    for (int i = 0; i < 184; i++) {
        gt911.Chksum += gt9x_cfg_tab[i];
    }
    gt911.Chksum      = (~gt911.Chksum) + 1;
    gt9x_cfg_tab[184] = gt911.Chksum;
}

void GT911_Read_Config(void)
{
    GT911_Read_Register(GT911_Config_Version, GT911_Config_Buffer, sizeof(GT911_Config_Buffer));
    HAL_UART_Transmit(&huart4, GT911_Config_Buffer, sizeof(GT911_Config_Buffer), 1000);
}

void GT911_Init(void)
{
    HAL_GPIO_WritePin(GT911_RES_Port, GT911_RES_Pin, GPIO_PIN_SET);
    HAL_Delay(50);
    GT911_Read_Product_ID(&gt911.ID);
    if (gt911.ID == 0x313139) {
        GT911_Calcutate_Chksum();
        uint8_t temp_chksum = 0;
        GT911_Read_Register(Config_Chksum_Addr, &temp_chksum, 1);
        if (temp_chksum != gt911.Chksum) {
            GT911_Write_Register(GT911_Config_Version, gt9x_cfg_tab, sizeof(gt9x_cfg_tab));
            HAL_Delay(200);
            GT911_Read_Config();
            HAL_UART_Transmit(&huart4, (uint8_t *)"GT911 Config Compalte", sizeof("GT911 Config Compalte"), 100);
        }
    }
}
void Serial_SendString(char *String)
{
    uint8_t i;
    for (i = 0; String[i] != '\0'; i++) // 遍历字符数组（字符串），遇到字符串结束标志位后停止
    {
        HAL_UART_Transmit(&huart4, (uint8_t *)(String + i), 1, 100);
    }
}

void Serial_Printf(char *format, ...)
{
    char String[100];              // 定义字符数组
    va_list arg;                   // 定义可变参数列表数据类型的变量arg
    va_start(arg, format);         // 从format开始，接收参数列表到arg变量
    vsprintf(String, format, arg); // 使用vsprintf打印格式化字符串和参数列表到字符数组中
    va_end(arg);                   // 结束变量arg
    Serial_SendString(String);     // 串口发送字符数组（字符串）
}

void test(void)
{
    int** s_point=malloc(4*sizeof(int*));
    s_point[0]=malloc(4*sizeof(int*));
    s_point[1]=malloc(4*sizeof(int*));
    s_point[2]=malloc(4*sizeof(int*));
    s_point[3]=malloc(4*sizeof(int*));

    s_point[0][0]=1234;
    s_point[0][1]=2345;
    s_point[0][2]=3456;
    s_point[0][3]=4567;


}