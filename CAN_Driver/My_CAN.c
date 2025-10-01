#include "can.h"
#include "gpio.h"
#include "stdio.h"
#include "usart.h"
#include <string.h>
uint8_t CAN_TX_Buffer[8];
uint8_t CAN_RX_Buffer[8];
void My_CAN_Init(void)
{
 CAN_FilterTypeDef CAN_FilterInitStructure;
CAN_FilterInitStructure.FilterBank=0;
CAN_FilterInitStructure.FilterIdHigh=0x0000;
CAN_FilterInitStructure.FilterIdLow=0x0000;
CAN_FilterInitStructure.FilterMaskIdHigh=0x0000;
CAN_FilterInitStructure.FilterMaskIdLow=0x0000;
CAN_FilterInitStructure.FilterScale=CAN_FILTERSCALE_32BIT;
CAN_FilterInitStructure.FilterMode=CAN_FILTERMODE_IDMASK;
CAN_FilterInitStructure.FilterFIFOAssignment=CAN_FILTER_FIFO0;
CAN_FilterInitStructure.FilterActivation=ENABLE;
HAL_CAN_ConfigFilter(&hcan, &CAN_FilterInitStructure);
// 5. 启动CAN
HAL_CAN_Start(&hcan);

}

void MyCan_Transmit(uint32_t ID,uint8_t Lenght,uint8_t *Data)
{
  CAN_TxHeaderTypeDef TxMessage;
  TxMessage.StdId=ID;
  TxMessage.ExtId=ID;
  TxMessage.IDE=CAN_ID_STD;
  TxMessage.RTR=CAN_RTR_DATA;
  TxMessage.DLC=Lenght;
  uint32_t txMailbox;
  HAL_CAN_AddTxMessage(&hcan, &TxMessage, Data, &txMailbox);

}

void MyCan_Receive(uint32_t *ID,uint8_t *Length,uint8_t *Data)
{
  CAN_RxHeaderTypeDef RxMessage;
  HAL_CAN_GetRxMessage(&hcan,CAN_FilterFIFO0,&RxMessage,Data);
  if (RxMessage.IDE == CAN_ID_STD)
	{
		*ID = RxMessage.StdId;
	}
	else
	{
		*ID = RxMessage.ExtId;
	}
  if (RxMessage.RTR == CAN_RTR_DATA)
	{
		*Length = RxMessage.DLC;
	}
}

uint8_t MyCAN_ReceiveFlag(void)
{
	if (HAL_CAN_GetRxFifoFillLevel(&hcan, CAN_FilterFIFO0) > 0)
	{
		return 1;
	}
	return 0;
}
