/***************************************************************************//**
  @file     Application.c
  @brief    App - TP3
  @author   Grupo 1 - M. Bergerman, P. Carranza Vélez, P. González Leiro, M. Moutin
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "Util.h"
#include <string.h>
#include <stdio.h>
#include "DRV_UART.h"
#include "DRV_Timers.h"
#include "DRV_WaveGen.h"
#include "DRV_Buttons.h"
#include "DRV_Board.h"

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

	//initWaveGen();

	initBoard();

	initButtons();
}

void App_Run() {
	static SR_button_t sr_data = 0;

	toggleLED(BLUE);

	sr_data = getButtonPressed();

	if(sr_data == BUTTON_LEFT){
		printf("L\n");
	}else if(sr_data == BUTTON_RIGHT){
		printf("R\n");
	}else if(sr_data == BUTTON_STATE){
		printf("S\n");
	}

	timerDelay(TIMER_MS2TICKS(500));


	/*wavegenSetFreq(0x0200000);
	delayLoop(10000000UL);
	wavegenSetFreq(0x0100000);
	delayLoop(10000000UL);*/
	/*static uint16_t val = 0;

	dacWrite(val);

	val = (val+1) % (0b1 << 12);

	delayLoop(1000UL);*/
}

static void delayLoop(uint32_t veces)
{
    while (veces--);
}
