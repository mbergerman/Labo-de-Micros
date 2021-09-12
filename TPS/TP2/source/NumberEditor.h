/***************************************************************************//**
  @file     +Nombre del archivo (ej: template.h)+
  @brief    +Descripcion del archivo+
  @author   +Nombre del autor (ej: Salvador Allende)+
 ******************************************************************************/

#ifndef _NUMBER_EDITOR_H_
#define _NUMBER_EDITOR_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdbool.h>
#include <stdint.h>


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/



/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef enum{
	EVENT_EDITOR_PREV,
	EVENT_EDITOR_NEXT,
	EVENT_EDITOR_NONE
}editorEvent_t;

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

// +ej: extern unsigned int anio_actual;+


/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief TODO: completar descripcion
 * @param param1 Descripcion parametro 1
 * @param param2 Descripcion parametro 2
 * @return Descripcion valor que devuelve
*/
// +ej: char lcd_goto (int fil, int col);+

void start_number_editor(uint8_t editor_len, bool flag_next, bool flag_hidden);

void number_editor_left();

void number_editor_right();


// Si clickeas en '<' devuelve PREV
// Si clickeas en '>' devuelve NEXT
// Si clickeas en un dígito devuelve NONE

editorEvent_t number_editor_click();

void init_number_editor();

char* getBufferNumber();

uint8_t getNumberLen();
/*******************************************************************************
 ******************************************************************************/

#endif // _TEMPLATE_H_
