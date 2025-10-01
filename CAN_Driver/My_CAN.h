#ifndef __MY_CAN_ 
#define __MY_CAN_
void My_CAN_Init(void);
void MyCan_Transmit(uint32_t ID,uint8_t Lenght,uint8_t *Data);
void MyCan_Receive(uint32_t *ID,uint8_t *Length,uint8_t *Data);
uint8_t MyCAN_ReceiveFlag(void);



#endif  
