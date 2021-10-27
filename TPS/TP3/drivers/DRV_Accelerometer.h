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

#include <Util.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define ACC_RANGE_LSB	0.488	// mg/LSB

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef struct {
	int16_t x;
	int16_t y;
} acc_t;

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
bool accelReadDataX(void);
bool accelReadDataY(void);
int16_t accelProcessResult(uint8_t* result);
bool accelReadingReady();

acc_t getAccelerationVector();
acc_t readAcceleration(void);


/*******************************************************************************
 ******************************************************************************/

#endif /* DRV_ACCELEROMETER_H_ */
