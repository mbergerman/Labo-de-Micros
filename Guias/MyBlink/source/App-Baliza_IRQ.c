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

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void baliza_toggle(void);
static void button_irq(void);

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
static bool baliza = false;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Función que se llama 1 vez, al comienzo del programa */
void App_Init_Baliza_IRQ (void)
{
	gpioMode(PIN_LED_RED, OUTPUT);
    gpioMode(PIN_LED_YELLOW, OUTPUT);
    gpioWrite(PIN_LED_YELLOW, LOW);
    gpioWrite(PIN_LED_RED, !LED_ACTIVE);
    gpioMode(PIN_SW2, INPUT_PULLUP);

    SysTick_Init(baliza_toggle);

    gpioIRQ(PIN_SW2, GPIO_IRQ_MODE_RISING_EDGE, button_irq);
}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run_Baliza_IRQ (void)
{

}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void baliza_toggle(void)
{
	static uint32_t counter = SYSTICK_CYCLES;

	if(baliza) {
		counter--;
		if(counter == 0) {
			gpioToggle(PIN_LED_YELLOW);
			counter = SYSTICK_CYCLES;
		}
	}
}

static void button_irq(void) {
	baliza = !baliza;
	gpioToggle(PIN_LED_RED);
	if(baliza) gpioWrite(PIN_LED_YELLOW,HIGH);
	else gpioWrite(PIN_LED_YELLOW, LOW);
}

/*******************************************************************************
 ******************************************************************************/
