/***************************************************************************//**
  @file     DRV_ShiftRegister.h
  @brief    Shift register driver.
  @author   Grupo 1
 ******************************************************************************/

#ifndef _SHIFTREGISTER_H_
#define _SHIFTREGISTER_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define	SR_SERIAL_SHIFT_PIN	PORTNUM2PIN(PC, 12)
#define	SR_LATCH_PIN		PORTNUM2PIN(PC, 4)

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

//Conectar OE y PARALLEL LOAD al CHIP SELECT (PTD0)
//Conectar SERIAL IN a MOSI (PTD2) y SERIAL OUT a MISO (PTD3)
//Conectar LATCH CLOCK y SHIFT CLOCK a SCK (PTD1)

void ShiftRegister_init();

uint8_t ShiftRegister_read();

/*******************************************************************************
 ******************************************************************************/

#endif // _SHIFTREGISTER_H_
