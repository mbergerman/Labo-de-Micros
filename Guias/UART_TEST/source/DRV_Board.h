/***************************************************************************//**
  @file     DRV_Board.h
  @brief    FRDM Board management
  @author   Grupo 1
 ******************************************************************************/

#ifndef _BOARD_H_
#define _BOARD_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include <stdbool.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/



/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef enum{
	RED,
	GREEN,
	BLUE
}LED_t;

typedef enum{
	SW2,
	SW3
}SW_t;


/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Initialize FRDM Board
 */
void initBoard();

/**
 * @brief Sets value of LED
 * @param led led: RED, GREEN,	BLUE
 * @param value value: true or false
 */
void setLED(LED_t led, bool value);

/**
 * @brief Turn off LED
 * @param led led: RED, GREEN,	BLUE
 */
void clearLED(LED_t led);

/**
 * @brief Toggle LED
 * @param led led: RED, GREEN,	BLUE
 */
void toggleLED(LED_t led);

/**
 * @brief Read value of switch
 * @param sw sw: SW2, SW3
 */
bool getSW(SW_t sw);


/*******************************************************************************
 ******************************************************************************/

#endif // _BOARD_H_
