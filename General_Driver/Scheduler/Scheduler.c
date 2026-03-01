#include "Scheduler.h"
#include "gpio.h"
// #include "SerialDebug.h"
#include "Get_Key.h"
#include "ST7735.h"
#include "Servo.h"
#include "Speaker.h"
#include "algorithm.h"
#include "cpptest.h"
#include "ctest.h"
#include "mpu6050.h"
#include "myMath.h"
#include "string.h"
#include "tim.h"
// #include "uart_to_esp32.h"
schedulerTask_t taskArray[TASK_COUNT];
Queue_t taskQueue; // 任务队列

schedulerManger_t schedulerManger = {
    .load = 0.0f,
    .allTasksRunTimes = 0,
};
schedulerTime_t schedulerTime = {
    .overFlowCount = 0,
    .overFlowValue = 4294967295,
};
uint8_t num = 0;
void Serial(void)
{
}
void test(void)
{
    num += 1;
}

void LED(void)
{
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
}
void Servo_Control(void)
{
    static int8_t velocity = 0;
    if (Read_KeyState(Key_1) == Key_Click)
    {
        velocity += 10;
        velocity = rangeInt(velocity, -100, 100);
        schedulerPushTask(test);
        Servo_SetVelocity(SERVO_1, velocity);
    }
    if (Read_KeyState(Key_2) == Key_Click)
    {
        velocity -= 10;
        velocity = rangeInt(velocity, -100, 100);
        Servo_SetVelocity(SERVO_1, velocity);
    }
}
void OLED(void)
{
    ST7735_ShowSignedNum(0, 0, num, 3, 0xFFFF, 0x0000);
    ST7735_ShowNum(0, 16, taskQueue.count, 2, 0xFFFF, 0x0000);
    ST7735_ShowNum(0, 32, taskQueue.head, 2, 0xFFFF, 0x0000);
    ST7735_ShowNum(0, 48, taskQueue.tail, 2, 0xFFFF, 0x0000);
}

inline uint64_t getNowTime(void)
{

    // uint64_t time = (uint64_t)(schedulerTime.overFlowCount * schedulerTime.overFlowValue + __HAL_TIM_GET_COUNTER(Schedual_TIM));
    return schedulerTime.nowTime + __HAL_TIM_GET_COUNTER(Schedual_TIM);
}

inline static taskState_b checkTaskEnable(uint8_t taskID)
{
    return taskArray[taskID].isEnable;
}

void schedulerTIMOverFlowHandle(void)
{
    // schedulerTime.overFlowCount++;
    schedulerTime.nowTime += schedulerTime.overFlowValue;
}

uint32_t schedulerMinTaskTime(void)
{
    uint32_t minTime = 0xFFFFFFFF;
    for (uint8_t i = 0; i < TASK_COUNT; i++)
    {
        if (taskArray[i].isEnable && taskArray[i].desiredPeriod < minTime)
        {
            minTime = taskArray[i].desiredPeriod;
        }
    }
    return minTime;
}

void calculateSchedulerLoad(void)
{
    float load = 0.0f;
    float allTime = 0.0f;
    float taskNum = 0.0f;
    for (uint8_t i = 0; i < TASK_COUNT; i++)
    {
        if (taskArray[i].isEnable)
        {
            load += (float)taskArray[i].taskSpendTime / (float)taskArray[i].desiredPeriod * 100.0f;
            allTime += (1.0f - (float)(taskArray[i].realPeriod - taskArray[i].desiredPeriod) / (float)taskArray[i].desiredPeriod);
            taskNum++;
        }
    }
    schedulerManger.onTimeRate = allTime / taskNum * 100.0f;
    schedulerManger.load = load;
}

static void schedulerAddTask(taskID_e taskID, char *taskName, void (*taskFunction)(void), uint32_t freq, taskState_b state)
{
    taskArray[taskID].taskFunction = taskFunction;
    taskArray[taskID].desiredPeriod = _1Mhz / freq;
    taskArray[taskID].isEnable = state;
    taskArray[taskID].taskName = taskName;
}

static void schedulerTasksInit(void)
{
    schedulerAddTask(Servo_Control_Task, "Servo_Control_Task", Servo_Control, 50, taskEnable);
    schedulerAddTask(Serial_Task, "Serial_Task", Serial, 20, taskEnable);
    schedulerAddTask(Key_Scan_Task, "Key_Scan_Task", Key_Scan, 50, taskEnable);
    schedulerAddTask(LED_Task, "LED_Task", LED, 2, taskEnable);
    schedulerAddTask(OLED_Task, "OLED_Task", OLED, 10, taskEnable);
    schedulerAddTask(Scheduler_Manager_Task, "Scheduler_Manager_Task", calculateSchedulerLoad, 10, taskEnable);
}

void schedulerPushTask(void (*taskFunction)(void))
{
    taskQueue.push(&taskQueue, &taskFunction);
}
static void schedulerPopTask(void)
{
    void (*task)(void) = NULL;
    taskQueue.pop(&taskQueue, (void *)&task);
    if (task != NULL)
    {
        task();
    }
}
static void schedulerTaskQueueInit(void)
{
    Queue_Init(&taskQueue, 20, sizeof(functionPoint_t));
}

void schedulerInit(void)
{
    __HAL_TIM_SET_PRESCALER(Schedual_TIM, (Schedual_TIM_FREQ / _1Mhz) - 1); // 1MHz计数频率
    __HAL_TIM_SET_COUNTER(Schedual_TIM, 0);
    *Schedual_TIM.Instance->EGR = 1; // 生成更新事件，加载预设值
    HAL_TIM_Base_Start_IT(Schedual_TIM);
    schedulerTime.overFlowCount = 0;
    schedulerTasksInit(); // 加载任务数组
    schedulerManger.minTaskTime = schedulerMinTaskTime();
    schedulerTaskQueueInit(); // 初始化任务队列，队列中每个任务只运行一次，运行完后从队列中删除
}

void schedulerRun(void)
{
    static uint64_t nowTime = 0;
    for (uint8_t i = 0; i < TASK_COUNT; i++)
    {
        nowTime = getNowTime();
        if (nowTime >= taskArray[i].nextRunTime && checkTaskEnable(i))
        {
            taskArray[i].realPeriod = nowTime - taskArray[i].nextRunTime + taskArray[i].desiredPeriod; // 计算实际任务间隔
            taskArray[i].taskFunction();                                                               // 执行任务
            taskArray[i].taskSpendTime = getNowTime() - nowTime;                                       // 计算任务执行时间
            taskArray[i].nextRunTime = nowTime + taskArray[i].desiredPeriod;                           // 更新下次执行时间
            schedulerManger.allTasksRunTimes += taskArray[i].taskSpendTime;                            // 统计所有任务执行总时间
            taskArray[i].count++;                                                                      // 任务执行计数加1
        }
    }
    schedulerPopTask(); // 执行一次队列中的任务
}

void init(void)
{
    schedulerInit();
    ST7735_Init();
    Key_Init();
    Servo_Init(SERVO_1);
    Servo_SetVelocity(SERVO_1, 0);
}