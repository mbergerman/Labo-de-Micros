/***************************************************************************//**
  @file     PDRV_SPI.c
  @brief    SPI driver.
  @author   Grupo 1
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

<<<<<<< HEAD:TPS/TP3/source/App.c
#include <stdio.h>
#include <string.h>

#include "DRV_Board.h"
#include "DRV_Timers.h"
#include "DRV_Accelerometer.h"

//Debug
#include "PDRV_I2C.h"
=======
#include "PDRV_FTM.h"
#include "MK64F12.h"
#include "hardware.h"
>>>>>>> main:TPS/TP3/drivers/PDRV_FTM.c

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

<<<<<<< HEAD:TPS/TP3/source/App.c
=======
/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

>>>>>>> main:TPS/TP3/drivers/PDRV_FTM.c
/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

<<<<<<< HEAD:TPS/TP3/source/App.c
static uint8_t xaxis[2];
static uint8_t yaxis[2];
=======
static FTM_Type* const FTM_PTRS[] = FTM_BASE_PTRS;
>>>>>>> main:TPS/TP3/drivers/PDRV_FTM.c

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

<<<<<<< HEAD:TPS/TP3/source/App.c
/* Función que se llama 1 vez, al comienzo del programa */
void App_Init (void)
{
   	initBoard();
   	initTimers();
   	initAccelerometer();
}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
=======
void FTM_init(uint8_t FTM, FTM_Channel_t channel, FTM_Config_t config)
>>>>>>> main:TPS/TP3/drivers/PDRV_FTM.c
{
	static bool accel_config = false;
	if(!accel_config) accel_config = accelConfigInit();


	accelReadDataX(xaxis);
	timerDelay(TIMER_MS2TICKS(500));
	accelReadDataY(yaxis);
	timerDelay(TIMER_MS2TICKS(500));
	printf("X = %d , Y = %d\n", accelProcessResult(xaxis), accelProcessResult(yaxis));

}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/



/******************************************************************************/
