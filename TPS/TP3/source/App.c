/***************************************************************************//**
  @file     Application.c
  @brief    App - TP3
  @author   Grupo 1 - M. Bergerman, P. Carranza Vélez, P. González Leiro, M. Moutin
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "MK64F12.h"
#include "hardware.h"
#include <stdio.h>
#include <string.h>
#include "DRV_UART.h"
#include "DRV_Timers.h"
#include "PDRV_ADC.h"
#include "DRV_WaveGen.h"



#include "PDRV_PIT.h"

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
	//initPIT(0x02FAF07F);
}

void App_Run() {
	wavegenSetFreq(0x0200000);
	delayLoop(10000000UL);
	wavegenSetFreq(0x0100000);
	delayLoop(10000000UL);
	/*static uint16_t val = 0;

	dacWrite(val);

	val = (val+1) % (0b1 << 12);

	delayLoop(1000UL);*/
}

static void delayLoop(uint32_t veces)
{
    while (veces--);
}
