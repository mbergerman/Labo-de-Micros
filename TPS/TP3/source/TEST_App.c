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
#include "TEST_I2C.h"


//debug
#include "fsl_device_registers.h"


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

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/



void my_callback(void *data) {
  /* This callback function gets called once the sequence has been processed. Note that this gets called from an ISR, so
	 it should do as little as possible. */
}

static uint32_t status;
static uint16_t init_sequence[] = {0x3a, 0x0d, I2C_RESTART, 0x3b, I2C_READ};
static uint8_t device_id = 0;		/* Will contain the device id after sequence has been processed. */


/* Función que se llama 1 vez, al comienzo del programa */
void App_Init (void)
{
   	initBoard();

   	NVIC_EnableIRQ(I2C0_IRQn);

	/* These depend on the particular chip being used. */
	SIM->SCGC4 |= SIM_SCGC4_I2C0_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
	PORTE->PCR[24] = PORT_PCR_MUX(5) | PORT_PCR_ODE_MASK;
	PORTE->PCR[25] = PORT_PCR_MUX(5) | PORT_PCR_ODE_MASK;

	status = i2c_init(0, 0x2, 0x26);

   	initTimers();
}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
{
	static bool flag = false;

	if(getSW(SW3) && !flag){
		flag = true;

		toggleLED(BLUE);

		status = i2c_send_sequence(0, init_sequence, 5, &device_id, my_callback, (void*)0x1234);

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
