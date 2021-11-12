/***************************************************************************//**
  @file     App.c
  @brief    Application functions
  @author   Grupo 1
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdio.h>
#include <string.h>

#include "os.h"
#include "DRV_DataTransmission.h"
#include "DRV_Board.h"
#include "DRV_Timers.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define WAIT_FOR_RESPONSE_TICKS		TIMER_MS2TICKS(500)
#define SEND_DATA_TIMEOUT_TICKS		TIMER_MS2TICKS(15000)
#define KEEPALIVE_TIMEOUT_TICKS		TIMER_MS2TICKS(1000)
#define KEEPALIVE_BLINK_TICKS		TIMER_MS2TICKS(500)

#define MAX_KA_FAILS				3

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

enum{
	IDLE,
	SEND_DATA,
	KEEPALIVE
};

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void response_callback();
static void sd_timeout_callback();
static void ka_timeout_callback();
static void ka_blink_callback();

static void KeepAlive_Init();

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static OS_Q* floorMsgQPointer;
static uint8_t current_transmission;
static uint8_t keep_alive_fails = MAX_KA_FAILS;
static bool send_data_ok = false;
static bool keepalive_ok = false;

static tim_id_t sd_timeout_tim_id;
static tim_id_t ka_timeout_tim_id;
static tim_id_t ka_blink_tim_id;

static OS_SEM semSendDataResponse;
static OS_SEM semKeepAliveResponse;

static OS_MUTEX data_transmission_mutex;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Función que se llama 1 vez, al comienzo del programa */
void Transmission_Init (OS_Q* msgq)
{
	OS_ERR os_err;

	initBoard();
	initTimers();
	initDataTransmission();
	setResponseCallback(response_callback);

	floorMsgQPointer = msgq;

	sd_timeout_tim_id = timerGetId();


	//Create Mutex
	OSMutexCreate(&data_transmission_mutex, "Data Transmission Mutex", &os_err);

	//Create semaphore
	OSSemCreate(&semSendDataResponse, "Sem Send Data Response", 0u, &os_err);

	KeepAlive_Init();
}

/* Función que se llama constantemente en un ciclo infinito */
void Transmission_Run (void)
{
	OS_ERR os_err;

	void* p_msg;
	OS_MSG_SIZE msg_size;

	p_msg = OSQPend(floorMsgQPointer, 0, OS_OPT_PEND_BLOCKING, &msg_size, NULL, &os_err);

	// Wait for transmission channel to be available
	OSMutexPend(&data_transmission_mutex, 0, OS_OPT_PEND_BLOCKING, NULL, &os_err);

	send_data_ok = false;
	while(send_data_ok == false){
		current_transmission = SEND_DATA;
		sendData(((uint16_t*)p_msg)[0], ((uint16_t*)p_msg)[1], ((uint16_t*)p_msg)[2]);
		timerStart(sd_timeout_tim_id, SEND_DATA_TIMEOUT_TICKS, TIM_MODE_SINGLESHOT, sd_timeout_callback);
		OSSemPend(&semSendDataResponse, 0, OS_OPT_PEND_BLOCKING, NULL, &os_err);
	}

	// Release Mutex
	OSMutexPost(&data_transmission_mutex, OS_OPT_POST_NONE, &os_err);

	// Creemos que no hace falta este delay, ya que en este punto se obtuvo una respuesta
	// OSTimeDlyHMSM(0, 0, 16, 0, OS_OPT_TIME_HMSM_STRICT, &os_err);
}


static void KeepAlive_Init (void){
	ka_timeout_tim_id = timerGetId();

	ka_blink_tim_id = timerGetId();
	timerStart(ka_blink_tim_id, KEEPALIVE_BLINK_TICKS, TIM_MODE_PERIODIC, ka_blink_callback);

	//Create semaphore
	OS_ERR os_err;
	OSSemCreate(&semKeepAliveResponse, "Sem Keep Alive Response", 0u, &os_err);
}

void KeepAlive_Run (void){
	OS_ERR os_err;

	OSTimeDlyHMSM(0, 0, 1, 0, OS_OPT_TIME_HMSM_STRICT, &os_err);

	// Wait for transmission channel to be available
	OSMutexPend(&data_transmission_mutex, 0, OS_OPT_PEND_BLOCKING, NULL, &os_err);

	keepalive_ok = false;
	current_transmission = KEEPALIVE;
	sendKeepAlive();
	timerStart(ka_timeout_tim_id, KEEPALIVE_TIMEOUT_TICKS, TIM_MODE_SINGLESHOT, ka_timeout_callback);
	OSSemPend(&semKeepAliveResponse, 0, OS_OPT_PEND_BLOCKING, NULL, &os_err);

	// Release Mutex
	OSMutexPost(&data_transmission_mutex, OS_OPT_POST_NONE, &os_err);

	if(keepalive_ok){
		keep_alive_fails = 0;
	}else{
		keep_alive_fails = (keep_alive_fails >= MAX_KA_FAILS)? MAX_KA_FAILS : keep_alive_fails+1;
	}
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void response_callback(){
	OS_ERR os_err;

	if(responseReady()){
		if(current_transmission == KEEPALIVE){
			if(getResponse() == KEEPALIVE_OK){
				current_transmission = IDLE;
				keepalive_ok = true;
				timerStop(ka_timeout_tim_id);
				OSSemPost(&semKeepAliveResponse, OS_OPT_POST_1, &os_err);
			}
		}else if(current_transmission == SEND_DATA){
			switch(getResponse()){
			case SEND_DATA_OK:
				current_transmission = IDLE;
				send_data_ok = true;
				timerStop(sd_timeout_tim_id);
		        OSSemPost(&semSendDataResponse, OS_OPT_POST_1, &os_err);
		        break;
			case SEND_DATA_FAIL:
				current_transmission = IDLE;
				send_data_ok = false;
				timerStop(sd_timeout_tim_id);
				OSSemPost(&semSendDataResponse, OS_OPT_POST_1, &os_err);
				break;
			default: break;
			}
		}
	}
}

static void sd_timeout_callback(){
	OS_ERR os_err;

	current_transmission = IDLE;
	send_data_ok = false;
	OSSemPost(&semSendDataResponse, OS_OPT_POST_1, &os_err);
}

static void ka_timeout_callback(){
	OS_ERR os_err;

	current_transmission = IDLE;
	keepalive_ok = false;
	OSSemPost(&semKeepAliveResponse, OS_OPT_POST_1, &os_err);
}

static void ka_blink_callback(){
	if(keep_alive_fails < MAX_KA_FAILS){
		toggleLED(BLUE);
	}else{
		setLED(BLUE, 0);
	}
}

/*******************************************************************************
 ******************************************************************************/
