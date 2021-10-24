/***************************************************************************//**
  @file     DRV_UART.c
  @brief    UART driver with package's functionality
  @author   Grupo 1
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "DRV_UART.h"
#include "PDRV_UART.h"

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
void initUartCom() {
	uart_cfg_t config = {9600,0};
	initUart(UART_ID,config);
}

bool sentPackage(package_t package) {
	if (uartIsTxMsgComplete(UART_ID)) {
		uartWriteMsg(UART_ID, (char*)&package, PACKAGE_SIZE);
		return true;
	}
	else {
		return false;
	}
}

package_t receivePackage(void) {
	package_t package = {{0},{0}};
	if(uartIsRxMsg(UART_ID) && (uartGetRxMsgLength(UART_ID) >= PACKAGE_SIZE) ) {
		if (uartGetRxMsgLength(UART_ID) >= PACKAGE_SIZE) {
			uartReadMsg(UART_ID, package.topic, TOPIC_SIZE);
			uartReadMsg(UART_ID, package.payload, PAYLOAD_SIZE);
		}
	}
	return package;
}

void emptyInbox(void) {
	char trash[32];
	if(uartIsRxMsg(UART_ID)) {
		uartReadMsg(UART_ID,trash,uartGetRxMsgLength(UART_ID));
	}
}
