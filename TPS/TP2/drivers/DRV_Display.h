/*
 * DRV_Display.h
 *
 *  Created on: 29 ago. 2021
 *      Author: matia
 */

#ifndef DRV_DISPLAY_H_
#define DRV_DISPLAY_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "PDRV_GPIO.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define DISP_CHARS_NUM		4
#define DISP_LEDS_NUM		3
#define DISP_BUFFER_LEN	32		// Longitud del buffer de palabras

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

enum{
	SLOW,
	MEDIUM,
	FAST
};

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

void initDisplay();
void dispClearBuffer();
void dispWriteBuffer(int c, char* buffer);
void dispWriteChar(int n, char character);
bool dispValidChar(char character);

void dispScrollLeft();
void dispScrollRight();
void dispStartAutoscroll(int speed);
void dispStopAutoscroll();
void dispResetScroll();
void dispSetBufferPos(int n);	//Set position within buffer
int dispGetBufferPos();

void dispClearLED(int n);
void dispSetLED(int n);
void dispToggleLED();


/*******************************************************************************
 ******************************************************************************/



#endif /* DRV_DISPLAY_H_ */
