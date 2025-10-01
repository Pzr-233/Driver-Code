#ifndef __RGB_H
#define __RGB_H

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_sdram.h" // Ensure FMC_SDRAM_CommandTypeDef is defined

#define SDRAM_Size 0x02000000  //32M字节
#define SDRAM_Half_Size 0x01000000
#define SDRAM_BANK_ADDR     ((uint32_t)0xC0000000) 				// FMC SDRAM 数据基地址(已测试D开头的SDRAM2地址无法写入，所以没问题)
#define FMC_COMMAND_TARGET_BANK   FMC_SDRAM_CMD_TARGET_BANK1	//	SDRAM 的bank选择
#define SDRAM_TIMEOUT     ((uint32_t)0x1000) 						// 超时判断时间

#define SRAM_Size 			0x00020000  //128K字节
#define SRAM_Half_Size 0x00010000
#define SRAM_ADDR     ((uint32_t)0x30000000)

#define SDRAM_MODEREG_BURST_LENGTH_1             ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((uint16_t)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((uint16_t)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2              ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000) 
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((uint16_t)0x0200) 
extern FMC_SDRAM_CommandTypeDef command;

void SDRAM_Initialization_Sequence(SDRAM_HandleTypeDef *hsdram, FMC_SDRAM_CommandTypeDef *Command);
void SDRAM_Device_Init(void);
void SDRAM_Delay(__IO uint32_t Count);
void RGB_Fill_Color(uint16_t X_Start,uint16_t Y_Start,uint16_t X_End,uint16_t Y_End,uint16_t RGB_Color);
void RGB_Fill_Color_LVGL(uint16_t X_Start,uint16_t Y_Start,uint16_t X_End,uint16_t Y_End,uint16_t *RGB_Color);
extern uint16_t Color_buffer[480][800];
#endif // !