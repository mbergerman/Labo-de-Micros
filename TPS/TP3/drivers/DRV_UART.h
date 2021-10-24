/***************************************************************************//**
  @file     DRV_UART.h
  @brief    UART driver with package's functionality
  @author   Grupo 1
 ******************************************************************************/

#ifndef _DRV_UART_H_
#define _DRV_UART_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include <stdbool.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define UART_ID 3
#define TOPIC_SIZE 1
#define PAYLOAD_SIZE 1
#define PACKAGE_SIZE (PAYLOAD_SIZE+TOPIC_SIZE)

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

// A package has a topic and a payload.
// The topic is used to identify the category of the package
// The payload is the content
// For example for 45% brightness we use, package = {{'B'},{45}}

typedef struct {
	char topic[TOPIC_SIZE];
	char payload[PAYLOAD_SIZE];
} package_t;


/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief initialize the communication
*/
void initUartCom(void);

/**
 * @brief Add a package to the transmitter's queue
 * @param package to be transmitted
 * @return true if the tx is successful
*/
bool sentPackage(package_t package);

/**
 * @brief Receive the first package in the receiver's queue
 * @return the package or an empty package {{0},{0}} if there aren't any packages in the queue
*/
package_t receivePackage(void);

/**
 * @brief empty the receiver's queue without reading the packages
*/
void emptyInbox(void);
/*******************************************************************************
 ******************************************************************************/

#endif // _DRV_UART_H_
