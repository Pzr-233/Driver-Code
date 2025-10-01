#ifndef __MT6701_H
#define __MT6701_H

void MT6701_ON(void);
void MT6701_OFF(void);
void MT6701_ReceiveData(void);
float MT6701_Get_Absolute_Angle(void);
void Motor_Velocity_Measure(void);
void Enable_Velocity_Measure(void);



uint8_t MT6701_Magnetic_State(void);
void MT6701_Show_Magnetic_State(void);
void MT6701_Show_Button_State(void);


extern uint8_t Receive_Arry[3];
extern uint16_t Absolute_Angle;
extern float Angle_Change;
extern int16_t Cycle_Change;
extern uint16_t Angle_Data;
extern int32_t Data_Change;
extern float Actual_velocity;


void MT6701_Get_Change(void);

#endif
