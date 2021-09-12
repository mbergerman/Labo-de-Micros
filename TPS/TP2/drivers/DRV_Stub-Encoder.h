#ifndef _STUB_ENCODER_
#define _STUB_ENCODER_

#include <stdbool.h>

typedef enum{
	ENC_LEFT,
	ENC_RIGHT,
	ENC_CLICK,
	ENC_NONE
}encoderResult_t;

void initEncoder();

//Devuelve true si hubo movimiento
bool encoderGetStatus();

//Devuelve la data
encoderResult_t encoderGetEvent();

#endif
