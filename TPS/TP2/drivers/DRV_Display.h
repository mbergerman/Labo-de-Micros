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

#define DISP_CHARS_NUM	4
#define DISP_BUFFER_LEN	32		// Longitud del buffer de palabras

#define MIN_BRIGHT 1
#define MAX_BRIGHT 9

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

enum{
	DISP_SLOW=1000,
	DISP_MEDIUM=750,
	DISP_FAST=500
};

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

// initDisplay() - Initialize display.
void initDisplay();
// clearDisplay() - Clears the display.
void dispClearBuffer();
// dispClearChar() - Clears the character from position n from the buffer.
void dispClearChar(int n);
// dispSetChar() - sets character in position n of the buffer.
void dispWriteChar(int n, char character);

// dispSetWord() - sets word of length c in the buffer.
void dispWriteBuffer(int c, const char* word);

// dispUpdateRefreshRate() - Updates the refresh rate of the display.
void dispUpdateRefreshRate(int refreshRate);

// dispUpdateBrightness(int brightness) - Updates the brightness of the display - brightness: int between one and nine.
void dispUpdateBrightness(int brightness);

// dispScrollLeft() - Scroll display to the left.
void dispScrollLeft();
// dispScrollRight() - Scroll display to the right.
void dispScrollRight();
// dispStartAutoScroll(int speed) - Initialize autoscroll with specified speed.
void dispStartAutoScroll(int speed);
// dispStopAutoScroll() - Stop autoscroll.
void dispStopAutoScroll();
//void resetScroll();
// dispSetPosition(int n) - Sets n as the starting print position for the buffer.
void dispSetBufferPos(int n);
// dispGetPosition() - Get starting print position.
uint8_t dispGetBufferPos();

// dispSetDP(int n) - Turns on the decimal point of the nth seven segment. n = 0, 1, 2, 3
void dispSetDP(int n);
// dispSetDP(int n) - Turns off the decimal point of the nth seven segment. n = 0, 1, 2, 3
void dispClearDP(int n);
// dispSetDP(int n) - Toggles the decimal point of the nth seven segment. n = 0, 1, 2, 3
void dispToggleDP(int n);


/*******************************************************************************
 ******************************************************************************/



#endif /* DRV_DISPLAY_H_ */
