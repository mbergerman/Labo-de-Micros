/***************************************************************************//**
  @file     App.c
  @brief    Application functions
  @author   Grupo 1
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdio.h>
#include <string.h>

#include "DRV_Board.h"
#include "DRV_Timers.h"
#include "DRV_Accelerometer.h"

//Debug
#include "PDRV_I2C.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static uint8_t xaxis[2];

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Función que se llama 1 vez, al comienzo del programa */
void App_Init (void)
{
   	initBoard();
   	initTimers();
   	initAccelerometer();
}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
{
	static bool btn_flag = false;
	static bool xaxis_sent = false;
	static bool accel_config = false;
	if(!accel_config) accel_config = accelConfigInit();

	if(getSW(SW3) && !btn_flag){
		btn_flag = true;
		toggleLED(BLUE);

		accelReadDataX(xaxis);
		xaxis_sent = true;

		//i2cSendSequence(0, init_sequence, 5, &device_id, NULL, NULL);
	}else{
		btn_flag = false;
	}

	if(xaxis_sent && accelReadingReady()){
		printf("%d %d\n", xaxis[0], xaxis[1]);
		xaxis_sent = false;

	}
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/*******************************************************************************
 ******************************************************************************/
