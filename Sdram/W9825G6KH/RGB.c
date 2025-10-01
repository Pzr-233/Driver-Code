#include "gpio.h"
#include "ltdc.h"
#include "stdio.h"
#include "string.h"
#include "RGB.h"
#include "fmc.h"
#include "stdio.h"
#include "dma2d.h"
#include "lv_port_disp_template.h"
uint16_t RGB_buffer[480][800] __attribute__((section(".SDRAM")));
uint16_t Color_buffer[480][800] __attribute__((section(".SDRAM")));

uint8_t Flag=0;
FMC_SDRAM_CommandTypeDef command;

void SDRAM_Delay(__IO uint32_t Count)
{
  __IO uint32_t index=0;
  for(index=(200000*Count);index!=0;index--);
}
void SDRAM_Initialization_Sequence(SDRAM_HandleTypeDef *hsdram, FMC_SDRAM_CommandTypeDef *Command)
{
  __IO uint32_t tmpmrd = 0;
  
  /* Configure a clock configuration enable command */
  Command->CommandMode 					= FMC_SDRAM_CMD_CLK_ENABLE;	// 开启SDRAM时钟 
  Command->CommandTarget 				= FMC_COMMAND_TARGET_BANK; 	// 选择要控制的区域
  Command->AutoRefreshNumber 			= 1;
  Command->ModeRegisterDefinition 	= 0;
  
  HAL_SDRAM_SendCommand(hsdram, Command, 100);	// 发送控制指令
  SDRAM_Delay(5);		// 延时等待
  
  /* Configure a PALL (precharge all) command */ 
  Command->CommandMode 					= FMC_SDRAM_CMD_PALL;		// 预充电命令
  Command->CommandTarget 				= FMC_COMMAND_TARGET_BANK;	// 选择要控制的区域
  Command->AutoRefreshNumber 			= 1;
  Command->ModeRegisterDefinition 	= 0;
  
  HAL_SDRAM_SendCommand(hsdram, Command, 100);  // 发送控制指令
  
  /* Configure a Auto-Refresh command */ 
  Command->CommandMode 					= FMC_SDRAM_CMD_AUTOREFRESH_MODE;	// 使用自动刷新
  Command->CommandTarget 				= FMC_COMMAND_TARGET_BANK;          // 选择要控制的区域
  Command->AutoRefreshNumber			= 8;                                // 自动刷新次数
  Command->ModeRegisterDefinition 	= 0;
  
  HAL_SDRAM_SendCommand(hsdram, Command, 100);	// 发送控制指令
  
  /* Program the external memory mode register */
  tmpmrd = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_2          |
                     SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   |
                     SDRAM_MODEREG_CAS_LATENCY_3           |
                     SDRAM_MODEREG_OPERATING_MODE_STANDARD |
                     SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;
  
  Command->CommandMode					= FMC_SDRAM_CMD_LOAD_MODE;	// 加载模式寄存器命令
  Command->CommandTarget 				= FMC_COMMAND_TARGET_BANK;	// 选择要控制的区域
  Command->AutoRefreshNumber 			= 1;
  Command->ModeRegisterDefinition 	= tmpmrd;
  HAL_SDRAM_SendCommand(hsdram, Command, 0x1000);	// 发送控制指令
  HAL_SDRAM_ProgramRefreshRate(hsdram, 761); 	// 设置刷新计数器 
}

void SDRAM_Device_Init(void)
{
	SDRAM_Initialization_Sequence(&hsdram1, &command);//因为用的是SDRAM1，然后前面也定义了一个FMC_SDRAM_CommandTypeDef类型的command
  memset(RGB_buffer,0xFF,sizeof(RGB_buffer));
  HAL_LTDC_SetAddress(&hltdc,(uint32_t)RGB_buffer,0);
  HAL_LTDC_SetWindowPosition(&hltdc,0,0,0);
}

void RGB_Fill_Color(uint16_t X_Start,uint16_t Y_Start,uint16_t X_End,uint16_t Y_End,uint16_t RGB_Color)
{

  for(uint16_t i=0;i<X_End-X_Start;i++)
  {
    for (uint16_t j=0;j<Y_End-Y_Start;j++)
    {
      RGB_buffer[Y_Start+j][X_Start+i]=RGB_Color; 
    }
  }
}

void RGB_Fill_Color_LVGL(uint16_t X_Start,uint16_t Y_Start,uint16_t X_End,uint16_t Y_End,uint16_t *RGB_Color)
{
DMA2D->CR&=~(DMA2D_CR_START); //停止DMA2D
DMA2D->CR=DMA2D_M2M; //DMA2D配置为内存到内存
DMA2D->FGMAR=(uint32_t)RGB_Color;
DMA2D->OMAR=(uint32_t)(&RGB_buffer[Y_Start][X_Start]);
DMA2D->NLR = ((X_End-X_Start+1)<< DMA2D_NLR_PL_Pos)|((Y_End-Y_Start+1)<< DMA2D_NLR_NL_Pos);
DMA2D->OOR=0;//行偏移
DMA2D->CR|=DMA2D_CR_START;
// 等待DMA2D传输完成（轮询方式）
while ((DMA2D->ISR & DMA2D_ISR_TCIF) == 0) { // 等待TCIF标志置位
    // 可以添加超时机制防止死循环
}
DMA2D->IFCR |= DMA2D_IFCR_CTCIF; // 清除传输完成标志位
}


