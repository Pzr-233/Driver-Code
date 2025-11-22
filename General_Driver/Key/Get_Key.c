#include "gpio.h"
#include "tim.h"
#include "Get_Key.h"
#include "IT_Freq.h"
uint8_t Key_State;
uint8_t Key_Count      = 0;
uint8_t Key_slect      = 0;
uint16_t test_falg     = 0;
uint32_t Key_Scan_Freq = 0;

KeyManager_s KeyManager = {
    .Key = {
        {GPIOC, GPIO_PIN_1, Key_1, Key_Released, Key_None, 0},
        {GPIOC, GPIO_PIN_2, Key_2, Key_Released, Key_None, 0},
        {GPIOC, GPIO_PIN_3, Key_3, Key_Released, Key_None, 0},
    },
};

void setKeyScanFreq(uint32_t freq) // 设置按键扫描频率，单位Hz
{
    KeyManager.Key_Scan_Freq = freq;
}

uint32_t getKeyScanFreq(void)
{
    return KeyManager.Key_Scan_Freq;
}

void Key_Init(void)
{
    setKeyScanFreq(50);
    KeyManager.keyDuobleClickTime = getKeyScanFreq() / 10;
    KeyManager.keyLongPressTime   = getKeyScanFreq() / 2;
}

void Key_Scan(void)
{
    uint8_t pinState = 0;
    for (uint8_t i = 0; i < Key_Number; i++) {                                            // 遍历所有按键
        pinState = HAL_GPIO_ReadPin(KeyManager.Key[i].GPIOx, KeyManager.Key[i].GPIO_Pin); // 读取按键状态
        if (pinState == GPIO_PIN_RESET)                                                   // 按键按下
        {
            if (KeyManager.Key[i].Time_Counter <= KeyManager.keyLongPressTime) // 未达到长按时间
            {
                KeyManager.Key[i].State = Key_Pressed;
                KeyManager.Key[i].Time_Counter++;
                if (KeyManager.Key[i].duobleClick_Counter != 0) { // 已有一次按下，准备进行双击检测
                    KeyManager.Key[i].doubleClickFlag     = 1;    // 标记为双击
                    KeyManager.Key[i].duobleClick_Counter = 0;    // 清除双击计数器
                }
            } else // 达到长按时间
            {
                KeyManager.Key[i].Action        = Key_Long_Pres; // 长按动作
                KeyManager.Key[i].longPressFlag = 1;             // 标记为长按
            }
        } else if (pinState == GPIO_PIN_SET && KeyManager.Key[i].State == Key_Pressed && KeyManager.Key[i].doubleClickFlag != 1 && KeyManager.Key[i].longPressFlag != 1) // 按键释放且未标记为双击或长按
        {
            KeyManager.Key[i].duobleClick_Counter++;
            KeyManager.Key[i].Time_Counter = 0;
            if (KeyManager.Key[i].duobleClick_Counter > KeyManager.keyDuobleClickTime) {
                KeyManager.Key[i].Action              = Key_Click;    // 单击动作
                KeyManager.Key[i].State               = Key_Released; // 更新状态为释放
                KeyManager.Key[i].duobleClick_Counter = 0;            // 清除双击计数器
            }
        } else if (pinState == GPIO_PIN_SET) {
            if (KeyManager.Key[i].doubleClickFlag == 1) {
                KeyManager.Key[i].Action          = Key_Double_Click; // 双击动作
                KeyManager.Key[i].doubleClickFlag = 0;                // 清除双击标志
            }
            if (KeyManager.Key[i].longPressFlag == 1) { // 清除长按标志与动作
                KeyManager.Key[i].longPressFlag = 0;
                KeyManager.Key[i].Action        = Key_None; // 长按动作已处理，清除动作
            }
            KeyManager.Key[i].State        = Key_Released; // 更新状态为释放
            KeyManager.Key[i].Time_Counter = 0;            // 清除时间计数器
        }
    }
}

Key_Action_e Read_KeyState(Key_Number_e keyNum)
{
    Key_Action_e action = 0;
    switch (KeyManager.Key[keyNum].Action) {
        case Key_Long_Pres:
            action = Key_Long_Pres;
            break;
        case Key_Double_Click:
            action                        = Key_Double_Click;
            KeyManager.Key[keyNum].Action = Key_None; // 读取后清除动作
            break;
        case Key_Click:
            action                        = Key_Click;
            KeyManager.Key[keyNum].Action = Key_None; // 读取后清除动作
            break;
        default:
            action = Key_None;
    }
    return action;
}

//     if(Key_State==Key1_Pres)
//     {
//         Key_State=0;
//         return Key1_Pres;
//     }
//     else if (Key_State==Key2_Pres)
//     {
//         Key_State=0;
//         return Key2_Pres;
//     }
//      else if (Key_State==Key3_Pres)
//     {
//         Key_State=0;
//         return Key3_Pres;
//     }
//      else if (Key_State==Key1_Long_Pres)
//     {
//         Key_State=0;
//         return Key1_Long_Pres;
//     }
//      else if (Key_State==Key2_Long_Pres)
//     {
//         Key_State=0;
//         return Key2_Long_Pres;
//     }
//     else if (Key_State==Key3_Long_Pres)
//     {
//         Key_State=0;
//         return Key3_Long_Pres;
//     }
//     else
//     {
//         return 0;
//     }

//     if(Key_slect==0)
//     {
//         if(HAL_GPIO_ReadPin(Key_GPIO_Port,Key_1)==GPIO_PIN_RESET)
//         {
//             Key_slect=1;
//         }
//         else if(HAL_GPIO_ReadPin(Key_GPIO_Port,Key_2)==GPIO_PIN_RESET)
//         {
//             Key_slect=2;
//         }
//         else if(HAL_GPIO_ReadPin(Key_GPIO_Port,Key_3)==GPIO_PIN_RESET)
//         {
//             Key_slect=3;
//         }
//     }
//     else if (Key_slect==1)
//     {
//            if(HAL_GPIO_ReadPin(Key_GPIO_Port,Key_1)==GPIO_PIN_RESET)
//         {
//             Key_Count++;
//         }
//         else if (HAL_GPIO_ReadPin(Key_GPIO_Port,Key_1)==GPIO_PIN_SET)
//         {
//             if(Key_Count<5)
//             {
//                 Key_State=Key1_Pres;
//                 Key_Count=0;
//                 Key_slect=0;
//             }
//             else if(Key_Count>=5)
//             {
//                 Key_State=Key1_Long_Pres;
//                 Key_Count=0;
//                 Key_slect=0;
//             }
//         }
//     }
//      else if (Key_slect==2)
//     {
//       if(HAL_GPIO_ReadPin(Key_GPIO_Port,Key_2)==GPIO_PIN_RESET)
//         {
//             Key_Count++;
//         }
//         else if (HAL_GPIO_ReadPin(Key_GPIO_Port,Key_2)==GPIO_PIN_SET)
//         {
//             if(Key_Count<5)
//             {
//                 Key_State=Key2_Pres;
//                 Key_Count=0;
//                 Key_slect=0;
//             }
//             else if(Key_Count>=5)
//             {
//                 Key_State=Key2_Long_Pres;
//                 Key_Count=0;
//                 Key_slect=0;
//             }
//         }
//     }
//     else if (Key_slect==3)
//     {
//        if(HAL_GPIO_ReadPin(Key_GPIO_Port,Key_3)==GPIO_PIN_RESET)
//         {
//             Key_Count++;
//         }
//         else if (HAL_GPIO_ReadPin(Key_GPIO_Port,Key_3)==GPIO_PIN_SET)
//         {
//             if(Key_Count<5)
//             {
//                 Key_State=Key3_Pres;
//                 Key_Count=0;
//                 Key_slect=0;
//             }
//              else if(Key_Count>=5)
//             {
//                 Key_State=Key3_Long_Pres;
//                 Key_Count=0;
//                 Key_slect=0;
//             }
//         }
//      }
