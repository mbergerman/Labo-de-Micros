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
#include "MK64F12.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
static void callback(void);
static void callback2(void);
static void callback3(void);

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Función que se llama 1 vez, al comienzo del programa */
void App_Init_IRQ (void)
{
	gpioMode(PIN_LED_RED, OUTPUT);
	gpioMode(PIN_LED_GREEN, OUTPUT);
	gpioMode(PIN_LED_BLUE, OUTPUT);

    gpioMode(PIN_SW3, INPUT);
    gpioIRQ(PIN_SW3, GPIO_IRQ_MODE_RISING_EDGE, callback);
    gpioMode(PIN_SW2, INPUT_PULLUP);
    gpioIRQ(PIN_SW2, GPIO_IRQ_MODE_FALLING_EDGE, callback2);
    gpioMode(PIN_MySW, INPUT);
    gpioIRQ(PIN_MySW, GPIO_IRQ_MODE_FALLING_EDGE, callback3);
}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run_IRQ (void)
{

}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
static void callback(void) {
	gpioToggle(PIN_LED_RED);
}

static void callback2(void) {
	gpioToggle(PIN_LED_GREEN);
}

static void callback3(void) {
	gpioToggle(PIN_LED_BLUE);
}

/*******************************************************************************
 ******************************************************************************/
