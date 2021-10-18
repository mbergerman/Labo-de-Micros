/***************************************************************************//**
  @file     PDRV_I2C.c
  @brief    +Descripcion del archivo+
  @author   matia
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "PDRV_I2C.h"
#include "MK64F12.h"
#include "hardware.h"
#include "string.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define I2C0_SDA_PORT	4
#define I2C0_SCL_PORT	4
#define I2C1_SDA_PORT	2
#define I2C1_SCL_PORT	2
#define I2C2_SDA_PORT	0
#define I2C2_SCL_PORT	0

#define I2C0_SDA_PIN	((I2C0_SDA_PORT<<5) + 25)	// PTE25
#define I2C0_SCL_PIN	((I2C0_SCL_PORT<<5) + 24)	// PTE24
#define I2C1_SDA_PIN	((I2C1_SDA_PORT<<5) + 11)	// PTC11
#define I2C1_SCL_PIN	((I2C1_SCL_PORT<<5) + 10)	// PTC10
#define I2C2_SDA_PIN	((I2C2_SDA_PORT<<5) + 13)	// PTA13	No está disponible en la FRDM
#define I2C2_SCL_PIN	((I2C2_SCL_PORT<<5) + 14)	// PTA14	No está disponible en la FRDM

#define I2C0_SDA_ALT	5
#define I2C0_SCL_ALT	5
#define I2C1_SDA_ALT	2
#define I2C1_SCL_ALT	2
#define I2C2_SDA_ALT	5
#define I2C2_SCL_ALT	5


#define I2C_SDA_PORT(id)	((id == 0)?	I2C0_SDA_PORT: \
							((id == 1)?	I2C1_SDA_PORT: \
										I2C2_SDA_PORT))

#define I2C_SCL_PORT(id)	((id == 0)?	I2C0_SCL_PORT: \
							((id == 1)?	I2C1_SCL_PORT: \
										I2C2_SCL_PORT))

#define I2C_SDA_PIN(id)		((id == 0)?	I2C0_SDA_PIN: \
							((id == 1)?	I2C1_SDA_PIN: \
										I2C2_SDA_PIN))

#define I2C_SCL_PIN(id)		((id == 0)?	I2C0_SCL_PIN: \
							((id == 1)?	I2C1_SCL_PIN: \
										I2C2_SCL_PIN))

#define I2C_SDA_ALT(id)		((id == 0)?	I2C0_SDA_ALT: \
							((id == 1)?	I2C1_SDA_ALT: \
										I2C2_SDA_ALT))

#define I2C_SCL_ALT(id)		((id == 0)?	I2C0_SCL_ALT: \
							((id == 1)?	I2C1_SCL_ALT: \
										I2C2_SCL_ALT))

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

enum{
	TX,
	RX
};

/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

// +ej: unsigned int anio_actual;+


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void IRQHandler(uint8_t id);


/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

// +ej: static const int temperaturas_medias[4] = {23, 26, 24, 29};+

static I2C_Type* const I2C_PTRS[] = I2C_BASE_PTRS;
static PORT_Type* const PORT_PTRS[FSL_FEATURE_SOC_PORT_COUNT] = PORT_BASE_PTRS;

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static char msg_buffer[MSG_BUFFER_LEN];
static uint8_t msg_buffer_pos;
static uint8_t msg_buffer_len;
static uint8_t i2c_msg_mode = TX;
static uint8_t i2c_mode = TX;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void initI2C(uint8_t id, i2c_speed_t speed){

	// 1) Clock enable
	switch(id){
	case 0:
		SIM->SCGC4 |= SIM_SCGC4_I2C0(1);
		break;
	case 1:
		SIM->SCGC4 |= SIM_SCGC4_I2C1(1);
		break;
	case 2:
		SIM->SCGC1 |= SIM_SCGC1_I2C2(1);
		break;
	}

	// 2) Port configuration
	PORT_PTRS[I2C_SDA_PORT(id)]->PCR[I2C_SDA_PIN(id)] = (uint32_t) 0x0;
	PORT_PTRS[I2C_SCL_PORT(id)]->PCR[I2C_SCL_PIN(id)] = (uint32_t) 0x0;
	PORT_PTRS[I2C_SDA_PORT(id)]->PCR[I2C_SDA_PIN(id)] |= PORT_PCR_MUX(I2C_SDA_ALT(id));
	PORT_PTRS[I2C_SCL_PORT(id)]->PCR[I2C_SCL_PIN(id)] |= PORT_PCR_MUX(I2C_SCL_ALT(id));
	PORT_PTRS[I2C_SDA_PORT(id)]->PCR[I2C_SDA_PIN(id)] |= PORT_PCR_ODE(1);	// Enable open drain
	PORT_PTRS[I2C_SCL_PORT(id)]->PCR[I2C_SCL_PIN(id)] |= PORT_PCR_ODE(1);	// Enable open drain


	I2C_PTRS[id]->F = (uint32_t) 0x0;	// Clear

	// I2C baud rate = I2C module clock speed (Hz)/(mul × SCL divider)
	I2C_PTRS[id]->F |= I2C_F_MULT(0b10);		// mul = 4
	I2C_PTRS[id]->F |= I2C_F_ICR(speed);
	I2C_PTRS[id]->C1 |= I2C_C1_IICEN(1) | I2C_C1_IICIE(1);	// I2C Enable, I2C Interrupt Enable
	I2C_PTRS[id]->C1 |= I2C_C1_TX(1);	// Transmit mode select: Transmit
}



uint8_t i2cWriteMsg(uint8_t id, uint8_t address, const char* msg, uint8_t cant){
	I2C_PTRS[id]->C1 |= I2C_C1_MST(1);	// Master mode select: Master mode, Esto genera el START
	I2C_PTRS[id]->D  = address << 1 & (~0x01);
	i2c_mode = TX;
	i2c_msg_mode = TX;
	strcpy(msg_buffer, msg);
	msg_buffer_pos = 0;
	msg_buffer_len = cant;
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void IRQHandler(uint8_t id) {
	if(I2C_PTRS[id]->FLT | I2C_FLT_STOPF_MASK){
		I2C_PTRS[id]->FLT |= I2C_FLT_STOPF(1); 		// Clear STOPFF: w1c
		I2C_PTRS[id]->S |= I2C_S_IICIF(1); 			// Clear IICIF: w1c
		// Zero start count
	}else{
		if(I2C_PTRS[id]->FLT | I2C_FLT_STARTF_MASK){
			I2C_PTRS[id]->FLT |= I2C_FLT_STARTF(1); 	// Clear STARTF: w1c
			I2C_PTRS[id]->S |= I2C_S_IICIF(1); 			// Clear IICIF: w1c
			// Log start count ++
		}else{
			I2C_PTRS[id]->S |= I2C_S_IICIF(1); 			// Clear IICIF: w1c
			if(i2c_mode == TX){
				if(msg_buffer_len == msg_buffer_pos){ 	//Last byte transmitted?
					I2C_PTRS[id]->C1 &= ~I2C_C1_MST(1);	// Esto genera el STOP
				}else{
					if(I2C_PTRS[id]->S | I2C_S_RXAK_MASK){
						if(i2c_msg_mode == RX){
							i2c_mode = RX;
							uint8_t dummy_read = I2C_PTRS[id]->D;
						}else{
							I2C_PTRS[id]->D = msg_buffer[msg_buffer_pos];
							msg_buffer_pos += 1;
						}
					}else{
						I2C_PTRS[id]->C1 &= ~I2C_C1_MST(1);	// Esto genera el STOP
					}
				}
			}else{
				//if Last byte to be read?{
				//	if Second to last byte to be read{
				//		Set TXAK
				//	}else{
				//	}
				//}
			}
		}
	}
}

__ISR__ I2C0_IRQHandler(void) { IRQHandler(0); }
__ISR__ I2C1_IRQHandler(void) { IRQHandler(1); }
__ISR__ I2C2_IRQHandler(void) { IRQHandler(2); }
 
