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

typedef struct{
	i2c_speed_t speed;
}i2c_cfg_t;

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

// +ej: extern unsigned int anio_actual;+


/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief TODO: completar descripcion
 * @param param1 Descripcion parametro 1
 * @param param2 Descripcion parametro 2
 * @return Descripcion valor que devuelve
*/
// +ej: char lcd_goto (int fil, int col);+

void initI2C(uint8_t id, i2c_cfg_t config);

/*******************************************************************************
 ******************************************************************************/

#endif /* PDRV_I2C_H_ */
