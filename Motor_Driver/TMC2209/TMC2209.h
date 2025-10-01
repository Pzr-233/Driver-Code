#ifndef __TMC2209__
#define __TMC2209__

#define TMC2209_uart huart1
#define ENN GPIO_PIN_11
#define SPREAD GPIO_PIN_12

//通讯开头
#define sync_and_reserved 0x05

//设备地址
#define Slave_Address0 0x00
#define Slave_Address1 0x01
#define Slave_Address2 0x02
#define Slave_Address3 0x03

//寄存器地址

#define GCONF 0x00  //全局配置
#define IHOLD_IRUN 0x10 //电流控制
#define TPOWERDOWN 0x11
#define TPWMTHRS 0x13 //静音高速切换阈值
#define TCOOLTHRS 0x14 //CoolStep启用阈值
#define VACTUAL 0x22 //内部速度设置
#define SGTHRS 0x40 //堵转检测阈值，值越小灵敏度越高
#define COOLCONF 0x42 //智能电流控制
#define CHOPCONF 0x6C //细分配置
#define DRV_STATUS 0x6F //实时诊断
#define PWMCONF 0x70 //StealthChop配置
#define MSCNT 0x6A //微步计数器(只读)

//IHOLD_IRUN寄存器默认值
#define IHOLD_IRUN_Default_Value 0x00000000

void TMC2209_Write_Reg(uint8_t Slave_Address,uint8_t Register_Address,uint32_t Data);
void TMC2209_Read_Reg(uint8_t Slave_Address,uint8_t Register_Address);
void TMC2209_Init(void);
void set_motor_speed(int32_t speed) ;
void Enable_StealthChop(uint8_t Slave_Address);
void Enable_SpreadCycle(uint8_t Slave_Address);
void Enable_TMC2209(uint8_t Slave_Address);
void Disable_TMC2209(uint8_t Slave_Address);
void TMC2209_Write_IHOLD_IRN(uint8_t Slave_Address,uint8_t IHOLD,uint8_t IRUN);

typedef enum {
    MICRO_4 = 4,
    MICRO_8 = 8,
    MICRO_16 = 16,
    MICRO_32 = 32,
    MICRO_64 = 64,
    MICRO_128 = 128,
    MICRO_256 = 256
} MicrostepMode;

extern MicrostepMode step_factor;

#endif