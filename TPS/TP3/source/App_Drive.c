/***************************************************************************//**
  @file     App.c
  @brief    Application functions
  @author   Grupo 1
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <I2C_Drive.h>
#include <stdio.h>
#include <string.h>

#include "DRV_Board.h"
#include "DRV_Timers.h"
#include "hardware.h"


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

static uint8_t who_am_i;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/



void my_callback(void *data) {
  /* This callback function gets called once the sequence has been processed. Note that this gets called from an ISR, so
	 it should do as little as possible. */
}
/* Función que se llama 1 vez, al comienzo del programa */
void App_Init (void)
{
   	initBoard();
   	initTimers();


	//inicializo el I2C primero para poder establecer una comunicación
	I2C_init();
}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
{
	who_am_i = 0;
	char t[1] = {0x0D};	//who am i


	I2C_write_Transmition((0x1D<<1), t, 1, 0);
	I2C_read_Transmition((0x1D<<1), (char *) &who_am_i, 1, 1);

// application notes

	if(getSW(SW3)){
		toggleLED(BLUE);

		//i2cSendMessage(0, I2C_MODE_TX, 0x12, "Hola Mundo!", strlen("Hola Mundo!"));
	}
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/*******************************************************************************
 ******************************************************************************/
