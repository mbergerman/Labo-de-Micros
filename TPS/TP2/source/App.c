/***************************************************************************//**
  @file     App.c
  @brief    Application functions
  @author   Nicolás Magliola
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "DRV_Board.h"
#include "DRV_Timers.h"
#include "DRV_Encoder.h"

//debug
#include <stdio.h>


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void blink_callback();

/*******************************************************************************
 * GLOBAL VARIABLES ?)
 ******************************************************************************/

tim_id_t encoder_tim_id;
uint32_t blink_period = 500;

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
   	initEncoder();

   	encoder_tim_id = timerGetId();

   	timerStart(encoder_tim_id, TIMER_MS2TICKS(blink_period), TIM_MODE_SINGLESHOT, blink_callback);
}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
{
	if(getStatus()){
		if(getData() == LEFT){
			blink_period = (blink_period > 100)? blink_period-100 : blink_period;
		}
		if(getData() == RIGHT){
			blink_period += 100;
		}
	}

}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void blink_callback(){
	toggleLED(BLUE);
   	timerStart(encoder_tim_id, TIMER_MS2TICKS(blink_period), TIM_MODE_SINGLESHOT, blink_callback);
}

/*******************************************************************************
 ******************************************************************************/
