/***************************************************************************//**
  @file     +Nombre del archivo (ej: template.h)+
  @brief    +Descripcion del archivo+
  @author   +Nombre del autor (ej: Salvador Allende)+
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
#define PAYLOAD_SIZE 1
#define MESSAGE_SIZE 1
#define PACKAGE_SIZE (PAYLOAD_SIZE+MESSAGE_SIZE)

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
typedef struct {
	char payload[PAYLOAD_SIZE];
	char message[MESSAGE_SIZE];
} package_t;

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/


/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief TODO: completar descripcion
 * @param param1 Descripcion parametro 1
 * @param param2 Descripcion parametro 2
 * @return Descripcion valor que devuelve
*/

bool sentPackage(package_t package);
package_t receivePackage(void);
void emptyInbox(void);
/*******************************************************************************
 ******************************************************************************/

#endif // _DRV_UART_H_
