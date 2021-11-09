/***************************************************************************//**
  @file     DRV_UART.c
  @brief    UART driver with package's functionality
  @author   Grupo 1
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "DRV_DataTransmission.h"
#include "PDRV_UART.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define UART_ID 		0
#define RESPONSE_LEN	6

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
void initDataTransmission() {
	uart_cfg_t config = {1200,0};
	initUart(UART_ID, config);
}

bool sendData(uint16_t num1, uint16_t num2, uint16_t num3) {
	if (uartIsTxMsgComplete(UART_ID)) {
		char message[] = {0xAA, 0x55, 0xC3, 0x3C, 0x07, 0x01, num1>>8, num1&0xFF, num2>>8, num2&0xFF, num3>>8, num3&0xFF};
		uartWriteMsg(UART_ID, (char*)message, 12);
		return true;
	}
	else {
		return false;
	}
}

bool sendKeepAlive() {
	if (uartIsTxMsgComplete(UART_ID)) {
		char message[] = {0xAA, 0x55, 0xC3, 0x3C, 0x07, 0x02};
		uartWriteMsg(UART_ID, (char*)message, 6);
		return true;
	}
	else {
		return false;
	}
}

bool responseReady(){
	return uartIsRxMsg(UART_ID) && uartGetRxMsgLength(UART_ID) >= RESPONSE_LEN;
}

uint8_t getResponse(void) {
	char response[RESPONSE_LEN];
	if(uartIsRxMsg(UART_ID)) {
		if (uartGetRxMsgLength(UART_ID) >= RESPONSE_LEN) {
			uartReadMsg(UART_ID, response, RESPONSE_LEN);
		}
	}
	return response[5];
}

void emptyInbox(void) {
	char trash[32];
	if(uartIsRxMsg(UART_ID)) {
		uartReadMsg(UART_ID,trash,uartGetRxMsgLength(UART_ID));
	}
}
