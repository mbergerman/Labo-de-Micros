/***************************************************************************//**
  @file     DRV_Reader.h
  @brief    Card Reader driver
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

#define TERMINATOR_CHAR		'\0'
#define READER_TIMEOUT		500	 // Milisegundos a esperar antes de cancelar una lectura
#define READER_NUM_MAX_LEN	19

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

/**
 * @brief Initialize Card Reader
 */
void initReader(void);

/**
 * @brief Enable card Reader interruptions
 */
void enableReader(void);

/**
 * @brief Disable Card Reader interruptions
 */
void disableReader(void);

/**
 * @brief Check if card has been correctly swiped
 * @return reading ready
 */
bool readerIsReady();

/**
 * @brief Get card number value
 * @param result_number_ptr Pointer to result array
 * @param result_len_ptr Pointer to result length variable
 */
void getValueReader(char* result_number_ptr, uint8_t* result_len_ptr);


/*******************************************************************************
 ******************************************************************************/


#endif /* DRV_READER_H_ */
