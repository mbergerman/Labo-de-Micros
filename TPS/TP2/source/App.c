/***************************************************************************//**
  @file     App.c
  @brief    Application functions
  @author   Nicolás Magliola
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "DRV_Display.h"
#include "DRV_LEDs.h"
#include "DRV_Encoder.h"
#include "DRV_Board.h"
#include "DRV_Timers.h"
#include "NumberEditor.h"

#include <stdio.h>
#include <string.h>


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define ID_LENGTH	8
#define PIN_LENGTH	5
#define ERROR_TIME_MS 5000
#define LATCH_TIME_MS 5000

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef enum{
	EVENT_ENC_LEFT = ENC_LEFT,
	EVENT_ENC_RIGHT = ENC_RIGHT,
	EVENT_ENC_CLICK = ENC_CLICK,
	EVENT_NONE = ENC_NONE,
	EVENT_CARD,
}menuEvent_t;

typedef enum{
	STATE_MAIN,
	STATE_ID,
	STATE_PIN,
	STATE_EDIT,
	STATE_ADMIN,
	STATE_BRIGHTNESS,
	STATE_SPEED
}menuState_t;

typedef enum{
	ITEM_ID,
	ITEM_EDIT,
	ITEM_BRIGHTNESS,
	ITEM_SPEED,
	NUM_ITEMS
}menuItem_t;

enum {
	LED_ERROR,
	LED_OK,
	LED_LATCH
};
/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static const char* menu_item_strings[] = {"Enter ID", "Edit", "Brightness", "Speed"};

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void blink_callback();

static menuState_t state_main(menuEvent_t);

static menuState_t state_id(menuEvent_t event);


/*******************************************************************************
 * GLOBAL VARIABLES ?)
 ******************************************************************************/

static menuItem_t menu_item = ITEM_ID;
static bool edit_flag = false;
static bool edit_user_pin = false;

static tim_id_t encoder_tim_id;
static uint32_t blink_period = 500;

static char* card_number;
static uint8_t card_number_len = 0;

static User user_to_check;

static tim_id_t error_tim_id;
static tim_id_t latch_tim_id;

static uint8_t error_counter = 0;
/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/


/* Función que se llama 1 vez, al comienzo del programa */
void App_Init (void)
{
   	initBoard();
   	initTimers();
   	initEncoder();
   	initDisplay();
   	initLEDs();

   	encoder_tim_id = timerGetId();

   	timerStart(encoder_tim_id, TIMER_MS2TICKS(blink_period), TIM_MODE_SINGLESHOT, blink_callback);


	dispWriteBuffer(strlen(menu_item_strings[menu_item]), menu_item_strings[menu_item]);
	dispStartAutoScroll(DISP_FAST);

	//Inicializo timers de latch y error
	error_tim_id = timerGetId();
	latch_tim_id = timerGetId();

	//Inicializo DB
	init_data_base();
}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
{

	static menuState_t menu_state = STATE_MAIN;
	menuEvent_t event = EVENT_NONE;


	//if(readerGetStatus()){
	//	event = EVENT_CARD;
	/*}else*/
	if(encoderGetStatus()){
		event = encoderGetEvent();	//Ya que en el enum se definen los mismos índices
	}

	if(menu_state == STATE_ID) {
		initReader();
		if(readerIsReady()) {
			getValueReader(card_number, &card_number_len);
			if(card_number_len != 0) {
				event = EVENT_CARD;
			}
		}
		stopReader();
	}

	if(event != EVENT_NONE){
		switch(menu_state){
		case STATE_MAIN:
			menu_state = state_main(event);
			break;
		case STATE_ID:
			menu_state = state_id(event);
			break;
		case STATE_PIN:
			//menu_state = state_pin(event);
			break;
		case STATE_ADMIN:
			//menu_state = state_admin(event);
			break;
		case STATE_BRIGHTNESS:
			//menu_state = state_brightness(event);
			break;
		case STATE_SPEED:
			//menu_state = state_speed(event);
			break;
		default: break;
		}
	}

}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void blink_callback(){
	toggleLED(BLUE);
   	timerStart(encoder_tim_id, TIMER_MS2TICKS(blink_period), TIM_MODE_SINGLESHOT, blink_callback);
}



static menuState_t state_main(menuEvent_t event){
	menuState_t next_state = STATE_MAIN;

	switch(event){
	case EVENT_ENC_LEFT:
		menu_item = (menu_item + NUM_ITEMS - 1) % NUM_ITEMS;
		dispWriteBuffer(strlen(menu_item_strings[menu_item]), menu_item_strings[menu_item]);
		break;
	case EVENT_ENC_RIGHT:
		menu_item = (menu_item + 1) % NUM_ITEMS;
		dispWriteBuffer(strlen(menu_item_strings[menu_item]), menu_item_strings[menu_item]);
		break;
	case EVENT_ENC_CLICK:
		switch(menu_item){
		case ITEM_ID:
			edit_flag = false;
			next_state = STATE_ID;
			start_number_editor(ID_LENGTH, true, false);
			break;
		case ITEM_EDIT:
			edit_flag = true;
			next_state = STATE_ID;
			start_number_editor(ID_LENGTH, true, false);
			break;
		case ITEM_BRIGHTNESS:
			next_state = STATE_BRIGHTNESS;
			start_number_editor(1, false, false);
			break;
		case ITEM_SPEED:
			next_state = STATE_SPEED;
			start_number_editor(1, false, false);
			break;
		default: break;
		}
	default: break;
	}

	return next_state;
}

static menuState_t state_id(menuEvent_t event) {

	menuState_t next_state = STATE_ID;
	editorEvent_t event_click = EVENT_EDITOR_NONE;
	error_counter = 0;

	switch(event){
	case EVENT_ENC_LEFT:
		number_editor_left();
		break;
	case EVENT_ENC_RIGHT:
		number_editor_right();
		break;
	case EVENT_ENC_CLICK:
		event_click = number_editor_click();
		if(event_click == EVENT_EDITOR_PREV) next_state = STATE_MAIN;
		else if(event_click == EVENT_EDITOR_NEXT) {
			char* buffer = getBufferNumber();
			buffer[getNumberLen()] = '\0';
			id_value = atoi(buffer);
			next_state = state_check_id(id_value);
		}
		break;
	case EVENT_CARD:
		card_number[card_number_len] = '\0';
		uint32_t id_value = atoi(card_number);
		next_state = state_check_id(id_value);
		break;
	default: 
	break;
	}

	return next_state;
}


static menuState_t state_check_id(uint32_t id_value){
	next_state = STATE_ID;
	if (id_check(id_value))
		{
		next_state = STATE_PIN;
		user_to_check = user_search(id_value);
		start_number_editor(PIN_LENGTH, 1, 1);
		if(user_to_check.is_blocked()){
			timerStart(error_tim_id, TIMER_MS2TICKS(ERROR_TIME_MS), TIM_MODE_SINGLESHOT, unshow_LED_error());
			show_LED_error();
			next_state = STATE_MAIN;
			}
		}
	else {
		timerStart(error_tim_id, TIMER_MS2TICKS(ERROR_TIME_MS), TIM_MODE_SINGLESHOT, unshow_LED_error());
		show_LED_error();
		}
	return next_state;
}

static menuState_t state_pin(menuEvent_t event) {

	menuState_t next_state = STATE_PIN;


	switch (event) {
		case EVENT_ENC_LEFT:
			number_editor_left();
			break;
		case EVENT_ENC_RIGHT:
			number_editor_right();
			break;
		case EVENT_ENC_CLICK:
			editorEvent_t event_click = number_editor_click();
			if (event_click == EVENT_EDITOR_PREV) next_state = STATE_MAIN;
			else if (event_click == EVENT_EDITOR_NEXT) {
				char* buffer = getBufferNumber();
				buffer[getNumberLen()] = '\0';
				pin_value = atoi(buffer);
				
				if (edit_user_pin){
					edit_PIN(user_to_check.ID, pin_value);
					next_state = STATE_MAIN;
					edit_user_pin = false; 
				}
				else if ((pin_value == user_to_check.PIN) && !edit_user_pin){
					if (edit_flag) {
						if (!user_to_check.admin) {
							edit_user_pin = true;
							next_state = STATE_PIN;
							start_number_editor(PIN_LENGTH, 1, 1);
							edit_flag = false;
						}
						else {
							next_state = STATE_ADMIN;
							edit_flag = false;
							user_to_check.unblock_user();

						}
					}
					else {
						//activo latch "entro"
						timerStart(latch_tim_id, TIMER_MS2TICKS(LATCH_TIME_MS), TIM_MODE_SINGLESHOT, deactivate_latch());
						activate_latch();
						next_state = STATE_MAIN;
						user_to_check.unblock_user();
					}

				}
				else {
					//Si no se valida ID y PIN, se prende el PIN de error
					timerStart(error_tim_id, TIMER_MS2TICKS(LATCH_TIME_MS), TIM_MODE_SINGLESHOT, unshow_LED_error());
					show_LED_error();
					user_to_check.error_counter++;
				}
			}
			break;
		default:
			break;
		}
}


void activate_latch() {
	
	dispSetLED(LED_LATCH);
}


void  deactivate_latch() {

	dispClearLED(LED_LATCH);
}


void show_LED_error() {

	dispSetLED(LED_ERROR);
}

void unshow_LED_error() {

	dispClearLED(LED_ERROR);
}
/*******************************************************************************
 ******************************************************************************/
