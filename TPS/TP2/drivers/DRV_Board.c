/***************************************************************************//**
  @file     DRV_Board.c
  @brief    FRDM Board management
  @author   Grupo 1
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "DRV_Board.h"
#include "PDRV_GPIO.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/***** BOARD defines **********************************************************/

// On Board User LEDs
static const pin_t PIN_LEDS[] = {	PORTNUM2PIN(PB,22), // PTB22
									PORTNUM2PIN(PE,26), // PTE26
									PORTNUM2PIN(PB,21) 	// PTB21
									};

#define LED_ACTIVE      LOW


// On Board User Switches
static const pin_t PIN_SWS[] = {PORTNUM2PIN(PC,6),	// PTC6
								PORTNUM2PIN(PA,4)	// PTA4
								};

#define SW_ACTIVE       LOW
//#define SW_INPUT_TYPE   INPUT	//Los switch ya tienen un pullup externo


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

void initBoard(){
	gpioMode(PIN_LEDS[RED], OUTPUT);
	gpioMode(PIN_LEDS[GREEN], OUTPUT);
	gpioMode(PIN_LEDS[BLUE], OUTPUT);

	gpioMode(PIN_SWS[SW2], INPUT_PULLUP);
	gpioMode(PIN_SWS[SW3], INPUT);

	clearLED(RED);
	clearLED(GREEN);
	clearLED(BLUE);

#ifdef GPIO_DEVELOPMENT_MODE
	gpioMode(GPIO_IRQ_TEST_PIN, OUTPUT);
#endif //GPIO_DEVELOPMENT_MODE
}

void setLED(LED_t led, bool value){
	gpioWrite(PIN_LEDS[led], (value)? LED_ACTIVE : !LED_ACTIVE);
}

void clearLED(LED_t led){
	gpioWrite(PIN_LEDS[led], !LED_ACTIVE);
}

void toggleLED(LED_t led){
	gpioToggle(PIN_LEDS[led]);
}

bool getSW(SW_t sw){
	return gpioRead(PIN_SWS[sw]) == SW_ACTIVE;
}

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/



/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/


/******************************************************************************/
