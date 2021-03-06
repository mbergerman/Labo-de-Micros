/***************************************************************************//**
  @file     DRV_LEDMatrix.h
  @brief    LED matrix driver.
  @author   Grupo 1
 ******************************************************************************/

#ifndef _LEDMATRIX_H_
#define _LEDMATRIX_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define HEIGHT	8
#define WIDTH	8

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef struct color_t
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
}color_t;

typedef color_t LEDMatrix_t[HEIGHT][WIDTH];

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

void LEDMatrix_init();
void LEDMatrix_updateLED(color_t led, uint8_t height, uint8_t width);
void LEDMatrix_setBrightness(float brightness);
float LEDMatrix_getBrightness();

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 ******************************************************************************/

#endif // _LEDMATRIX_H_
