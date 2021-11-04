/***************************************************************************//**
  @file     NumberEditor.h
  @brief    Editor de numeros
  @author   Grupo 1
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

void numberEditorStart(uint8_t editor_len, bool flag_next, bool flag_hidden);

void numberEditorSetDigit(uint8_t n, uint8_t digit);

void numberEditorLeft();

void numberEditorRight();

editorEvent_t numberEditorClick();

void initNumberEditor();

uint32_t getBufferNumber();

uint8_t getNumberLen();

/*******************************************************************************
 ******************************************************************************/

#endif // _NUMBER_EDITOR_H_
