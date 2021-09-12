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
	EVENT_EDITOR_PREV,
	EVENT_EDITOR_NEXT,
	EVENT_EDITOR_NONE
}editorEvent_t;

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

static menuState_t state_main(menuEvent_t);

static void start_number_editor(uint8_t editor_len, bool flag_next, bool flag_hidden);

//static void number_editor_left();

//static void number_editor_right();


// Si clickeas en '<' devuelve PREV
// Si clickeas en '>' devuelve NEXT
// Si clickeas en un dígito devuelve NONE

//static editorEvent_t number_editor_click();

/*******************************************************************************
 * GLOBAL VARIABLES ?)
 ******************************************************************************/

menuItem_t menu_item = ITEM_ID;
bool edit_flag = false;

uint8_t number_editor_len = 0;
uint8_t number_editor_array[EDITOR_BUFFER_LEN];
bool number_editor_flag_next = true;
bool number_editor_flag_hidden = false;

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
	static menuState_t menu_state = STATE_MAIN;
	menuEvent_t event = EVENT_NONE;

	//if(readerGetStatus()){
	//	event = EVENT_CARD;
	/*}else*/
	if(encoderGetStatus()){
		event = encoderGetEvent();	//Ya que en el enum se definen los mismos índices
	}

	if(event != EVENT_NONE){
		switch(menu_state){
		case STATE_MAIN:
			menu_state = state_main(event);
			break;
		case STATE_ID:
			//menu_state = state_id(event);
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

// editor_len: cuántos numero queres editar?
// flag_next: si queres que haya un botón de 'ok' (flechita para la derecha)
static void start_number_editor(uint8_t editor_len, bool flag_next, bool flag_hidden){
	number_editor_len = editor_len;
	number_editor_flag_hidden = flag_hidden;

	dispStopAutoscroll();
	dispSetBufferPos(0);
	dispClearBuffer();

	dispWriteChar(0, CHAR_PREV);	// '<'
	for(uint8_t i = 0; i < editor_len; i++){
		number_editor_array[i] = 0;
		dispWriteChar(i+1, (flag_hidden)? '-': '0');
	}
	number_editor_flag_next = flag_next;

	if(flag_next){
		dispWriteChar(editor_len + 1, CHAR_NEXT);  // '>'
	}
}

//static void number_editor_left();

//static void number_editor_right();


// Si clickeas en '<' devuelve PREV
// Si clickeas en '>' devuelve NEXT
// Si clickeas en un dígito devuelve NONE

//static editorEvent_t number_editor_click();

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
			start_number_editor(1, false);
			break;
		case ITEM_SPEED:
			next_state = STATE_SPEED;
			start_number_editor(1, false);
			break;
		default: break;
		}
	default: break;
	}

	return next_state;
}

/*******************************************************************************
 ******************************************************************************/
