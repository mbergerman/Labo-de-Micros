/***************************************************************************//**
  @file     +Nombre del archivo (ej: template.c)+
  @brief    +Descripcion del archivo+
  @author   +Nombre del autor (ej: Salvador Allende)+
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "DRV_UART.h"
#include "PDRV_UART.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/



/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/



/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

// +ej: static void falta_envido (int);+


/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

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
			uartReadMsg(UART_ID, package.payload, PAYLOAD_SIZE);
			uartReadMsg(UART_ID, package.message, MESSAGE_SIZE);
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

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/



