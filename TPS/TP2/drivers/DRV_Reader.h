/***************************************************************************//**
  @file     DRV_Reader.h
  @brief    +Descripcion del archivo+
  @author   Grupo 1
 ******************************************************************************/

#ifndef DRV_READER_H_
#define DRV_READER_H_


/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "PDRV_GPIO.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define TERMINATOR_CHAR	'\0'
#define READER_TIMEOUT	500	 // Milisegundos a esperar antes de cancelar una lectura

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef	void (*readerCallback_t)(char*, uint8_t);

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/


/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

void initReader(void);
void stopReader(void);
//bool readerRunning(void);
bool readerIsReady(); // return and clear flag
void getValueReader(char* result_number_ptr, uint8_t* result_len_ptr);
//void clearFlagReady();


/*******************************************************************************
 ******************************************************************************/


#endif /* DRV_READER_H_ */
