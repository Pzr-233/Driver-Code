#include "gpio.h"
#include "Scheduler.h"
// #include "SerialDebug.h"
#include "Get_Key.h"
#include "tim.h"
#include "ST7735.h"
schedulerTime_s schedulerTime={
    .overFlowCount=0,
    .overFlowValue=65535,
};


schedulerTask_s taskArray[TASK_COUNT]={
    // [Key_Scan_Task]={.taskName="Key_Scan_Task",.taskFunction=Key_Scan,.desiredPeriod=(_1Mhz/Key_Scan_FREQ),.isEnable=taskEnable},
    // [Test_Task]={.taskName="Test_Task",.taskFunction=testTask,.desiredPeriod=(_1Mhz/Test_Task_FREQ),.isEnable=taskEnable},
    // [Sampling_Task]={.taskName="Sampling_Task",.taskFunction=startSampling,.desiredPeriod=(_1Mhz/Sampling_FREQ),.isEnable=taskDisable},
    // [MT6701_Task]={.taskName="MT6701_Task",.taskFunction=MT6701_ReceiveData,.desiredPeriod=(_1Mhz/MT6701_FREQ),.isEnable=taskEnable},
    // [Serial_Task]={.taskName="Serial_Task",.taskFunction=Serial_Send_Data,.desiredPeriod=(_1Mhz/Serial_FREQ),.isEnable=taskEnable},
    // [rpmMeasure_Task]={.taskName="rpmMeasure_Task",.taskFunction=motorRPMMeasure,.desiredPeriod=(_1Mhz/rpmMeasure_FREQ),.isEnable=taskEnable},
};

uint64_t getNowTime(void)
{
    return (schedulerTime.overFlowCount*schedulerTime.overFlowValue+*Schedual_TIM.Instance->CNT);
}

taskState_b checkTask(uint8_t taskID)
{
    return taskArray[taskID].isEnable;
}

void schedulerTimeOverFlowHandle(void)
{
    schedulerTime.overFlowCount++;
}

void schedulerInit(void)
{
    __HAL_TIM_SET_PRESCALER(Schedual_TIM,(Schedual_TIM_FREQ/_1Mhz)-1); // 1MHz计数频率
    __HAL_TIM_SET_COUNTER(Schedual_TIM,0);
    HAL_TIM_Base_Start_IT(Schedual_TIM);
    taskArray[Key_Scan_Task].desiredPeriod=(_1Mhz/getKeyScanFreq());
}

void schedulerRun(void) {
    static uint64_t nowTime=0;
    for (uint8_t i = 0; i < TASK_COUNT; i++) {
        nowTime=getNowTime();
        if (nowTime >= taskArray[i].nextRunTime && checkTask(i)) {
            taskArray[i].taskFunction();  // 执行任务
            taskArray[i].nextRunTime = nowTime + taskArray[i].desiredPeriod; // 更新下次执行时间
        }
    }
}

