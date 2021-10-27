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
#include <math.h>
#include "DRV_UART.h"
#include "DRV_Timers.h"
#include "DRV_Sensors.h"
#include "DRV_Accelerometer.h"
#include "DRV_Buttons.h"
#include "DRV_LEDMatrix.h"
#include "DRV_WaveGen.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

//DEFAULT POINT
#define DEFAULT_BRIGHTNESS 	(50)
#define DEFAULT_POS			(pos_t){3,3}
#define	RED					{255,0,0}
#define DEFAULT_COLOR		RED
#define DEFAULT_POINT  		{DEFAULT_POS, DEFAULT_POS, DEFAULT_BRIGHTNESS, DEFAULT_COLOR}

//TOPICS
#define BRIGHTNESS_TOPIC	'B'
#define RED_TOPIC 			'r'
#define GREEN_TOPIC 		'g'
#define BLUE_TOPIC			'b'
#define SUSPEND_TOPIC		'S'
#define POSITION_TOPIC		'P'
#define AMPLITUD_TOPIC		'V'
#define FREQUENCY_TOPIC		'F'
#define LDR_TOPIC			'L'
#define ADAPTATIVE_TOPIC	'M'

//STATUS
#define SUSPENDED			'A'
#define AWAKE				'E'

#define SECONDS2SUSPEND		10

//ACC
#define	ACC_MEMORY_SIZE		10
#define MIN_POS				0
#define MAX_POS				7
#define ACC_THRESHOLD		250  // 80

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef struct {
	uint8_t x;
	uint8_t y;
} pos_t;


typedef struct {
	pos_t pos;
	pos_t last_pos;
	uint8_t brightness;
	color_t color;
} point_t;

typedef struct {
	uint16_t x;
	uint16_t y;
} std_acc_t;

typedef enum {VERTICAL, HORIZONTAL} direction_t;

typedef struct {
	float a;
	uint8_t f;
} wave_t;

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

//UART's timers callbacks
void timer_tx_bright_callback();
void timer_tx_pos_callback();
void timer_tx_status_callback();
void timer_rx_callback();
void timer_suspend_callback();
void timer_position_callback();
void timer_buttons_callback();

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

//static std_acc_t standard_deviation(acc_t* data, uint8_t len);

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static const color_t black_color = {0, 0, 0};

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static point_t point = DEFAULT_POINT;

// Timers' IDs
static tim_id_t timer_tx_bright;
static tim_id_t timer_rx;
static tim_id_t timer_tx_status;
static tim_id_t timer_acc;
static tim_id_t timer_tx_pos;

//flags
static bool change_color = true;
static bool change_brightness = true;
static bool change_position = true;
static bool change_amplitud = true;
static bool change_frequency = true;
static bool adaptative = false;

// suspend status
static char status = AWAKE;
static uint8_t wait = SECONDS2SUSPEND;


// ADC
static sensorData_t pot_data[2]; //save two values in order to notice significant changes
static sensorData_t LDR_data[2];

// Accel
typedef struct {
	acc_t data[ACC_MEMORY_SIZE];
	acc_t* ptr;
}acc_memory_t;
static acc_memory_t acc_memory;
static acc_t accel_value;

// buttons
static SR_button_t button = BUTTON_NONE;
static direction_t direction = HORIZONTAL;


//wavegen
static wave_t wave_params = {100.0, 10};

/*******************************************************************************
 *******************************************************************************
                        	FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void App_Init() {
	// Init Timer
	initTimers();

	// Init UART
	initUartCom();

	//Init Potentiometer
	initSensor(0);
	sensorStartReading(0);

	//Init LDR
	initSensor(1);
	sensorStartReading(1);

	//Init Accelerometer
	initAccelerometer();
	acc_memory.ptr = acc_memory.data;

	//Init buttons with SPI
	initButtons();

	//Init WaveGen
	initWaveGen(wave_params.a, wave_params.f);

	//Config UART Timers

	// send brightness to nodered
	timer_tx_bright = timerGetId();
	timerStart(timer_tx_bright, TIMER_MS2TICKS(120), TIM_MODE_PERIODIC, timer_tx_bright_callback);

	// uart receive packages
	timer_rx = timerGetId();
	timerStart(timer_rx, TIMER_MS2TICKS(100), TIM_MODE_PERIODIC, timer_rx_callback);

	// send status to nodered
	timer_tx_status = timerGetId();
	timerStart(timer_tx_status, TIMER_MS2TICKS(200), TIM_MODE_PERIODIC, timer_tx_status_callback);

	// update accelerometer
	timer_acc = timerGetId();
	timerStart(timer_acc, TIMER_MS2TICKS(250), TIM_MODE_PERIODIC, timer_position_callback);

	// send position to nodered
	timer_tx_pos = timerGetId();
	timerStart(timer_tx_pos, TIMER_MS2TICKS(150), TIM_MODE_PERIODIC, timer_tx_pos_callback);

	// suspend timer
	tim_id_t timer_suspend = timerGetId();
	timerStart(timer_suspend, TIMER_MS2TICKS(1000*SECONDS2SUSPEND/10), TIM_MODE_PERIODIC, timer_suspend_callback);

	//update position with buttons
	tim_id_t timer_buttons = timerGetId();
	timerStart(timer_buttons, TIMER_MS2TICKS(50), TIM_MODE_PERIODIC, timer_buttons_callback);

	LEDMatrix_init();
	LEDMatrix_updateLED(point.color, point.pos.y, point.pos.x);
}

void App_Run() {

	//Check accelerometer
	static bool accel_config = false;
	if (!accel_config) {
		accel_config = accelConfigInit();
	}

	accel_value = readAcceleration();

	//*acc_memory.ptr++ = readAcceleration();
	//if (acc_memory.ptr - acc_memory.data == ACC_MEMORY_SIZE) {
	//	acc_memory.ptr = acc_memory.data;
	//}
	timerDelay(10);

	// Read LDR
	LDR_data[0] = sensorGetValue(1);

	if ( abs(LDR_data[0]-LDR_data[1]) >= 3 ) {
		LDR_data[1] = LDR_data[0];

		package_t package;
		package.topic[0] = LDR_TOPIC;
		package.payload[0] = 255-LDR_data[0];
		sentPackage(package);
		if(adaptative) {
			point.brightness = (uint8_t)( 100 * (255-LDR_data[0]) / 255 );
			change_brightness = true;
			wait = SECONDS2SUSPEND;
		}
	}

	//WaveGen parameters

	if(change_amplitud) {
		// Stub for update_amplitud()
		//printf("New amplitud: %.2fV\n", wave_params.a);

		wavegenSetAmp(wave_params.a);

		change_amplitud = false;
	}

	if(change_frequency) {
		// Stub for update_amplitud()
		//printf("New frequency: %dHz\n", wave_params.f);

		wavegenSetFreq(wave_params.f);

		change_frequency = false;
	}

	if(status == AWAKE) {
		//Read Pot
		pot_data[0] = sensorGetValue(0);


		if ( abs(pot_data[0]-pot_data[1]) >= 10 ) {
			point.brightness = (uint8_t)( 100.0 * pot_data[0] / 255.0 );
			change_brightness = true;
			pot_data[1] = pot_data[0];
			wait = SECONDS2SUSPEND;
		}

		if(change_color) {
			// Stub for update_color()
			//printf("New color: rgb(%d,%d,%d)\n",point.color.r,point.color.g,point.color.b);

			LEDMatrix_updateLED(black_color, point.last_pos.y, point.last_pos.x);
			LEDMatrix_updateLED(point.color, point.pos.y, point.pos.x);

			change_color = false;
			wait = SECONDS2SUSPEND;
		}

		if(change_brightness) {
			// Stub for update_brightness()
			//printf("New brightness: %d%%\n",point.brightness);
			//

			LEDMatrix_setBrightness(point.brightness);

			change_brightness = false;
			wait = SECONDS2SUSPEND;
		}

		if(change_position) {
			// Stub for update_position()
			//printf("New position: (%d,%d)\n", point.pos.x, point.pos.y);
			//

			LEDMatrix_updateLED(black_color, point.last_pos.y, point.last_pos.x);
			LEDMatrix_updateLED(point.color, point.pos.y, point.pos.x);

			change_position = false;
			wait = SECONDS2SUSPEND;
		}
	}

}

// === Timer callbacks for UART communication ===

void timer_tx_bright_callback() {
	if(status == AWAKE){
		package_t package;
		package.topic[0] = BRIGHTNESS_TOPIC;
		package.payload[0] = point.brightness;
		sentPackage(package);
	}
}

void timer_tx_pos_callback() {
	if(status == AWAKE) {
		package_t package;
		package.topic[0] = POSITION_TOPIC;
		package.payload[0] = point.pos.x<<4 | point.pos.y;
		sentPackage(package);
	}
}

void timer_tx_status_callback() {
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
			LEDMatrix_updateLED(black_color, point.pos.y, point.pos.x);
		}
		else if(status == SUSPENDED) {
			status = AWAKE;
			LEDMatrix_updateLED(point.color, point.pos.y, point.pos.x);
		}
		wait = SECONDS2SUSPEND;
		emptyInbox();
		break;
	case AMPLITUD_TOPIC:
		wave_params.a = (float)(100.0 / 33.0)*package.payload[0];
		change_amplitud = true;
		break;
	case FREQUENCY_TOPIC:
		wave_params.f = package.payload[0];
		change_frequency = true;
		break;
	case ADAPTATIVE_TOPIC:
		if(package.payload[0]==1) {
			adaptative = true;
		}
		else {
			adaptative = false;
		}
		break;
	}
}

// ==== CHECK INACTIVITY ====
void timer_suspend_callback() {
	// si no hay actividad por 10 llamados consecutivos se suspende la app.
	if(status == AWAKE) {
		wait--;
		if(wait == 0) {
			LEDMatrix_updateLED(black_color, point.pos.y, point.pos.x);
			status = SUSPENDED;
			wait = SECONDS2SUSPEND;
		}
	}
}

// ==== UPDATE POSITION =====
void timer_position_callback() {

	if(abs(accel_value.x) > ACC_THRESHOLD || abs(accel_value.y) > ACC_THRESHOLD){
		if(!change_position){
			point.last_pos.x = point.pos.x;
			point.last_pos.y = point.pos.y;
		}
		change_position = true;

		if(accel_value.x > ACC_THRESHOLD) {
			point.pos.x += point.pos.x!=MAX_POS;
		}
		else if(accel_value.x < -ACC_THRESHOLD) {
			point.pos.x -= point.pos.x!=MIN_POS;
		}

		// Por como esta conectado, el sentido de Y está invertido
		if(accel_value.y < -ACC_THRESHOLD) {
			point.pos.y += point.pos.y!=MAX_POS;
		}
		else if(accel_value.y > ACC_THRESHOLD) {
			point.pos.y -= point.pos.y!=MIN_POS;
		}
	}
}


void timer_buttons_callback() {
	static SR_button_t last_button = BUTTON_NONE;
	static bool holding_down = false;
	button = getButtonPressed();
	if(button == last_button){
		switch(button) {
		case BUTTON_STATE:
			if(!holding_down){
				direction = !direction; //toggle direction
				holding_down = true;
			}
			break;
		case BUTTON_RIGHT:
			if(!holding_down){

				if(!change_position){
					point.last_pos.x = point.pos.x;
					point.last_pos.y = point.pos.y;
				}

				change_position = true;
				holding_down = true;
				if(direction == HORIZONTAL) {
					point.pos.x += point.pos.x!=MAX_POS;
				}
				else if(direction == VERTICAL) {
					point.pos.y += point.pos.y!=MAX_POS;
				}
			}
			break;
		case BUTTON_LEFT:
			if(!holding_down){

				if(!change_position){
					point.last_pos.x = point.pos.x;
					point.last_pos.y = point.pos.y;
				}

				change_position = true;
				holding_down = true;
				if(direction == HORIZONTAL) {
					point.pos.x -= point.pos.x!=MIN_POS;
				}
				else if(direction == VERTICAL) {
					point.pos.y -= point.pos.y!=MIN_POS;
				}
			}
			break;
		default:
			holding_down = false;
			break;
		}
	}
	last_button = button;
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/*static std_acc_t standard_deviation(acc_t* data, uint8_t len) {
   int32_t sum_x = 0;
   int32_t sum_y = 0;
   double deviation;
   double mean_x, stddeviation_x;
   double mean_y, stddeviation_y;
   long double variance_x = 0;
   long double variance_y = 0;

   for (int i=0; i < len ; i++) {
      sum_x += (data+i)->x;
      sum_y += (data+i)->y;
   }

   mean_x = (double) sum_x/len;
   mean_y = (double) sum_y/len;

   for (int i = 0 ; i < len; i++) {
      deviation = (data+i)->x - mean_x;
      variance_x += (long double)deviation * deviation / len;
      deviation = (data+i)->y - mean_y;
      variance_y += (long double)deviation * deviation / len;
   }


   stddeviation_x = sqrt(variance_x);
   stddeviation_y = sqrt(variance_y);
   std_acc_t std_acc = {stddeviation_x,stddeviation_y};

   return std_acc;
}*/
