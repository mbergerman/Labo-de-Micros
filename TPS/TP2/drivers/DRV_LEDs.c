/***************************************************************************//**
  @file     DRV_LEDs.c
  @brief    LEDs driver
  @author   LM1
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "DRV_LEDs.h"
#include "DRV_Timers.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

// GPIOs
#define STATUS0    PORTNUM2PIN(PC,2)
#define STATUS1    PORTNUM2PIN(PC,3)

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
typedef struct selector_data_t
{
	bool select0;
	bool select1;
}selector_data_t;

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static bool leds[DISP_LEDS_NUM];

static int LEDCounter = 0;

static int my_refresh_rate = 2;

static tim_id_t rate_id;

static selector_data_t leds_selector[] = {{HIGH, LOW}, {LOW, HIGH}, {HIGH, HIGH}, {LOW, LOW}};

// update_leds - update LEDs.
static void update_leds();


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

// initDisplay() - Initialize display.
void initLEDs()
{
	gpioMode(STATUS0, OUTPUT);
	gpioMode(STATUS1, OUTPUT);

	int i;
	for(i=0; i<DISP_LEDS_NUM;i++)
	{
		dispClearLED(i);
	}

	rate_id = timerGetId();
	timerStart(rate_id, TIMER_MS2TICKS(my_refresh_rate), TIM_MODE_PERIODIC, &update_leds);
}


// clearLED(int n) - Sets LED n to low.
void dispClearLED(int n)
{
	leds[n] = false;
}

// setLED(int n) - Sets LED n to high.
void dispSetLED(int n)
{
	leds[n] = true;
}

// toggleLED(int n) - Toggles LED n.
void dispToggleLED(int n)
{
	leds[n] = !leds[n];
}



/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void update_leds()
{
	// Update LEDS
	if(LEDCounter == DISP_LEDS_NUM)
	{
		LEDCounter = 0;
	}
	if(leds[LEDCounter])
	{
		gpioWrite (STATUS0, leds_selector[LEDCounter].select0);
		gpioWrite (STATUS1, leds_selector[LEDCounter].select1);
	}else{
		gpioWrite (STATUS0, leds_selector[DISP_LEDS_NUM].select0);
		gpioWrite (STATUS1, leds_selector[DISP_LEDS_NUM].select1);
	}
	LEDCounter++;
}

