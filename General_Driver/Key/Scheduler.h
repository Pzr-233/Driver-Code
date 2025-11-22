#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__
#include "tim.h"

#define _1Mhz 1000000
#define Schedual_TIM &htim3

typedef enum {
    Sampling_Task,
    MT6701_Task,
    Serial_Task,
    rpmMeasure_Task,
    TASK_COUNT
} taskID_e;

typedef enum 
{
    Sampling_FREQ=100,
    MT6701_FREQ=1000,    
    Serial_FREQ=10,
    rpmMeasure_FREQ=1,      
}taskFreq_e;
    
typedef enum {
    taskDisable=0,
    taskEnable=1,
} taskState_b;

typedef struct {
    uint32_t overFlowCount;
    uint32_t overFlowValue;
}schedulerTime_s;

// 定义任务结构
typedef struct {
    void (*taskFunction)(void); // 任务函数
    uint32_t desiredPeriod;     // 执行间隔
    uint64_t nextRunTime;       // 下次运行时间
    char *taskName;             // 任务ID
    uint8_t isEnable;
} schedulerTask_s;
extern schedulerTime_s schedulerTime;
void schedulerRun(void);
void schedulerInit(void);
uint64_t getNowTime(void);

#endif // !