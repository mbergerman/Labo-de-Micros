/***************************************************************************//**
  @file     PDRV_I2C.h
  @brief    +Descripcion del archivo+
  @author   matia
 ******************************************************************************/

#ifndef PDRV_I2C_H_
#define PDRV_I2C_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define	MSG_BUFFER_LEN	32

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef enum{
	I2C_SPEED_156250HZ = 0x15,
	I2C_SPEED_97656HZ = 0x18,
	I2C_SPEED_78125HZ = 0x1E,
	I2C_SPEED_65104HZ = 0x1F,
	I2C_SPEED_48828HZ = 0x24,
	I2C_SPEED_32522HZ = 0x27,
	I2C_SPEED_24414HZ = 0x2C,
	I2C_SPEED_16276HZ = 0x2F
}i2c_speed_t;

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

// +ej: extern unsigned int anio_actual;+


/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Initialize I2C driver
 * @param id I2C's number
 * @param config I2C's configuration (speed, etc.)
*/
void initI2C(uint8_t id, i2c_speed_t speed);

/**
 * @brief Write a message to be transmitted. Non-Blocking
 * @param id I2C's number
 * @param address I2C Address
 * @param msg Buffer with the bytes to be transfered
 * @param cant Desired quantity of bytes to be transfered
 * @return Real quantity of bytes to be transfered
*/
uint8_t i2cWriteMsg(uint8_t id, uint32_t address, const char* msg, uint8_t cant);


/*******************************************************************************
 ******************************************************************************/

#endif /* PDRV_I2C_H_ */
