#include "gpio.h"
#include "Scheduler.h"
#include "SerialDebug.h"
#include "Samping.h"
#include "tim.h"
#include "MT6701.h"
#include "Motor.h"
schedulerTime_s schedulerTime={
    .overFlowCount=0,
    .overFlowValue=65535,
};



schedulerTask_s taskArray[TASK_COUNT]={
    [Sampling_Task]={.taskName="Sampling_Task",.taskFunction=startSampling,.desiredPeriod=(_1Mhz/Sampling_FREQ),.isEnable=taskDisable},
    [MT6701_Task]={.taskName="MT6701_Task",.taskFunction=MT6701_ReceiveData,.desiredPeriod=(_1Mhz/MT6701_FREQ),.isEnable=taskEnable},
    [Serial_Task]={.taskName="Serial_Task",.taskFunction=Serial_Send_Data,.desiredPeriod=(_1Mhz/Serial_FREQ),.isEnable=taskEnable},
    [rpmMeasure_Task]={.taskName="rpmMeasure_Task",.taskFunction=motorRPMMeasure,.desiredPeriod=(_1Mhz/rpmMeasure_FREQ),.isEnable=taskEnable},
};

uint64_t getNowTime(void)
{
    return (schedulerTime.overFlowCount*schedulerTime.overFlowValue+*Schedual_TIM.Instance->CNT);
}

taskState_b checkTask(uint8_t taskID)
{
    return taskArray[taskID].isEnable;
}

void schedulerInit(void)
{
    // __HAL_TIM_SET_PRESCALER(Schedual_TIM,72-1); // 1MHz计数频率
    // __HAL_TIM_SET_COUNTER(Schedual_TIM,0);
    HAL_TIM_Base_Start_IT(Schedual_TIM);

}

FAST_CODE void schedulerRun(void) {
    static uint64_t nowTime=0;
    for (uint8_t i = 0; i < TASK_COUNT; i++) {
        nowTime=getNowTime();
        if (nowTime >= taskArray[i].nextRunTime && checkTask(i)) {
            taskArray[i].taskFunction();  // 执行任务
            taskArray[i].nextRunTime = nowTime + taskArray[i].desiredPeriod; // 更新下次执行时间
        }
    }
}

