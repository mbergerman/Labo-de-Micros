/***************************************************************************//**
  @file     Application.c
  @brief    App - TP3
  @author   Grupo 1 - M. Bergerman, P. Carranza Vélez, P. González Leiro, M. Moutin
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "MK64F12.h"
#include "hardware.h"
#include <stdio.h>
#include <string.h>
#include "DRV_UART.h"
#include "DRV_Timers.h"
#include "PDRV_ADC.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

//DEFAULT POINT
#define DEFAULT_BRIGHTNESS 	(50)
#define DEFAULT_POS			{0,0}
#define	RED					{255,0,0}
#define DEFAULT_COLOR		RED
#define DEFAULT_POINT  		{DEFAULT_POS,DEFAULT_BRIGHTNESS,DEFAULT_COLOR}

//TOPICS
#define BRIGHTNESS_TOPIC	'B'
#define RED_TOPIC 			'r'
#define GREEN_TOPIC 		'g'
#define BLUE_TOPIC			'b'
#define SUSPEND_TOPIC		'S'

//STATUS
#define SUSPENDED			'A'
#define AWAKE				'E'

#define SECONDS2SUSPEND		10

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef struct {
	uint8_t x;
	uint8_t y;
} pos_t;

typedef struct {
	uint8_t r;
	uint8_t g;
	uint8_t b;
} color_t;


typedef struct {
	pos_t pos;
	uint8_t brightness;
	color_t color;
} point_t;


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

//UART's timers callbacks
void timer_tx_callback();
void timer_rx_callback();
void timer_status_callback();
void timer_suspend_callback();

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static const point_t default_point = DEFAULT_POINT;

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static point_t point = default_point;

// UART timers
static tim_id_t timer_tx;
static tim_id_t timer_rx;
static tim_id_t timer_status;

//flags
static bool change_color = false;
static bool change_brightness = false;

// suspend
static char status = AWAKE;

static uint8_t wait = SECONDS2SUSPEND;

static ADCData_t ADC_data[3]; //save three values in order to notice significant changes

/*******************************************************************************
 *******************************************************************************
                        	FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void App_Init() {

	// Init UART
	initUartCom();

	// Init Timer
	initTimers();

	//Config UART Timers
	timer_tx = timerGetId();
	timerStart(timer_tx, TIMER_MS2TICKS(1000), TIM_MODE_PERIODIC, timer_tx_callback);
	timer_rx = timerGetId();
	timerStart(timer_rx, TIMER_MS2TICKS(100), TIM_MODE_PERIODIC, timer_rx_callback);
	timer_status = timerGetId();
	timerStart(timer_status, TIMER_MS2TICKS(200), TIM_MODE_PERIODIC, timer_status_callback);

	//Suspend timer
	tim_id_t timer_suspend = timerGetId();
	timerStart(timer_suspend, TIMER_MS2TICKS(1000*SECONDS2SUSPEND/10), TIM_MODE_PERIODIC, timer_suspend_callback);


	//ADC
	ADC_Init (ADC0_t, ADC_b8, ADC_c4, input_clock, ADC_mA, 0);
	ADC_SetInterruptMode(ADC0_t, true);
	ADC_StartConversion(ADC0_t, 0);
}

void App_Run() {

	if(status == AWAKE) {
		//Read ADC
		ADC_data[2] = ADC_data[1];
		ADC_data[1] = ADC_data[0];
		ADC_data[0] = ADC_getValue(ADC0_t);


		if ( abs(ADC_data[1]-ADC_data[0])>=5 && abs(ADC_data[2]-ADC_data[1])>=2  ) {
			point.brightness = (uint8_t)( 100 * ADC_getValue(ADC0_t) / 255 );
			change_brightness = true;
			wait = SECONDS2SUSPEND;
		}

		if(change_color) {
			// Stub for update_color()
			printf("New color: rgb(%d,%d,%d)\n",point.color.r,point.color.g,point.color.b);
			//
			change_color = false;
			wait = SECONDS2SUSPEND;
		}

		if(change_brightness) {
			// Stub for update_brightness()
			printf("New brightness: %d%%\n",point.brightness);
			//
			change_brightness = false;
			wait = SECONDS2SUSPEND;
		}

	}

}

// === Timer callbacks for UART communication ===

void timer_tx_callback() {
	if(status == AWAKE){
		package_t package;
		package.topic[0] = BRIGHTNESS_TOPIC;
		package.payload[0] = point.brightness;
		sentPackage(package);
	}
}

void timer_status_callback() {
	package_t package;
	package.topic[0] = SUSPEND_TOPIC;
	package.payload[0] = status;
	sentPackage(package);
}

void timer_rx_callback() {
	package_t package = receivePackage();

	switch(package.topic[0]) {
	case RED_TOPIC:
		point.color.r = (uint8_t)package.payload[0];
		break;
	case GREEN_TOPIC:
		point.color.g = (uint8_t)package.payload[0];
		break;
	case BLUE_TOPIC:
		point.color.b = (uint8_t)package.payload[0];
		change_color = true;
		break;
	case BRIGHTNESS_TOPIC:
		point.brightness = (uint8_t)package.payload[0];
		change_brightness = true;
		break;
	case SUSPEND_TOPIC:
		if(status == AWAKE) {
			status = SUSPENDED;
		}
		else if(status == SUSPENDED) {
			status = AWAKE;
		}
		wait = SECONDS2SUSPEND;
		emptyInbox();
		break;
	}

}

// ==== CHECK INACTIVITY ====
void timer_suspend_callback() {
	// si no hay actividad por 10 llamados consecutivos se suspende la app.
	if(status == AWAKE) {
		wait--;
		if(wait == 0) {
			status = SUSPENDED;
			wait = SECONDS2SUSPEND;

		}
	}
}



 
