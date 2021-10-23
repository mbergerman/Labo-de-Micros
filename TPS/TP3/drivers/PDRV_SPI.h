/***************************************************************************//**
  @file     PDRV_SPI.h
  @brief    SPI driver.
  @author   Grupo 1
 ******************************************************************************/

#ifndef _SPI_H_
#define _SPI_H_

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

typedef enum {SLAVE, MASTER} SPI_mode_t;

/**
 * @brief SPI_config_t - SPI configuration parameters data type
 * @param mode - SPI port mode, either SLAVE or MASTER (check SPI_mode_t)
 * @param chip_selects - number of peripheral chip selects used (SPI0: 1-6, SPI1: 1-4, SPI2: 1)
 * @param frame_size - number of bits in a frame from 4 to 16
 * @param clock_polarity - SCK Active High or Active low (e.g. clock_polarity = HIGH, then SCK is Active Low)
 * @param clock_phase - Defines when data is captured and changed (e.g clock_phase = 0 - data is captured in leading edge and changed in the following edge)
 * @param LSB_first - Defines if data is transfered as LSB first or MSB first (e.g LSB_first = TRUE, then data is transfered LSB first)
 * @param PCSIS (only used in MASTER mode) - Peripheral Chip Select Inactive State - defines Chip select as either Active High or Active Low (e.g. PCSIS = HIGH, then chip select is Active Low)
*/

typedef struct SPI_config_t
{
	SPI_mode_t mode;
	uint8_t chip_selects;
	uint8_t frame_size;
	bool clock_polarity;
	bool clock_phase;
	bool LSB_first;
	bool CS_active_low;
}SPI_config_t;

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/



/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief SPI_init: Initialize an SPI port
 * @param spi_port - SPI port to initialize (SPI0 = 0, SPI1 = 1, SPI2 = 2)
 * @param config - User defined configuration of the SPI port (see SPI_config_t data type)
*/
void SPI_Init(uint8_t spi_port, SPI_config_t config);

uint16_t SPI_Read(uint8_t spi_port);

void SPI_Write(uint8_t spi_port, uint16_t spi_data, uint8_t chip_select);

bool SPI_TransferComplete(uint8_t spi_port);

/*******************************************************************************
 ******************************************************************************/

#endif // _SPI_H_
