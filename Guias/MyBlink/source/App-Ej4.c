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




/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Función que se llama 1 vez, al comienzo del programa */
void App_Init4 (void)
{
    gpioMode(PIN_LED_GREEN, OUTPUT);
    //gpioWrite(PIN_LED_GREEN, !LED_ACTIVE);
    gpioMode(PIN_SW3, INPUT);
}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run4 (void)
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

	switch (state) {
	case WAIT_PRESSED:
		if ( gpioRead(PIN_SW3) == SW_ACTIVE ){state = WAIT_RELEASED;}
		break;//!!!!
	case WAIT_RELEASED:
		if ( gpioRead(PIN_SW3) == !SW_ACTIVE ){
			gpioToggle(PIN_LED_GREEN);
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



/*******************************************************************************
 ******************************************************************************/
