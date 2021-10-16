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

enum {SPI0, SPI1, SPI2};

typedef enum {SLAVE, MASTER} SPI_mode_t;

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/



/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief SPI_init: Initialize an SPI port
 * @param spi_port - SPI port to initialize (SPI0, SPI1, SPI2)
 * @param mode - SPI port mode - SLAVE or MASTER
 * @param chip_selects - Number of peripheral chip selects used (SPI0: 1-6, SPI1: 1-4, SPI2: 1) - Only relevant in MASTER mode
 * @param PCSIS - SPI Port Peripheral Chip Select Inactive State - Defines inactive state as High or Low (e.g. PCSIS = HIGH => Chip Select works as Active Low)
*/
void SPI_init(uint8_t spi_port, SPI_mode_t mode, uint8_t chip_selects, bool PCSIS);

/*******************************************************************************
 ******************************************************************************/

#endif // _SPI_H_
