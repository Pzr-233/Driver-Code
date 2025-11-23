#ifndef __GET_KEY_H
#define __GET_KEY_H
#include "gpio.h"
#define PRESSED  GPIO_PIN_RESET // 按下时GPIO状态
#define RELEASED GPIO_PIN_SET   // 释放时GPIO状态

typedef enum {
    Key_Pressed,
    Key_Released,
} Key_State_e;

typedef enum {
    Key_None = 0,
    Key_Click,
    Key_Double_Click,
    Key_Long_Pres,
} Key_Action_e; // 按键动作枚举

typedef enum {
    Key_1,
    Key_2,
    Key_3,
    Key_Number,
} Key_Number_e; // 按键数量

typedef struct
{
    GPIO_TypeDef *GPIOx;
    uint16_t GPIO_Pin;
    Key_Number_e Number;
    Key_State_e State;
    Key_Action_e Action;
    uint32_t Time_Counter;
    uint32_t duobleClick_Counter;
    uint8_t doubleClickFlag;
    uint8_t longPressFlag;
} Key_s; // 按键结构体

typedef struct
{
    uint32_t Key_Scan_Freq;      // 按键扫描频率，单位Hz
    uint32_t keyLongPressTime;   // 长按时间，单位为扫描次数
    uint32_t keyDuobleClickTime; // 双击时间间隔，单位为扫描次数
    Key_s Key[Key_Number];
} KeyManager_s; // 按键管理结构体

void Key_Init(void);
void Key_Scan(void);
Key_Action_e Read_KeyState(Key_Number_e keyNum);
uint32_t getKeyScanFreq(void);
void setKeyScanFreq(uint32_t freq); // 设置按键扫描频率，单位Hz

#endif
