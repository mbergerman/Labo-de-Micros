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

typedef enum {FTM_Prescale_1 = 0b000,
			  FTM_Prescale_2 = 0b001,
			  FTM_Prescale_4 = 0b010,
			  FTM_Prescale_8 = 0b011,
			  FTM_Prescale_16 = 0b100,
			  FTM_Prescale_32 = 0b101,
			  FTM_Prescale_64 = 0b110,
			  FTM_Prescale_128 = 0b111}FTM_Prescale_t;

typedef enum {FTM_NoCLK, FTM_SysCLK, FTM_FixedFreqCLK, FTM_ExtCLK}FTM_CLKSource_t;

typedef enum {FTM_InputCaputre, FTM_OutputCompare, FTM_PWM}FTM_Mode_t;

typedef enum {FTM_Channel_0, FTM_Channel_1, FTM_Channel_2, FTM_Channel_3, FTM_Channel_4,
			  FTM_Channel_5, FTM_Channel_6, FTM_Channel_7, FTM_Channel_8}FTM_Channel_t;

typedef enum{}FTM_Edge_t;

typedef struct FTM_Config_t
{
	FTM_Mode_t mode;
	FTM_Prescale_t prescale;
	FTM_CLKSource_t CLK_source;
	uint16_t init_counter_value;
}FTM_Config_t;

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/



/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

void FTM_init(uint8_t FTM, FTM_Channel_t channel, FTM_Config_t config);

/*******************************************************************************
 ******************************************************************************/

#endif // _FTM_H_
