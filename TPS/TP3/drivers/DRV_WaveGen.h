/***************************************************************************//**
  @file     DRV_WaveGen.h
  @brief    +Descripcion del archivo+
  @author   matia
 ******************************************************************************/

#ifndef DRV_WAVEGEN_H_
#define DRV_WAVEGEN_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "Util.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/



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

void initWaveGen();
void wavegenSetFreq(uint32_t);
void wavegenSetAmp(uint8_t);


/*******************************************************************************
 ******************************************************************************/

#endif /* DRV_WAVEGEN_H_ */