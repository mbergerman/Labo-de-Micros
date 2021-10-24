/***************************************************************************//**
  @file     DRV_Accelerometer.h
  @brief    +Descripcion del archivo+
  @author   matia
 ******************************************************************************/

#ifndef DRV_ACCELEROMETER_H_
#define DRV_ACCELEROMETER_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "Util.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define ACC_RANGE_LSB	0.488	// mg/LSB

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/


/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

// +ej: extern unsigned int anio_actual;+


/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

void initAccelerometer();
bool accelConfigInit();
bool accelNewData(uint8_t*);
bool accelReadDataX(uint8_t* result);
bool accelReadDataY(uint8_t* result);
int16_t accelProcessResult(uint8_t* result);
bool accelReadingReady();

/*******************************************************************************
 ******************************************************************************/

#endif /* DRV_ACCELEROMETER_H_ */
