/***************************************************************************//**
  @file     App.c
  @brief    Application functions
  @author   Nicolás Magliola
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "board.h"
#include "gpio.h"
#include "SysTick.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define SYSTICK_CYCLES SYSTICK_ISR_FREQUENCY_HZ //1 seg

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
enum{WAIT_PRESSED,WAIT_RELEASED};

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void baliza_update(void);

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
static uint32_t state = WAIT_PRESSED;
static bool baliza = false;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Función que se llama 1 vez, al comienzo del programa */
void App_Init_Baliza_SysTick (void)
{
	gpioMode(PIN_LED_RED, OUTPUT);
    gpioMode(PIN_LED_YELLOW, OUTPUT);
    gpioWrite(PIN_LED_YELLOW, LOW);
    gpioWrite(PIN_LED_RED, !LED_ACTIVE);
    gpioMode(PIN_SW3, INPUT);

    SysTick_Init(baliza_update);
}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run_Baliza_SysTick (void)
{

}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void baliza_update(void)
{
	static uint32_t counter = SYSTICK_CYCLES;

	switch(state) {
	case WAIT_PRESSED:
		if ( gpioRead(PIN_SW3) == SW_ACTIVE ){
			state = WAIT_RELEASED;
		}
		break;
	case WAIT_RELEASED:
		if ( !(gpioRead(PIN_SW3) == SW_ACTIVE) ){
			gpioToggle(PIN_LED_RED);
			baliza = !baliza;
			state = WAIT_PRESSED;
		}
		break;
	}

	if(baliza) {
		counter--;
		if(counter == 0) {
			gpioToggle(PIN_LED_YELLOW);
			counter = SYSTICK_CYCLES;
		}
	}
	else {
		gpioWrite(PIN_LED_YELLOW, LOW);
	}
}

/*******************************************************************************
 ******************************************************************************/
