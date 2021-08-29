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

#define NUM_CHARS	4
#define NUM_LEDS	3
#define NUM_BUFFER	32		// Longitud del buffer de palabras

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
void clearDisplay();
void clearChar(int n);
void setChar(int n, char character);
void validChar(char character);

void setWord(int c, char* word);

void scrollLeft();
void scrollRight();
void startAutoscroll(int speed);
void stopAutoscroll();
void resetScroll();
void setPosition(int n);	//Set position within buffer
int getPosition();

void clearLED(int n);
void setLED(int n);
void toggleLED();


/*******************************************************************************
 ******************************************************************************/



#endif /* DRV_DISPLAY_H_ */
