/***************************************************************************//**
  @file     DRV_UART.h
  @brief    UART driver with package's functionality
  @author   Grupo 1
 ******************************************************************************/

#ifndef _DRV_DATATRANSMISSION_H_
#define _DRV_DATATRANSMISSION_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdbool.h>
#include <stdint.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define PAYLOAD_SIZE 	1

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

enum{
	SEND_DATA_OK = 0x81,
	SEND_DATA_FAIL = 0xC1,
	KEEP_ALIVE_OK = 0x82
};

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief initialize the communication
*/
void initDataTransmission(void);

/**
 * @brief Send three uint16_t values
 * @param num1
 * @param num2
 * @param num3
 * @return true if the tx is successful
*/
bool sendData(uint16_t num1, uint16_t num2, uint16_t num3);

/**
 * @brief Send keepalive signal
 * @return true if the tx is successful
*/
bool sendKeepAlive();

/**
 * @brief Determine if the response was received
 * @return true if response was received
*/
bool responseReady();

/**
 * @brief Receive the response from the gateway
 * @return the sixth byte of the transmission
*/
uint8_t getResponse(void) ;

/**
 * @brief empty the receiver's queue without reading the packages
*/
void emptyInbox(void);

/*******************************************************************************
 ******************************************************************************/

#endif // _DRV_DATATRANSMISSION_H_
