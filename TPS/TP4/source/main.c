#include "hardware.h"
#include  <os.h>
#include <stddef.h>		//NULL definition

/* Task Start */
#define TASKSTART_STK_SIZE 		512u
#define TASKSTART_PRIO 			2u
static OS_TCB TaskStartTCB;
static CPU_STK TaskStartStk[TASKSTART_STK_SIZE];

/* Task 2 */
#define TASK2_STK_SIZE			256u
#define TASK2_STK_SIZE_LIMIT	(TASK2_STK_SIZE / 10u)
#define TASK2_PRIO              3u
static OS_TCB Task2TCB;
static CPU_STK Task2Stk[TASK2_STK_SIZE];

/* Task 3 */
#define TASK3_STK_SIZE			256u
#define TASK3_STK_SIZE_LIMIT	(TASK3_STK_SIZE / 10u)
#define TASK3_PRIO              3u
static OS_TCB Task3TCB;
static CPU_STK Task3Stk[TASK3_STK_SIZE];

static OS_Q floorMsgQueue;

void App_Init (OS_Q*);
void App_Run (void);
void Transmission_Init (OS_Q*);
void Transmission_Run (void);
void KeepAlive_Run (void);

static void TaskTransmission(void *p_arg) {
    (void)p_arg;

    while (1) {
    	Transmission_Run();
    }
}

static void TaskKeepAlive(void *p_arg) {
    (void)p_arg;

    while (1) {
    	KeepAlive_Run();
    }
}


static void TaskStart(void *p_arg) {
    (void)p_arg;
    OS_ERR os_err;

    /* Initialize the uC/CPU Services. */
    CPU_Init();

#if OS_CFG_STAT_TASK_EN > 0u
    /* (optional) Compute CPU capacity with no task running */
    OSStatTaskCPUUsageInit(&os_err);
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();
#endif

    /* Create message queue */
    OSQCreate(&floorMsgQueue, "Floor Msg Queue", 32, &os_err);

    /* Create Task2 */
	OSTaskCreate(&Task2TCB, 			//tcb
				 "Task Transmission",	//name
				 TaskTransmission,		//func
				  0u,					//arg
				  TASK2_PRIO,			//prio
				 &Task2Stk[0u],			//stack
				  TASK2_STK_SIZE_LIMIT,	//stack limit
				  TASK2_STK_SIZE,		//stack size
				  0u,
				  0u,
				  0u,
				 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
				 &os_err);

	/* Create Task3 */
	OSTaskCreate(&Task3TCB, 			//tcb
				 "Task KeepAlive",		//name
				 TaskKeepAlive,			//func
				  0u,					//arg
				  TASK3_PRIO,			//prio
				 &Task3Stk[0u],			//stack
				  TASK3_STK_SIZE_LIMIT,	//stack limit
				  TASK3_STK_SIZE,		//stack size
				  0u,
				  0u,
				  0u,
				 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
				 &os_err);

    while (1) {
    	App_Run();
    }
}


int main(void) {
    OS_ERR err;

#if (CPU_CFG_NAME_EN == DEF_ENABLED)
    CPU_ERR  cpu_err;
#endif

    hw_Init();

    OSInit(&err);

    hw_DisableInterrupts();
    App_Init(&floorMsgQueue);
    Transmission_Init(&floorMsgQueue);
    hw_EnableInterrupts();

 #if OS_CFG_SCHED_ROUND_ROBIN_EN > 0u
	 /* Enable task round robin. */
	 OSSchedRoundRobinCfg((CPU_BOOLEAN)1, 0, &err);
 #endif
    OS_CPU_SysTickInit(SystemCoreClock / (uint32_t)OSCfg_TickRate_Hz);

    OSTaskCreate(&TaskStartTCB,
                 "Main App Task",
                  TaskStart,
                  0u,
                  TASKSTART_PRIO,
                 &TaskStartStk[0u],
                 (TASKSTART_STK_SIZE / 10u),
                  TASKSTART_STK_SIZE,
                  0u,
                  0u,
                  0u,
                 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP),
                 &err);

    OSStart(&err);

	/* Should Never Get Here */
    while (1) {

    }
}
