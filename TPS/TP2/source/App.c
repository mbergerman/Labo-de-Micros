/***************************************************************************//**
  @file     App.c
  @brief    Application functions
  @author   Nicolás Magliola
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdio.h>
#include <string.h>

#include "DRV_Display.h"
#include "DRV_LEDs.h"
#include "DRV_Encoder.h"
#include "DRV_Board.h"
#include "DRV_Timers.h"
#include "DRV_Reader.h"

#include "NumberEditor.h"
#include "AccountManager.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define ID_LENGTH	8
#define PIN_LENGTH	5
#define ERROR_TIME_MS 2500
#define WARNING_TIME_MS 2500
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
	LED_LATCH,
	LED_ERROR,
	LED_WARNING
};
/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static const char* menu_item_strings[] = {"Enter ID", "Edit", "Brightness", "Speed"};

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static menuState_t state_main(menuEvent_t);
static menuState_t state_id(menuEvent_t event);
static menuState_t state_pin(menuEvent_t event);
static menuEvent_t state_brightness(menuEvent_t event);

static menuState_t state_check_id(uint32_t id_value);

static void activate_latch();
static void deactivate_latch();
static void show_error();
static void unshow_error();
static void show_warning();
static void unshow_warning();

/*******************************************************************************
 * GLOBAL VARIABLES ?)
 ******************************************************************************/

static menuItem_t menu_item = ITEM_ID;
static bool edit_flag = false;
static bool edit_user_pin = false;

static char card_number[READER_NUM_MAX_LEN+1];
static uint8_t card_number_len = 0;

static User user_to_check;

static tim_id_t error_tim_id;
static tim_id_t warning_tim_id;
static tim_id_t latch_tim_id;

static uint16_t disp_scroll_speed = DISP_MEDIUM;

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
   	initReader();

	dispWriteBuffer(strlen(menu_item_strings[menu_item]), menu_item_strings[menu_item]);
	dispStartAutoScroll(disp_scroll_speed);

	//Inicializo timers de latch, error y warning
	error_tim_id = timerGetId();
	warning_tim_id = timerGetId();
	latch_tim_id = timerGetId();

	//Inicializo DB
	init_data_base();

	init_number_editor();
}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
{

	static menuState_t menu_state = STATE_MAIN;
	menuEvent_t event = EVENT_NONE;

	if(readerIsReady()){
		event = EVENT_CARD;
	}else if(encoderGetStatus()){
		event = encoderGetEvent();	//Ya que en el enum se definen los mismos índices
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
			menu_state = state_pin(event);
			break;
		case STATE_ADMIN:
			//menu_state = state_admin(event);
			break;
		case STATE_BRIGHTNESS:
			menu_state = state_brightness(event);
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
			enableReader();
			break;
		case ITEM_EDIT:
			edit_flag = true;
			next_state = STATE_ID;
			start_number_editor(ID_LENGTH, true, false);
			enableReader();
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

	switch(event){
	case EVENT_ENC_LEFT:
		number_editor_left();
		break;
	case EVENT_ENC_RIGHT:
		number_editor_right();
		break;
	case EVENT_ENC_CLICK:
		event_click = number_editor_click();
		if(event_click == EVENT_EDITOR_PREV){

			dispStartAutoScroll(disp_scroll_speed);
			dispWriteBuffer(strlen(menu_item_strings[menu_item]), menu_item_strings[menu_item]);

			next_state = STATE_MAIN;
		}else if(event_click == EVENT_EDITOR_NEXT) {
			uint32_t id_value = getBufferNumber();
			next_state = state_check_id(id_value);

			if(next_state == STATE_PIN)
				start_number_editor(PIN_LENGTH, true, true);

			if(next_state == STATE_MAIN){
				dispStartAutoScroll(disp_scroll_speed);
				dispWriteBuffer(strlen(menu_item_strings[menu_item]), menu_item_strings[menu_item]);
			}
		}
		break;
	case EVENT_CARD:

		getValueReader(card_number, &card_number_len);

		if(card_number_len >= ID_LENGTH){
			card_number[card_number_len] = '\0';
			uint32_t id_value = atoi(card_number + card_number_len - ID_LENGTH);
			next_state = state_check_id(id_value);
		}else{
			show_warning();
			timerStart(warning_tim_id, TIMER_MS2TICKS(ERROR_TIME_MS), TIM_MODE_SINGLESHOT, unshow_warning);
		}
		break;
	default: 
		break;
	}

	if(next_state != STATE_ID)
		disableReader();

	return next_state;
}


static menuState_t state_check_id(uint32_t id_value){
	menuState_t next_state = STATE_ID;
	if (id_check(id_value)){
		next_state = STATE_PIN;
		user_to_check = user_search(id_value);
		start_number_editor(PIN_LENGTH, 1, 1);
		if(is_blocked(user_to_check)){
			show_error();
			timerStart(error_tim_id, TIMER_MS2TICKS(ERROR_TIME_MS), TIM_MODE_SINGLESHOT, unshow_error);
			next_state = STATE_MAIN;
		}
	}else{
		show_error();
		timerStart(error_tim_id, TIMER_MS2TICKS(ERROR_TIME_MS), TIM_MODE_SINGLESHOT, unshow_error);
	}
	return next_state;
}

static menuState_t state_pin(menuEvent_t event) {

	menuState_t next_state = STATE_PIN;
	editorEvent_t event_click;

	switch (event) {
		case EVENT_ENC_LEFT:
			number_editor_left();
			break;
		case EVENT_ENC_RIGHT:
			number_editor_right();
			break;
		case EVENT_ENC_CLICK:
			event_click = number_editor_click();
			if (event_click == EVENT_EDITOR_PREV){
				start_number_editor(ID_LENGTH, true, false);
				next_state = STATE_ID;
				enableReader();
			}else if (event_click == EVENT_EDITOR_NEXT) {
				uint32_t pin_value = getBufferNumber();
				
				if (edit_user_pin){
					edit_PIN(user_to_check.ID, pin_value);
					next_state = STATE_MAIN;

					dispStartAutoScroll(disp_scroll_speed);
					dispWriteBuffer(strlen(menu_item_strings[menu_item]), menu_item_strings[menu_item]);

					edit_user_pin = false; 
				}else if ((pin_value == user_to_check.PIN) && !edit_user_pin){
					if (edit_flag) {
						if (!user_to_check.admin) {
							edit_user_pin = true;
							next_state = STATE_PIN;
							start_number_editor(PIN_LENGTH, 1, 1);
							edit_flag = false;
						}else{
							next_state = STATE_ADMIN;
							edit_flag = false;
							unblock_user(user_to_check);

						}
					}else{
						//activo latch "entro"
						activate_latch();
						timerStart(latch_tim_id, TIMER_MS2TICKS(LATCH_TIME_MS), TIM_MODE_SINGLESHOT, deactivate_latch);
						next_state = STATE_MAIN;

						dispStartAutoScroll(disp_scroll_speed);
						dispWriteBuffer(strlen(menu_item_strings[menu_item]), menu_item_strings[menu_item]);

						unblock_user(user_to_check);
					}
				}else{
					//Si no se valida ID y PIN, se prende el PIN de error
					show_error();
					timerStart(error_tim_id, TIMER_MS2TICKS(ERROR_TIME_MS), TIM_MODE_SINGLESHOT, unshow_error);
					add_error(user_to_check);
				}
			}
			break;
		default:
			break;
		}
	return next_state;
}


static void activate_latch() {
	dispSetLED(LED_LATCH);
}


static void deactivate_latch() {
	dispClearLED(LED_LATCH);
}


static void show_error() {
	dispSetLED(LED_ERROR);
}

static void unshow_error() {
	dispClearLED(LED_ERROR);
}

static void show_warning() {
	dispSetLED(LED_WARNING);
}

static void unshow_warning() {
	dispClearLED(LED_WARNING);
}



static menuEvent_t state_brightness(menuEvent_t event) {
	menuState_t next_state = STATE_BRIGHTNESS;
	editorEvent_t event_click = EVENT_EDITOR_NONE;

	switch(event){
	case EVENT_ENC_RIGHT:
		number_editor_right();
		break;
	case EVENT_ENC_LEFT:
		number_editor_left();
		break;
	case EVENT_ENC_CLICK:
		event_click = number_editor_click();
		if(event_click == EVENT_EDITOR_PREV) {
			dispUpdateBrightness(getBufferNumber());
			next_state = STATE_MAIN;
		}

	default:break;
	}

	return next_state;
}

/*******************************************************************************
 ******************************************************************************/
