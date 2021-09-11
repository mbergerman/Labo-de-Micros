/***************************************************************************//**
  @file     App.c
  @brief    Application functions
  @author   Nicolás Magliola
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "DRV_Board.h"
#include "DRV_Timers.h"
#include "DRV_Encoder.h"
#include "DRV_Display.h"

#include <stdio.h>
#include <string.h>


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define EDITOR_BUFFER_LEN	(DISP_BUFFER_LEN-2)

#define CHAR_PREV	'<'
#define CHAR_NEXT	'>'

#define ID_LENGTH	8
#define PIN_LENGTH	5

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

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static const char* menu_item_strings[] = {"Enter ID", "Edit", "Brightness", "Speed"};

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void blink_callback();

static void state_main(menuEvent_t);

/*******************************************************************************
 * GLOBAL VARIABLES ?)
 ******************************************************************************/

menuState_t menu_state = STATE_MAIN;
menuItem_t menu_item = ITEM_ID;
bool edit_flag = false;

uint8_t number_editor_len = 0;
uint8_t number_editor_array[EDITOR_BUFFER_LEN];
bool number_editor_flag_ok = true;

tim_id_t encoder_tim_id;
uint32_t blink_period = 500;

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

   	encoder_tim_id = timerGetId();

   	timerStart(encoder_tim_id, TIMER_MS2TICKS(blink_period), TIM_MODE_SINGLESHOT, blink_callback);


	dispWriteBuffer(strlen(menu_item_strings[menu_item]), menu_item_strings[menu_item]);
}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
{
	menuEvent_t event = EVENT_NONE;

	//if(readerGetStatus()){
	//	event = EVENT_CARD;
	/*}else*/
	if(encoderGetStatus()){
		event = encoderGetEvent();
	}

	if(event != EVENT_NONE){
		switch(menu_state){
		case STATE_MAIN:
			state_main(event); break;
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

static void start_number_editor(uint8_t editor_len, bool flag_ok){
	number_editor_len = editor_len;

	dispStopAutoscroll();
	dispSetBufferPos(0);
	dispClearBuffer();
	dispWriteChar(0, CHAR_PREV);
	for(uint8_t i = 0; i < editor_len; i++){
		number_editor_array[i] = 0;
		dispWriteChar(0, '0');
	}
	number_editor_flag_ok = flag_ok;

	if(flag_ok){
		dispWriteChar(editor_len + 1, CHAR_NEXT);
	}
}

static void state_main(menuEvent_t event){
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
			menu_state = STATE_ID;
			start_number_editor(ID_LENGTH, true);
			break;
		case ITEM_EDIT:
			edit_flag = true;
			menu_state = STATE_ID;
			start_number_editor(ID_LENGTH, true);
			break;
		case ITEM_BRIGHTNESS:
			menu_state = STATE_BRIGHTNESS;
			start_number_editor(1, false);
			break;
		case ITEM_SPEED:
			menu_state = STATE_SPEED;
			start_number_editor(1, false);
			break;
		default: break;
		}
	default: break;
	}
}

/*******************************************************************************
 ******************************************************************************/
