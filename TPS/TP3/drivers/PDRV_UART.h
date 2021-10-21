/***************************************************************************//**
  @file     UART.c
  @brief    UART Driver for K64F. Non-Blocking and using FIFO feature
  @author   Nicolás Magliola
 ******************************************************************************/

#ifndef _UART_H_
#define _UART_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define UART_CANT_IDS	6

#define BUFFER_SIZE	32

// Ports
enum { PA, PB, PC, PD, PE };

// Convert port and number into pin ID
// Ex: PTB5  -> PORTNUM2PIN(PB,5)  -> 0x25
//     PTC22 -> PORTNUM2PIN(PC,22) -> 0x56
#define PORTNUM2PIN(p,n)    (((p)<<5) + (n))
#define PIN2PORT(p)         (((p)>>5) & 0x07)
#define PIN2NUM(p)          ((p) & 0x1F)

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef struct {
	uint32_t baudrate;
	bool parity;
	//etc
} uart_cfg_t;


/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Initialize UART driver
 * @param id UART's number
 * @param config UART's configuration (baudrate, parity, etc.)
*/
void initUart (uint8_t id, uart_cfg_t config);

/**
 * @brief Check if a new byte was received
 * @param id UART's number
 * @return A new byte has being received
*/
uint8_t uartIsRxMsg(uint8_t id);

/**
 * @brief Check how many bytes were received
 * @param id UART's number
 * @return Quantity of received bytes
*/
uint32_t uartGetRxMsgLength(uint8_t id);

/**
 * @brief Read a received message. Non-Blocking
 * @param id UART's number
 * @param msg Buffer to paste the received bytes
 * @param cant Desired quantity of bytes to be pasted
 * @return Real quantity of pasted bytes
*/
uint8_t uartReadMsg(uint8_t id, char* msg, uint8_t cant);

/**
 * @brief Write a message to be transmitted. Non-Blocking
 * @param id UART's number
 * @param msg Buffer with the bytes to be transfered
 * @param cant Desired quantity of bytes to be transfered
 * @return Real quantity of bytes to be transfered
*/
uint8_t uartWriteMsg(uint8_t id, const char* msg, uint8_t cant);

/**
 * @brief Check if all bytes were transfered
 * @param id UART's number
 * @return All bytes were transfered
*/
bool uartIsTxMsgComplete(uint8_t id);


/*******************************************************************************
 ******************************************************************************/

#endif // _UART_H_
