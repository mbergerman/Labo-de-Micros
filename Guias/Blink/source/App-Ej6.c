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


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static bool delayLoopWithInterruption(uint32_t veces);
static bool buttonPressed(pin_t pin);

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Función que se llama 1 vez, al comienzo del programa */
void App_Init6 (void)
{
	gpioMode(PIN_LED_RED, OUTPUT);
    gpioMode(PIN_LED_YELLOW, OUTPUT);
    gpioWrite(PIN_LED_YELLOW, LOW);
    gpioWrite(PIN_LED_RED, !LED_ACTIVE);
    gpioMode(PIN_SW3, INPUT);
}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run6 (void)
{
	// v1
//    static bool button_pressed = 0;

//    if ( gpioRead(PIN_SW3) == SW_ACTIVE ) {button_pressed = 1;}
//    else {button_pressed = 0;}
//    if ( button_pressed && (gpioRead(PIN_SW3) == !SW_ACTIVE) ) {
//    	gpioToggle(PIN_LED_GREEN);

	// v2
	enum{WAIT_PRESSED,WAIT_RELEASED};
	static bool state = WAIT_PRESSED;
	static bool baliza = 0;

	switch (state) {

	case WAIT_PRESSED:
		if ( buttonPressed(PIN_SW3) ){state = WAIT_RELEASED;}
		if (baliza) {
			gpioToggle(PIN_LED_YELLOW);
			if ( delayLoopWithInterruption(570000UL) ) // 570000 ciclos = 1seg / (1/114MHz * 200 ciclos)
			{
				gpioWrite(PIN_LED_YELLOW, LOW);
				state = WAIT_RELEASED;
			}
		}
		break;//!!!!

	case WAIT_RELEASED:
		if ( !buttonPressed(PIN_SW3) ){
			gpioToggle(PIN_LED_RED);
			baliza = !baliza;
			state = WAIT_PRESSED;
		}
		break;
	}


}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static bool buttonPressed(pin_t pin) {
	if ( gpioRead(pin) == SW_ACTIVE ) {
		return 1;
	}
	else {
		return 0;
	}
}

static bool delayLoopWithInterruption(uint32_t veces)
{
    while (veces--) {
    	if ( buttonPressed(PIN_SW3) ){
    		return 1;
    	}
    }
    return 0;
}



/*******************************************************************************
 ******************************************************************************/
