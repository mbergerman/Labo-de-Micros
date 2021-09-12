#include <DRV_Stub_Encoder.h>
#include <stdio.h>
#include "DRV_Board.h"
#include "PDRV_GPIO.h"

static void callback_left(void);
static void callback_right(void);
static void callback_click(void);

void initEncoder() {
	initBoard();
	gpioIRQ (PORTNUM2PIN(PC,6), GPIO_IRQ_MODE_RISING_EDGE, callback_left);
	gpioIRQ (PORTNUM2PIN(PA,4), GPIO_IRQ_MODE_RISING_EDGE, callback_right);
	gpioMode (PORTNUM2PIN(PB,2), INPUT);
	gpioIRQ (PORTNUM2PIN(PB,2), GPIO_IRQ_MODE_FALLING_EDGE, callback_click);
}




static encoderResult_t event = ENC_NONE;
static bool status = false;

//Devuelve true si hubo movimiento
bool encoderGetStatus() {
	if (event == ENC_NONE) {
		status = false;
	}
	if(status == true) {
		status = false;
		return true;
	}
	else {
		return false;
	}
}

//Devuelve la data
encoderResult_t encoderGetEvent() {
	encoderResult_t event_aux = event;
	event = ENC_NONE;
	return event_aux;
}


static void callback_left(void) {
	event = ENC_LEFT;
	status = true;
}
static void callback_right(void) {
	event = ENC_RIGHT;
	status = true;
}
static void callback_click(void) {
	event = ENC_CLICK;
	status = true;
}
