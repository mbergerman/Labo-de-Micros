/***************************************************************************//**
  @file     App.c
  @brief    Application functions
  @author   Pablo Gonzalez
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "MK64F12.h"
#include "hardware.h"
#include <stdio.h>
#include <string.h>
#include "DRV_UART.h"
#include "PDRV_UART.h"
#include "DRV_Timers.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define PAYLOAD(w) (w=='B'?"Brightness":"Other")

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
void timer_tx_callback();
void timer_rx_callback();


static tim_id_t timer_tx;
static tim_id_t timer_rx;

static uint8_t rgb_color[3];
static int indx = 0;

static bool print_color = false;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Función que se llama 1 vez, al comienzo del programa */
void App_Init (void)
{
	uart_cfg_t config = {9600,0};
	initUart(UART_ID,config);
	initTimers();
	timer_tx = timerGetId();
	timerStart(timer_tx, TIMER_MS2TICKS(1000), TIM_MODE_PERIODIC, timer_tx_callback);
	timer_rx = timerGetId();
	timerStart(timer_rx, TIMER_MS2TICKS(100), TIM_MODE_PERIODIC, timer_rx_callback);
}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
{

	if(print_color) {
		printf("rgb(%d,%d,%d)\n",rgb_color[0],rgb_color[1],rgb_color[2]);
		print_color = false;
	}



	//connect TX(PTD3) with RX (PTD2)

//	static bool run = true;
//	static package_t packages_received[1000];
//	static int index = 0;
//	static package_t package;
//	package.payload[0] = 'B'; //B = Brightness
//	package.message[0] = 0;
//
//	static package_t package_received;

//	if(run) {
//		for(int i=0; i>=101; i++) {
//			if(sentPackage(package)) {
//				package.message[0]++;
//			}
//		}
//		run = false;
//	}


//	while(run) {
//
//		if(sentPackage(package)) {
//			package.message[0]++;
//		}
//
//		package_received = receivePackage();
//
//		if(package_received.payload[0] != 0) {
//			packages_received[index++] = package_received;
//			//printf("Package received: %c, %d\n", package_received.payload[0], package_received.message[0]);
//		}
//
//		if(package_received.message[0] == 100) {
//			run = false;
//		}
//	}
//	static bool print = false;
//	if(!run && !print) {
//		printf("number\tpayload\t\tmessage\n");
//		printf("================================\n");
//		for(int j = 0; j<index; j++) {
//			printf("%d\t%s\t%7d%%\n", j, PAYLOAD(packages_received[j].payload[0]), packages_received[j].message[0]);
//		}
//		print = true;
//	}
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void timer_tx_callback() {
	static package_t package;
	package.payload[0] = 'B';

	if(sentPackage(package)) {
		package.message[0] = (package.message[0]+1) % 100;
	}
}

void timer_rx_callback() {
	package_t color_package;
	if (uartGetRxMsgLength(UART_ID) == 1) {
		emptyInbox();
	}
	color_package = receivePackage();
	indx = (indx+1)%100;


	char color = color_package.payload[0];

	switch(color) {
	case 'r':
		rgb_color[0] = color_package.message[0];
		break;
	case 'g':
		rgb_color[1] = color_package.message[0];
		break;
	case 'b':
		rgb_color[2] = color_package.message[0];
		print_color = true;
		break;
	}

}

/*******************************************************************************
 ******************************************************************************/

