/***************************************************************************//**
  @file     DRV_PIT.c
  @brief    +Descripcion del archivo+
  @author   matia
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <PDRV_PIT.h>
#include "MK64F12.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/



/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/



/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

// +ej: unsigned int anio_actual;+


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

// +ej: static void falta_envido (int);+


/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

// +ej: static const int temperaturas_medias[4] = {23, 26, 24, 29};+


/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void (*pit_callback)(void) = NULL;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void initPIT(uint32_t pit_time){
	static bool alreadyInit = false;
	if (alreadyInit) return;

	// Enable PIT clock
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;

	// Turn on PIT
	PIT->MCR = 0;

	// Configure PIT to produce an interrupt every 1s
	PIT->CHANNEL[0].LDVAL = pit_time;
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE_MASK | PIT_TCTRL_TEN_MASK; // Enable interrupt and enable timer

	// Enable interrupt registers ISER and ICPR
	NVIC_EnableIRQ(PIT0_IRQn);
}

void pitSetTime(uint32_t pit_time){
	PIT->CHANNEL[0].LDVAL = pit_time;
}

void pitSetCallback(void (*callback_fn)()){
	pit_callback = callback_fn;
}

void pitStartDMAtransfer(uint32_t channel){
	//Start DMA channel 0 transfer
	DMA0->TCD[channel].CSR |= DMA_CSR_START_MASK;
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

//ISR of the PIT timer. Clears the PIT interrupt flag and starts DMA channel 0 transfer
void PIT0_IRQHandler(void)
{
	// Clear interrupt
	PIT->CHANNEL[0].TFLG = PIT_TFLG_TIF_MASK;

	if(pit_callback != NULL){
		pit_callback();
	}
}
