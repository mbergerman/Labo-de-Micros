/***************************************************************************//**
  @file     DRV_Display.h
  @brief    7-segment display driver
  @author   Grupo 1
 ******************************************************************************/

#ifndef DRV_DISPLAY_H_
#define DRV_DISPLAY_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "PDRV_GPIO.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define DISP_CHARS_NUM	4
#define DISP_BUFFER_LEN	32		// Longitud del buffer de palabras

#define MIN_BRIGHT 1
#define MAX_BRIGHT 9

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Initialize display
 */
void initDisplay();

/**
 * @brief Clears the display buffer
 */
void dispClearBuffer();

/**
 * @brief Clears the character from position n from the buffer
 * @param n Index of char to clear
 */
void dispClearChar(int n);

/**
 * @brief Sets character in position n of the buffer
 * @param n Index of char to write
 * @param character Value of char
 */
void dispWriteChar(int n, char character);

/**
 * @brief Sets word of length c in the buffer
 * @param c Length of word
 * @param word Word to write
 */
void dispWriteBuffer(int c, const char* word);

/**
 * @brief Updates the refresh rate of the display
 * @param refreshRate New refresh rate
 */
void dispUpdateRefreshRate(int refreshRate);

/**
 * @brief Updates the brightness of the display
 * @param brightness brightness: int between one and nine
 */
void dispUpdateBrightness(int brightness);

/**
 * @brief Returns current display brightness
 * @return brightness value
 */
int dispGetBrightness();

/**
 * @brief Scroll display to the left
 */
void dispScrollLeft();

/**
 * @brief Scroll display to the right
 */
void dispScrollRight();

/**
 * @brief Initialize autoscroll with specified speed
 * @param speed Scroll speed
 */
void dispStartAutoScroll(int speed);

/**
 * @brief Stop autoscroll
 */
void dispStopAutoScroll();

/**
 * @brief Sets n as the starting print position for the buffer
 * @param n New buffer position
 */
void dispSetBufferPos(int n);

/**
 * @brief Get starting print position
 */
uint8_t dispGetBufferPos();

/**
 * @brief Turns on the decimal point of the nth seven segment
 * @param n DP index = 0, 1, 2, 3
 */
void dispSetDP(int n);

/**
 * @brief Turns off the decimal point of the nth seven segment
 * @param n DP index = 0, 1, 2, 3
 */
void dispClearDP(int n);

/**
 * @brief Toggles the decimal point of the nth seven segment
 * @param n DP index = 0, 1, 2, 3
 */
void dispToggleDP(int n);

/*******************************************************************************
 ******************************************************************************/

#endif /* DRV_DISPLAY_H_ */
