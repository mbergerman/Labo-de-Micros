/***************************************************************************//**
  @file     DRV_ShiftRegister.c
  @brief    Shift register driver.
  @author   Grupo 1
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "PDRV_SPI.h"
#include "DRV_Timers.h"
#include "DRV_ShiftRegister.h"
#include "PDRV_GPIO.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define REFRESH_RATE 3

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

void shift_register_ISR();

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static uint32_t SR_data;

static tim_id_t SR_timer_id;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void ShiftRegister_init()
{
	initTimers();

	SPI_config_t config;
	config.mode = MASTER;
	config.chip_selects = 1;
	config.frame_size = 8;
	config.clock_polarity = false;
	config.clock_phase = false;
	config.LSB_first = false;
	config.CS_active_low = false;

	SPI_Init(0, config);

	SR_timer_id = timerGetId();

	SPI_Write(0, 0, 0);
	SPI_Write(0, 0, 0);
	SPI_Write(0, 0, 0);
	SPI_Write(0, 0, 0);
	SPI_Write(0, 0, 0);
	timerStart(SR_timer_id, TIMER_MS2TICKS(REFRESH_RATE), TIM_MODE_PERIODIC, shift_register_ISR);

	gpioMode(SR_SERIAL_SHIFT_PIN, OUTPUT);
	gpioMode(SR_LATCH_PIN, OUTPUT);
}

uint8_t ShiftRegister_read()
{
	return (uint8_t) SR_data;
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void shift_register_ISR()
{
	if (SPI_TransferComplete(0))
	{
		SR_data = SPI_Read(0);

		gpioWrite(SR_SERIAL_SHIFT_PIN, LOW);
		gpioWrite(SR_LATCH_PIN, HIGH);
		gpioWrite(SR_LATCH_PIN, LOW);
		gpioWrite(SR_SERIAL_SHIFT_PIN, HIGH);

		SPI_Write(0, 0, 0);
	}
}


/******************************************************************************/
