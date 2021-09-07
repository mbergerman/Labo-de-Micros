/***************************************************************************//**
  @file     timer.c
  @brief    Timer driver. Advance implementation
  @author   Nicolás Magliola
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "DRV_Timers.h"
#include "PDRV_SysTick.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#if TIMER_TICK_MS != (1000U/SYSTICK_ISR_FREQUENCY_HZ)
#error Las frecuencias no coinciden!!
#endif // TIMER_TICK_MS != (1000U/SYSTICK_ISR_FREQUENCY_HZ)

#define TIMER_DEVELOPMENT_MODE    1

#define TIMER_ID_INTERNAL   0


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef struct {
	ttick_t             period;
	ttick_t             cnt;
    tim_callback_t      callback;
    uint8_t             mode        : 1;
    uint8_t             running     : 1;
    uint8_t             expired     : 1;
    uint8_t             unused      : 5;
} timer_t;


/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

/**
 * @brief Periodic service
 */
static void timer_isr(void);


/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static timer_t timers[TIMERS_MAX_CANT];
static tim_id_t timers_cant = TIMER_ID_INTERNAL+1;


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void initTimers(void)
{
    static bool alreadyInit = false;
    if (alreadyInit)
        return;
    
    SysTick_Init(timer_isr); // init peripheral
    
    alreadyInit = true;
}


tim_id_t timerGetId(void)
{
#ifdef TIMER_DEVELOPMENT_MODE
    if (timers_cant >= TIMERS_MAX_CANT)
    {
        return TIMER_INVALID_ID;
    }
    else
#endif // TIMER_DEVELOPMENT_MODE
    {
        return timers_cant++;
    }
}


void timerStart(tim_id_t id, ttick_t ticks, uint8_t mode, tim_callback_t callback)
{
#ifdef TIMER_DEVELOPMENT_MODE
    if ((id < timers_cant) && (mode < CANT_TIM_MODES))
#endif // TIMER_DEVELOPMENT_MODE
    {
        // disable timer
    	timers[id].running = false;

        // configure timer
		timers[id].period = ticks;
		timers[id].cnt = ticks;
		timers[id].mode = mode;
		timers[id].expired = false;
		timers[id].callback = callback;

        // enable timer
		timers[id].running = true;
    }
}


void timerStop(tim_id_t id)
{
    // Apago el timer
	timers[id].running = false;

    // y bajo el flag
	timers[id].expired = false;
}


bool timerExpired(tim_id_t id)
{
	return timers[id].expired;
}


void timerDelay(ttick_t ticks)
{
    timerStart(TIMER_ID_INTERNAL, ticks, TIM_MODE_SINGLESHOT, NULL);
    while ( !timerExpired(TIMER_ID_INTERNAL) ){
        //wait
    }
    timerStop(TIMER_ID_INTERNAL);
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void timer_isr(void)
{
    // decremento los timers activos
	for(tim_id_t id = 0; id < TIMERS_MAX_CANT; id++){

		// si hubo timeout!
		if(timers[id].running){
			if(--timers[id].cnt == 0){
				// 1) execute action: callback or set flag
				timers[id].expired = true;

				if(timers[id].callback != NULL) {
					(*timers[id].callback)();
				}

				// 2) update state
				timers[id].cnt = timers[id].period;
				timers[id].expired = (timers[id].mode == TIM_MODE_PERIODIC) ? false : timers[id].expired;
			}
		}
	}
}


/******************************************************************************/
