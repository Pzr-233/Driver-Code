#ifndef __ST7735_H
#define __ST7735_H
#include "gpio.h"
#include "spi.h"
#include "ST7735_Data.h"

#define ST7735_Use_Tim 0  // 是否使用定时器定时执行显示任务
#define ST7735_Use_Lvgl 0 // 是否使用LVGL

#if (ST7735_Use_Tim == 1)
#include "tim.h"
#define ST7735_Tim &htim12
#endif // DEBUG

/*##########################################*/
// ST7735引脚宏定义
#define ST7735_CS_GPIOX GPIOA
#define ST7735_CS GPIO_PIN_4

#define ST7735_DC_GPIOX GPIOA
#define ST7735_DC GPIO_PIN_3

#define ST7735_RES_GPIOX GPIOC
#define ST7735_RES GPIO_PIN_4
#define ST7735_SPI &hspi1
/*##########################################*/

/*##########################################*/
// ST7735命令及其参数宏定义
#define Soft_Reset 0x01 // 软件复位命令

#define Cmd_Sleep 0x10   // 进入睡眠命令
#define Cmd_Unsleep 0x11 // 退出睡眠命令

#define Cmd_RGB 0x3A     // 设置RGB格式命令
#define Data_RGB444 0x03 // 设置为RGB444
#define Data_RGB565 0x05 // 设置为RGB565
#define Data_RGB666 0x06 // 设置为RGB666

#define Cmd_Start_View 0x29 // 开启图像显示命令
#define Cmd_Close_View 0x28 // 关闭图像显示命令

#define Cmd_Scan_Dir 0x36  // 设置扫描方向
#define Data_Normal 0xC0   // 横屏
#define Data_Unnormal 0xA8 // 竖屏

#define Cmd_Set_List 0x2A  // 设置列地址
#define Cmd_Set_Row 0x2B   // 设置行地址
#define Cmd_Write_Mem 0x2C // 启动写入显存命令

#define Cmd_Set_GAMMA 0x26 // 伽马设置命令
#define Data_GAMMA_1 0x01  // 伽马校正曲线1
#define Data_GAMMA_2 0x02  // 伽马校正曲线2
/*##########################################*/

void ST7735_Init(void);
void ST7735_Start_SPI(void);
void ST7735_Close_SPI(void);
void ST7735_Reset(void);
void ST7735_Unsleep(void);
void ST7735_Fill_Color(uint8_t X_Start, uint8_t Y_Start, uint8_t X_End, uint8_t Y_End, uint16_t RGB_Color);
void ST7735_Fill_Color_LVGL(uint8_t X_Start, uint8_t Y_Start, uint8_t X_End, uint8_t Y_End, uint8_t *RGB_Color);
void ST7735_Clear_View(uint16_t RGB_Color);

uint32_t OLED_Pow(uint32_t X, uint32_t Y);
void ST7735_ShowChar(uint8_t X, uint8_t Y, char Char, uint16_t Font_Color, uint16_t Back_Color);
void ST7735_ShowFloatNum(int16_t X, int16_t Y, double Number, uint8_t IntLength, uint8_t FraLength, uint16_t Font_Color, uint16_t Back_Color);
void ST7735_ShowBinNum(int16_t X, int16_t Y, uint32_t Number, uint8_t Length, uint16_t Font_Color, uint16_t Back_Color);
void ST7735_ShowHexNum(int16_t X, int16_t Y, uint32_t Number, uint8_t Length, uint16_t Font_Color, uint16_t Back_Color);
void ST7735_ShowSignedNum(int16_t X, int16_t Y, int32_t Number, uint8_t Length, uint16_t Font_Color, uint16_t Back_Color);
void ST7735_ShowNum(int16_t X, int16_t Y, uint32_t Number, uint8_t Length, uint16_t Font_Color, uint16_t Back_Color);
void ST7735_ShowString(int16_t X, int16_t Y, char *String, uint16_t Font_Color, uint16_t Back_Color);
void Serial_Printf(char *format, ...);

typedef enum
{
    Mode_Fill_Color,
    Mode_Single_Data,
} ST7735_Mode;

typedef struct ST7735
{
    uint8_t Y_All_Flag;
    uint8_t Y_Now_Flag;
    uint16_t X_Len;
    ST7735_Mode ST7735_Mode;
#if (ST7735_Use_Tim == 1)
    uint8_t ST7735_State;
#endif
    __attribute__((aligned(4))) uint8_t X_Buffer[320];
    __attribute__((aligned(4))) uint8_t Chinese_Buffer[512];
} ST7735;

extern ST7735 st7735;

#endif
