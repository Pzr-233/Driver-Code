#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__
#include <stdint.h>
#include "Queue.h"
#define _1Mhz             1000000
#define Schedual_TIM      &htim2
#define Schedual_TIM_FREQ (90 * _1Mhz) // 定时器时钟频率

typedef enum {
    Servo_Control_Task,
    Serial_Task,
    Key_Scan_Task,
    LED_Task,
    OLED_Task,
    Scheduler_Manager_Task,
    TASK_COUNT
} taskID_e; // 任务ID枚举

typedef enum {
    taskDisable = 0,
    taskEnable  = 1,
} taskState_b; // 任务使能状态

typedef struct {
    uint32_t overFlowCount;
    uint32_t overFlowValue;
    uint64_t nowTime;
} schedulerTime_t; // 调度器时间结构

typedef struct
{
    float load;
    float onTimeRate;
    uint32_t minTaskTime;
    uint64_t allTasksRunTimes;
} schedulerManger_t; // 调度器管理结构

// 定义任务结构
typedef struct {
    void (*taskFunction)(void); // 任务函数
    uint32_t desiredPeriod;     // 期望执行周期（微秒）
    uint32_t realPeriod;        // 实际执行周期（微秒）
    uint32_t count;             // 任务执行计数
    uint32_t taskSpendTime;     // 任务执行一次花费时间
    uint64_t nextRunTime;       // 下次运行时间
    uint8_t isEnable;           // 任务使能标志
    char *taskName;             // 任务ID
} schedulerTask_t;

extern schedulerTime_t schedulerTime;
void schedulerRun(void);
uint64_t getNowTime(void);
void schedulerTIMOverFlowHandle(void);
void schedulerPushTask(void (*taskFunction)(void));
void init(void);

#endif // !