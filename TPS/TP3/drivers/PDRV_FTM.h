/***************************************************************************//**
  @file     PDRV_FTM.h
  @brief    FTM driver.
  @author   Grupo 1
 ******************************************************************************/

#ifndef _FTM_H_
#define _FTM_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef enum {FTM_Prescale_1   = 0b000,
			  FTM_Prescale_2   = 0b001,
			  FTM_Prescale_4   = 0b010,
			  FTM_Prescale_8   = 0b011,
			  FTM_Prescale_16  = 0b100,
			  FTM_Prescale_32  = 0b101,
			  FTM_Prescale_64  = 0b110,
			  FTM_Prescale_128 = 0b111}FTM_Prescale_t;

typedef enum {FTM_NoCLK, FTM_SysCLK, FTM_FixedFreqCLK, FTM_ExtCLK}FTM_CLKSource_t;

typedef enum {FTM_InputCapture, FTM_OutputCompare, FTM_PWM}FTM_Mode_t;

typedef enum {FTM_Channel_0, FTM_Channel_1, FTM_Channel_2, FTM_Channel_3,
			  FTM_Channel_4, FTM_Channel_5, FTM_Channel_6, FTM_Channel_7}FTM_Channel_t;

typedef enum {FTM_PositiveEdge = 0b01, FTM_NegativeEdge = 0b10, FTM_EitherEdge = 0b11} FTM_Edge_t;

typedef enum {FTM_High = 0b10, FTM_Low = 0b11} FTM_Logic_t;

typedef enum {FTM_Clear = 0b01, FTM_Toggle = 0b10, FTM_Set = 0b11} FTM_Effect_t;

typedef struct FTM_Config_t
{
	FTM_Channel_t channel;
	FTM_Mode_t mode;
	FTM_Prescale_t prescale;
	FTM_CLKSource_t CLK_source;
	uint8_t ext_clock;
	union
	{
		FTM_Edge_t IC_edge;
		FTM_Effect_t OC_effect;
		FTM_Logic_t PWM_logic;
	};
	uint16_t modulo;
	union
	{
		uint16_t counter;
		uint16_t PWM_DC;
	};
	bool active_low;
	bool DMA_on;
	bool interrupt_on;
}FTM_Config_t;

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/



/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

void FTM_init(uint8_t FTM, FTM_Config_t config);
void FTM_start(uint8_t FTM);
void FTM_stop(uint8_t FTM);
void FTM_resetCounter(uint8_t FTM);
uint16_t FTM_getCounter(uint8_t FTM);
void FTM_modifyDC(uint8_t FTM, uint16_t DC);
void FTM_modifyModulo(uint8_t FTM, uint16_t modulo);

void * FTM_getCounterPointer(uint8_t FTM, FTM_Channel_t channel);

/*******************************************************************************
 ******************************************************************************/

#endif // _FTM_H_
