/***************************************************************************//**
  @file     Application.c
  @brief    App - TP3
  @author   Grupo 1 - M. Bergerman, P. Carranza Vélez, P. González Leiro, M. Moutin
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <string.h>
#include <stdio.h>
#include "Util.h"
#include "DRV_UART.h"
#include "DRV_Timers.h"
#include "DRV_WaveGen.h"
#include "DRV_Buttons.h"
#include "DRV_Board.h"
#include "DRV_LEDMatrix.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void delayLoop(uint32_t veces);

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/


/*******************************************************************************
 *******************************************************************************
                        	FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void App_Init() {

	// Init UART
	//initUartCom();

	// Init Timer
	//initTimers();

	initWaveGen();
	initBoard();

	LEDMatrix_init();
}

void App_Run() {
	static uint8_t counter = 0;

	static uint8_t old_w = 0;
	static uint8_t old_h = 1;
	static uint8_t old2_w = 0;
	static uint8_t old2_h = 2;
	static uint8_t old3_w = 0;
	static uint8_t old3_h = 3;
	static uint8_t w = 0;
	static uint8_t h = 0;
	static int w_dir = 1;
	static int h_dir = 0;

	static color_t led_color = {100, 0, 0};
	static color_t led_black = {0, 0, 0};

	LEDMatrix_updateLED(led_black, old3_h, old3_w);
	LEDMatrix_updateLED(led_color, h, w);

	timerDelay(TIMER_MS2TICKS(100));

	if(led_color.r == 100){
		led_color = (color_t){0, 0, 100};
	}else{
		led_color = (color_t){100, 0, 0};
	}

	old3_w = old2_w;
	old3_h = old2_h;
	old2_w = old_w;
	old2_h = old_h;
	old_w = w;
	old_h = h;

	w += w_dir;
	h += h_dir;
	if(w == 7 && w_dir == 1){
		w_dir = 0;
		h_dir = 1;
	}
	if(h == 7 && h_dir == 1){
		w_dir = -1;
		h_dir = 0;
	}
	if(w == 0 && w_dir == -1){
		w_dir = 0;
		h_dir = -1;
	}
	if(h == 0 && h_dir == -1){
		w_dir = 1;
		h_dir = 0;
	}

	counter += 1;

	if(counter == 28){
		counter = 0;
		if(LEDMatrix_getBrightness() < 100){
			LEDMatrix_setBrightness(100);
		}else{
			LEDMatrix_setBrightness(25);
		}
	}


	/*wavegenSetFreq(5);
	delayLoop(10000000UL);
	wavegenSetFreq(10);
	delayLoop(10000000UL);*/

}

static void delayLoop(uint32_t veces)
{
    while (veces--);
}
