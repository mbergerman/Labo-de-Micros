/***************************************************************************//**
  @file     DRV_Accelerometer.c
  @brief    +Descripcion del archivo+
  @author   matia
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "DRV_Accelerometer.h"
#include "PDRV_I2C.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

// FXOS8700CQ I2C address
#define FXOS8700CQ_SLAVE_ADDR	0x1D // with pins SA0=1, SA1=0

// FXOS8700CQ internal register addresses
#define FXOS8700CQ_STATUS		0x00
#define FXOS8700CQ_WHOAMI		0x0D
#define FXOS8700CQ_XYZ_DATA_CFG 0x0E
#define FXOS8700CQ_CTRL_REG1	0x2A
#define FXOS8700CQ_M_CTRL_REG1 	0x5B
#define FXOS8700CQ_M_CTRL_REG2 	0x5C
#define FXOS8700CQ_WHOAMI_VAL 	0xC7

#define FXOS8700CQ_OUT_X_MSB	0x01
#define FXOS8700CQ_OUT_Y_MSB	0x03
#define FXOS8700CQ_OUT_Z_MSB	0x05

#define UINT14_MAX				16383


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/



/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

// +ej: unsigned int anio_actual;+


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

// +ej: static void falta_envido (int);+


/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

// +ej: static const int temperaturas_medias[4] = {23, 26, 24, 29};+


/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void initAccelerometer(){
   	initI2C(0, I2C_SPEED_32522HZ);
}

void accelConfigInit(){
   	uint16_t seq_standby[] = {I2C_ADDR_W(FXOS8700CQ_SLAVE_ADDR), FXOS8700CQ_CTRL_REG1, 0x00};
	i2cSendSequence(0, seq_standby, 3, 0, 0, 0);

	//Aca faltaria algun tipo de delay o callback


	// write 0000 0001= 0x01 to XYZ_DATA_CFG register
	// [7]: reserved
	// [6]: reserved
	// [5]: reserved
	// [4]: hpf_out=0
	// [3]: reserved
	// [2]: reserved
	// [1-0]: fs=01 for accelerometer range of +/-4g range with 0.488mg/LSB
   	uint16_t seq_xyz_config[] = {I2C_ADDR_W(FXOS8700CQ_SLAVE_ADDR), FXOS8700CQ_XYZ_DATA_CFG, 0x01};
	i2cSendSequence(0, seq_xyz_config, 3, 0, 0, 0);

	// write 0000 1101 = 0x1D to accelerometer control register 1
	// [7-6]: aslp_rate=00
	// [5-3]: dr=011 for 100Hz data rate (when not in hybrid mode)
	// [2]: lnoise=1 for low noise mode
	// [1]: f_read=0 for normal 16 bit reads
	// [0]: active=1 to take the part out of standby and enable	sampling
	uint16_t seq_odr_config[] = {I2C_ADDR_W(FXOS8700CQ_SLAVE_ADDR), FXOS8700CQ_CTRL_REG1, 0x1D};
	i2cSendSequence(0, seq_odr_config, 3, 0, 0, 0);
}

uint8_t accelNewData(void) {
    uint8_t stat = 0;
    uint16_t seq_status[] = {I2C_ADDR_W(FXOS8700CQ_SLAVE_ADDR), FXOS8700CQ_STATUS, I2C_RESTART, I2C_ADDR_R(FXOS8700CQ_SLAVE_ADDR), I2C_READ};

    i2cSendSequence(0, seq_status, 5, &stat, 0, 0);
    return stat;
}

void AccelReadDataX(uint8_t* result)
{
	uint16_t seq_getx[] = {I2C_ADDR_W(FXOS8700CQ_SLAVE_ADDR), FXOS8700CQ_OUT_X_MSB, I2C_RESTART, I2C_ADDR_R(FXOS8700CQ_SLAVE_ADDR), I2C_READ, I2C_READ};

    i2cSendSequence(0, seq_getx, 6, result, 0, 0);
}

uint16_t accelProcessResult(uint8_t* result){

	int16_t acc;
	acc = (result[0] << 6) | (result[1] >> 2);
	if (acc > UINT14_MAX/2)
		acc -= UINT14_MAX;

	return acc;
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
 
