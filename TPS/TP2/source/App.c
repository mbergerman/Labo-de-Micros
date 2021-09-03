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

#include "PDRV_GPIO.h"	// Debug


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

void animateLEDs();

/*******************************************************************************
 * GLOBAL VARIABLES ?)
 ******************************************************************************/

static tim_id_t led_tim_id;

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

   	led_tim_id = timerGetId();
   	timerStart(led_tim_id, TIMER_MS2TICKS(1000), TIM_MODE_PERIODIC, animateLEDs);
}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
{

//	setLED(RED);
//	timerDelay(TIMER_MS2TICKS(500));
//	clearLED(RED);
//	timerDelay(TIMER_MS2TICKS(500));

	static bool last_sw2 = false;

	if( getSW(SW2) && !last_sw2 ){
		timerStop(led_tim_id);
		timerDelay(TIMER_MS2TICKS(1000));
		clearLED(RED);
		clearLED(GREEN);
		clearLED(BLUE);
		timerDelay(TIMER_MS2TICKS(1000));
		setLED(RED);
		setLED(GREEN);
		setLED(BLUE);
	}
	if( !getSW(SW2) && last_sw2 ){
		timerStart(led_tim_id, TIMER_MS2TICKS(1000), TIM_MODE_PERIODIC, animateLEDs);
	}

	last_sw2 = getSW(SW2);
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void animateLEDs(){
	static int x = 0;

	switch(x){
	case 0:
		setLED(RED);
		clearLED(GREEN);
		clearLED(BLUE);
		break;
	case 1:
		clearLED(RED);
		setLED(GREEN);
		clearLED(BLUE);
		break;
	case 2:
		clearLED(RED);
		clearLED(GREEN);
		setLED(BLUE);
		//timerDelay(TIMER_MS2TICKS(30));
		break;
	}

	x = (x+1)%3;
}


/*******************************************************************************
 ******************************************************************************/
