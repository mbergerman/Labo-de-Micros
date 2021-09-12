/***************************************************************************//**
  @file     +Nombre del archivo (ej: template.c)+
  @brief    +Descripcion del archivo+
  @author   +Nombre del autor (ej: Salvador Allende)+
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "NumberEditor.h"
#include "DRV_Display.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define EDITOR_BUFFER_LEN	(DISP_BUFFER_LEN-2)
#define CHAR_PREV	'<'
#define CHAR_NEXT	'>'

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

// +ej: static void falta_envido (int);+


/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static editorState_t editor_state = STATE_EDITOR_SCROLL;
static uint8_t number_editor_len = 0;
static uint8_t number_editor_array[EDITOR_BUFFER_LEN];
static uint8_t* digit_pointer;

static bool number_editor_flag_next = true;
static bool number_editor_flag_hidden = false;


/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

// +ej: static int temperaturas_actuales[4];+


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

// editor_len: cuántos numero queres editar?
// flag_next: si queres que haya un botón de 'ok' (flechita para la derecha)
void start_number_editor(uint8_t editor_len, bool flag_next, bool flag_hidden){
	editor_state = STATE_EDITOR_SCROLL;
	number_editor_len = editor_len;
	number_editor_flag_hidden = flag_hidden;
	number_editor_flag_next = flag_next;


	dispStopAutoscroll();
	dispSetBufferPos(0);
	dispClearBuffer();

	dispWriteChar(0, CHAR_PREV);	// '<'
	for(uint8_t i = 0; i < editor_len; i++){
		number_editor_array[i] = 0;
		dispWriteChar(i+1, (flag_hidden)? '-': ('0'+i));
	}
	digit_pointer = number_editor_array;

	if(flag_next){
		dispWriteChar(editor_len + 1, CHAR_NEXT);  // '>'
	}
}

void number_editor_right() {
	switch(editor_state) {
	case STATE_EDITOR_SCROLL:
		if ( (digit_pointer - number_editor_array) < (number_editor_len + (uint8_t)number_editor_flag_next) ) {
			digit_pointer++;
			//dispDecimalPoint
		}
		if ((digit_pointer - &number_editor_array[dispGetBufferPos()] ) == 4) {
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
			digit_pointer--;
			//dispDecimalPoint
		}
		if ((digit_pointer - &number_editor_array[dispGetBufferPos()] ) == -1) {
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
		if ( digit_pointer - number_editor_array == 0 ) return EVENT_EDITOR_PREV;
		else if ( number_editor_flag_next && (digit_pointer - number_editor_array == (number_editor_len+1) )) {
			return EVENT_EDITOR_NEXT;
		}
		else {
			editor_state = STATE_EDITOR_MODIFY;
		}
		break;
	case STATE_EDITOR_MODIFY:
		editor_state = STATE_EDITOR_SCROLL;
		break;
	}
	return EVENT_EDITOR_NONE;
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/



