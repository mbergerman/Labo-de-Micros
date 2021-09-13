/***************************************************************************//**
  @file     PDRV_SysTick.c
  @brief    SysTick driver
  @author   Grupo 1
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "PDRV_SysTick.h"
#include "MK64F12.h"
#include "hardware.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
// clock frequency
#define FCLK __CORE_CLOCK__

// control and status register
#define CSR_MASK 0x00010007
#define ENABLE_MASK 0x01
#define TICKINT_MASK 0x02
#define CLKSOURCE_MASK 0x04
#define COUNTFLAG_MASK 0x10000

// reload value register
#define RELOAD_VALUE (FCLK/SYSTICK_ISR_FREQUENCY_HZ - 1)
#define RVR_MASK 0x00FFFFFF

// current value register
#define CLEAR_VALUE 0x0
#define CVR_MASK 0x00FFFFFF

// calibration value register
#define CALIB_MASK 0xC0FFFFFF

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
// See Section 4.4 ARM-Cortex-M4 Generic User Guide

//typedef struct {
//	__IO uint32_t CSR;
//	__IO uint32_t RVR;
//	__IO uint32_t CVR;
//	__I uint32_t CALIB;
//} SysTick_t;
//#define SYSTICK ((SysTick_t*)0xE000E010)

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
static void (*funcallback_ptr)(void);

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

bool SysTick_Init (void (*funcallback)(void)) {
	SysTick->CTRL = 0x00;
	// 1) set reload value
	SysTick->LOAD = RVR_MASK & RELOAD_VALUE;

	// 2) clear current value
	SysTick->VAL = CVR_MASK & CLEAR_VALUE;

	// 3) enable SysTick
	SysTick->CTRL = CSR_MASK & (ENABLE_MASK | TICKINT_MASK | CLKSOURCE_MASK);

	funcallback_ptr = funcallback;
	return 0;
}

__ISR__ SysTick_Handler(void) {
	(*funcallback_ptr)();
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/


