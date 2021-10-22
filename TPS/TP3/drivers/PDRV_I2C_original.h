/***************************************************************************//**
  @file     PDRV_I2C.h
  @brief    +Descripcion del archivo+
  @author   matia
 ******************************************************************************/

#ifndef PDRV_I2C_ORIGINAL_H_
#define PDRV_I2C_ORIGINAL_H_

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
	I2C_SPEED_156250HZ = 0x14,
	I2C_SPEED_97656HZ = 0x17,
	I2C_SPEED_78125HZ = 0x1D,
	I2C_SPEED_65104HZ = 0x1E,
	I2C_SPEED_48828HZ = 0x23,
	I2C_SPEED_32522HZ = 0x26,
	I2C_SPEED_24414HZ = 0x2B,
	I2C_SPEED_16276HZ = 0x2E,
	I2C_SPEED_12207HZ = 0x33,
	I2C_SPEED_8138HZ = 0x39,
	I2C_SPEED_4069HZ = 0x3E,
	I2C_SPEED_3255HZ = 0x3F
}i2c_speed_t;


typedef enum{
	I2C_MODE_TX = 0x00,
	I2C_MODE_RX = 0x01
}i2c_mode_t;

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
void i2cSendMessage(uint8_t id, i2c_mode_t mode, uint8_t address, const char* msg, uint8_t cant);


/*******************************************************************************
 ******************************************************************************/

#endif /* PDRV_I2C_ORIGINAL_H_ */
