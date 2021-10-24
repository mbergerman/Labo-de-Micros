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
#include "DRV_Timers.h"

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

#define BUSY_WAIT_TICKS			TIMER_MS2TICKS(2)


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

static void done_waiting();
static void reading_is_ready();

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

// +ej: static const int temperaturas_medias[4] = {23, 26, 24, 29};+

static const uint16_t seq_standby[] = {I2C_ADDR_W(FXOS8700CQ_SLAVE_ADDR), FXOS8700CQ_CTRL_REG1, 0x00};
static const uint16_t seq_xyz_config[] = {I2C_ADDR_W(FXOS8700CQ_SLAVE_ADDR), FXOS8700CQ_XYZ_DATA_CFG, 0x01};
static const uint16_t seq_odr_config[] = {I2C_ADDR_W(FXOS8700CQ_SLAVE_ADDR), FXOS8700CQ_CTRL_REG1, 0x1D};
static const uint16_t seq_status[] = {I2C_ADDR_W(FXOS8700CQ_SLAVE_ADDR), FXOS8700CQ_STATUS, I2C_RESTART, I2C_ADDR_R(FXOS8700CQ_SLAVE_ADDR), I2C_READ};

static const uint16_t seq_getx[] = {I2C_ADDR_W(FXOS8700CQ_SLAVE_ADDR), FXOS8700CQ_OUT_X_MSB, I2C_RESTART, I2C_ADDR_R(FXOS8700CQ_SLAVE_ADDR), I2C_READ, I2C_READ};



/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static bool accel_configured;
static uint8_t config_state;

static tim_id_t wait_tim_id;
static bool busy_waiting;
static bool reading_ready_flag;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void initAccelerometer(){
	static bool alreadyInit = false;
	if (alreadyInit)
		return;

   	initI2C(0, I2C_SPEED_97656HZ);
   	config_state = 0;
   	accel_configured = false;
   	wait_tim_id = timerGetId();
   	busy_waiting = false;

	alreadyInit = true;
}

bool accelConfigInit(){
	if(i2cGetStatus(0) == I2C_AVAILABLE && !busy_waiting){
		switch(config_state){
		case 0:
			accel_configured = false;
			config_state = 1;
			i2cSendSequence(0, (uint16_t*)seq_standby, 3, 0, 0, 0);

			busy_waiting = true;
			timerStart(wait_tim_id, BUSY_WAIT_TICKS, TIM_MODE_SINGLESHOT, done_waiting);
			break;
		case 1:
			// write 0000 0001= 0x01 to XYZ_DATA_CFG register
			// [7]: reserved
			// [6]: reserved
			// [5]: reserved
			// [4]: hpf_out=0
			// [3]: reserved
			// [2]: reserved
			// [1-0]: fs=01 for accelerometer range of +/-4g range with 0.488mg/LSB
			config_state = 2;
			i2cSendSequence(0, (uint16_t*)seq_xyz_config, 3, 0, 0, 0);

			busy_waiting = true;
			timerStart(wait_tim_id, BUSY_WAIT_TICKS, TIM_MODE_SINGLESHOT, done_waiting);
			break;
		case 2:
			// write 0000 1101 = 0x1D to accelerometer control register 1
			// [7-6]: aslp_rate=00
			// [5-3]: dr=011 for 100Hz data rate (when not in hybrid mode)
			// [2]: lnoise=1 for low noise mode
			// [1]: f_read=0 for normal 16 bit reads
			// [0]: active=1 to take the part out of standby and enable	sampling
			config_state = 3;
			i2cSendSequence(0, (uint16_t*)seq_odr_config, 3, 0, 0, 0);

			busy_waiting = true;
			timerStart(wait_tim_id, BUSY_WAIT_TICKS, TIM_MODE_SINGLESHOT, done_waiting);
			break;
		case 3:
			accel_configured = true;
			break;
		}
	}
	return accel_configured;
}

bool accelNewData(uint8_t* result){
	if(i2cGetStatus(0) == I2C_AVAILABLE && !busy_waiting){
		reading_ready_flag = false;
		i2cSendSequence(0, (uint16_t*)seq_status, 5, result, 0, 0);

		busy_waiting = true;
		timerStart(wait_tim_id, BUSY_WAIT_TICKS, TIM_MODE_SINGLESHOT, done_waiting);

		return true;
	}else{
		return false;
	}
}

bool accelReadDataX(uint8_t* result){
	if(i2cGetStatus(0) == I2C_AVAILABLE && !busy_waiting){
		reading_ready_flag = false;
		i2cSendSequence(0, (uint16_t*)seq_getx, 6, result, reading_is_ready, 0);

		busy_waiting = true;
		timerStart(wait_tim_id, BUSY_WAIT_TICKS, TIM_MODE_SINGLESHOT, done_waiting);

		return true;
	}else{
		return false;
	}
}

uint16_t accelProcessResult(uint8_t* result){

	int16_t acc;
	acc = (result[0] << 6) | (result[1] >> 2);
	if (acc > UINT14_MAX/2)
		acc -= UINT14_MAX;

	return acc;
}

bool accelReadingReady(){
	return reading_ready_flag;
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void done_waiting(){
	busy_waiting = false;
}

void reading_is_ready(){
	reading_ready_flag = true;
}
