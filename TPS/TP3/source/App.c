/***************************************************************************//**
  @file     Application.c
  @brief    App - TP3
  @author   Grupo 1 - M. Bergerman, P. Carranza Vélez, P. González Leiro, M. Moutin
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <string.h>
#include <stdio.h>
#include "Util.h"
#include "DRV_UART.h"
#include "DRV_Timers.h"
#include "DRV_WaveGen.h"
#include "DRV_Buttons.h"
#include "DRV_Board.h"
#include "DRV_LEDMatrix.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void delayLoop(uint32_t veces);

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/


/*******************************************************************************
 *******************************************************************************
                        	FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void App_Init() {

	// Init UART
	//initUartCom();

	// Init Timer
	//initTimers();

	initWaveGen();
	initBoard();

	//LEDMatrix_init();
}

void App_Run() {
	static uint8_t w = 0;

	color_t led_color = {120, 255, 0};

	//LEDMatrix_updateLED(led_color, 3, w);

	//timerDelay(TIMER_MS2TICKS(500));

	//w = (w+1)%8;


	wavegenSetFreq(5);
	delayLoop(10000000UL);
	wavegenSetFreq(10);
	delayLoop(10000000UL);

}

static void delayLoop(uint32_t veces)
{
    while (veces--);
}
