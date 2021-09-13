/***************************************************************************//**
  @file     +Nombre del archivo (ej: template.c)+
  @brief    +Descripcion del archivo+
  @author   +Nombre del autor (ej: Salvador Allende)+
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "stdlib.h"

#include "DRV_Display.h"
#include "DRV_Timers.h"
#include "NumberEditor.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define EDITOR_BUFFER_LEN	(DISP_BUFFER_LEN)
#define CHAR_PREV	'<'
#define CHAR_NEXT	'>'
#define BLINK_DP_MS	250

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef enum {
	STATE_EDITOR_SCROLL,
	STATE_EDITOR_MODIFY
}editorState_t;


/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

// +ej: unsigned int anio_actual;+


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void toggle_current_dp();

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/



/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static editorState_t editor_state = STATE_EDITOR_SCROLL;
static uint8_t number_editor_len = 0;
static uint8_t number_editor_array[EDITOR_BUFFER_LEN];
static uint8_t* digit_pointer;

static char display_editor_array[EDITOR_BUFFER_LEN];

static bool number_editor_flag_next = true;
static bool number_editor_flag_hidden = false;

static tim_id_t blink_dp_tim_id;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void init_number_editor(){
	blink_dp_tim_id = timerGetId();
}

// editor_len: cuántos numero queres editar?
// flag_next: si queres que haya un botón de 'ok' (flechita para la derecha)
void start_number_editor(uint8_t editor_len, bool flag_next, bool flag_hidden){
	editor_state = STATE_EDITOR_SCROLL;
	number_editor_len = editor_len;
	number_editor_flag_hidden = flag_hidden;
	number_editor_flag_next = flag_next;

	dispStopAutoScroll();
	dispSetBufferPos(0);
	dispClearBuffer();

	display_editor_array[0] = CHAR_PREV;
	for(uint8_t i = 0; i < editor_len; i++){
		number_editor_array[i+1] = 0;
		display_editor_array[i+1] = (number_editor_flag_hidden)? '-': '0';
	}
	digit_pointer = number_editor_array;
	dispSetDP( 0 );

	if(flag_next){
		display_editor_array[editor_len + 1] = CHAR_NEXT;
	}

	dispWriteBuffer(number_editor_len+1+flag_next , display_editor_array);
}

void set_number_editor_digit(uint8_t n, uint8_t digit){
	number_editor_array[n+1] = digit;
	display_editor_array[n+1] = (number_editor_flag_hidden)? '-': '0'+digit;
	dispWriteChar(n+1, display_editor_array[n+1]);
}

void number_editor_right() {
	switch(editor_state) {
	case STATE_EDITOR_SCROLL:
		if ( (digit_pointer - number_editor_array) < (number_editor_len + (uint8_t)number_editor_flag_next) ) {

			// check hideness
			bool hide = false;
			uint8_t index = digit_pointer-number_editor_array;
			if(number_editor_flag_hidden) {
				//if(index>=1 && index<=number_editor_len)
				hide = true;
			}

			if(hide && index>0) {
				dispWriteChar(index , '-' ); //hide value
			}

			dispClearDP( digit_pointer - &number_editor_array[dispGetBufferPos()] );

			digit_pointer++;

			dispSetDP( digit_pointer - &number_editor_array[dispGetBufferPos()] );

			if(hide && index<number_editor_len){
				dispWriteChar(index+1, (char)('0'+*digit_pointer) ); // show value to modify
			}

		}
		if ((digit_pointer - &number_editor_array[dispGetBufferPos()] ) == DISP_CHARS_NUM) {
			dispSetDP(3);
			dispScrollRight();
		}
		break;
	case STATE_EDITOR_MODIFY:
		*digit_pointer = (*digit_pointer+1)%10;
		dispWriteChar(digit_pointer-number_editor_array , '0' + *digit_pointer);
		break;
	}
}

void number_editor_left() {
	switch(editor_state) {
	case STATE_EDITOR_SCROLL:
		if ( (digit_pointer - number_editor_array) > 0 ) {

			// check hideness
			bool hide = false;
			uint8_t index = digit_pointer-number_editor_array;
			if(number_editor_flag_hidden) {
				if(index>=1 && index<=number_editor_len) hide = true;
			}

			if(hide) {
				dispWriteChar(index , '-' ); //hide value
			}

			dispClearDP( digit_pointer - &number_editor_array[dispGetBufferPos()] );

			digit_pointer--;

			dispSetDP( digit_pointer - &number_editor_array[dispGetBufferPos()] );

			if(hide && index>1) {
				dispWriteChar(index-1, (char)('0'+*digit_pointer) ); // show value to modify
			}
		}
		if ((digit_pointer - &number_editor_array[dispGetBufferPos()] ) == -1) {
			dispSetDP(0);
			dispScrollLeft();
		}
		break;
	case STATE_EDITOR_MODIFY:
		*digit_pointer = (*digit_pointer-1 < 0) ? 9 : *digit_pointer-1;
		dispWriteChar(digit_pointer-number_editor_array , '0' + *digit_pointer);
		break;
	}
}


// Si clickeas en '<' devuelve PREV
// Si clickeas en '>' devuelve NEXT
// Si clickeas en un dígito devuelve NONE

editorEvent_t number_editor_click() {
	switch(editor_state) {
	case STATE_EDITOR_SCROLL:
		if ( digit_pointer - number_editor_array == 0 ){
			dispClearDP(digit_pointer - &number_editor_array[dispGetBufferPos()]);
			return EVENT_EDITOR_PREV;
		}
		else if ( number_editor_flag_next && (digit_pointer - number_editor_array == (number_editor_len+1) )) {
			dispClearDP(digit_pointer - &number_editor_array[dispGetBufferPos()]);
			return EVENT_EDITOR_NEXT;
		}
		else {
			timerStart(blink_dp_tim_id, TIMER_MS2TICKS(BLINK_DP_MS), TIM_MODE_PERIODIC, toggle_current_dp);
			editor_state = STATE_EDITOR_MODIFY;
		}
		break;
	case STATE_EDITOR_MODIFY:
		timerStop(blink_dp_tim_id);
		dispSetDP( digit_pointer - &number_editor_array[dispGetBufferPos()] );
		editor_state = STATE_EDITOR_SCROLL;
		break;
	}
	return EVENT_EDITOR_NONE;
}


uint32_t getBufferNumber(){
	uint32_t result = 0;
	for(int i = 1; i <= number_editor_len; i++){
		if(number_editor_array[i] < 10){
			result *= 10;
			result += number_editor_array[i];
		}
	}
	return result;
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void toggle_current_dp(){
	dispToggleDP( digit_pointer - &number_editor_array[dispGetBufferPos()] );
}
