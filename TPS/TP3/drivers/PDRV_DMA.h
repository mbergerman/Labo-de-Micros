/***************************************************************************//**
  @file     PDRV_DMA.h
  @brief    DMA driver.
  @author   Grupo 1
 ******************************************************************************/

#ifndef _DMA_H_
#define _DMA_H_

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

typedef enum { FTM0CH0 = 20, FTM0CH1 = 21, FTM0CH2 = 22, FTM0CH3 = 23,
			   FTM0CH4 = 24, FTM0CH5 = 25, FTM0CH6 = 26, FTM0CH7 = 27 } DMA_request_t;

typedef struct DMA_config_t
{
	void * source_buffer;
	void * destination_buffer;
	DMA_request_t request_source;
	uint8_t source_offset;
	uint8_t destination_offset;
	uint8_t transfer_bytes;
	uint16_t major_cycles;
	uint32_t wrap_around;
}DMA_config_t;
/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/



/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

void DMA_init(uint8_t DMA_channel, DMA_config_t config);

/*******************************************************************************
 ******************************************************************************/

#endif // _DMA_H_
