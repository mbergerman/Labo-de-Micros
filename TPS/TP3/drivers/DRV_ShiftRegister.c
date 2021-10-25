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
	SPI_config_t config;
	config.mode = MASTER;
	config.chip_selects = 1;
	config.frame_size = 8;
	config.clock_polarity = false;
	config.clock_phase = false;
	config.LSB_first = false;
	config.CS_active_low = true;

	SPI_Init(0, config);

	SR_timer_id = timerGetId();

	SPI_Write(0, 0, 0);
	SPI_Write(0, 0, 0);
	SPI_Write(0, 0, 0);
	SPI_Write(0, 0, 0);
	SPI_Write(0, 0, 0);
	timerStart(SR_timer_id, TIMER_MS2TICKS(REFRESH_RATE), TIM_MODE_PERIODIC, shift_register_ISR);

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
		SPI_Write(0, 0, 0);
	}
}


/******************************************************************************/
