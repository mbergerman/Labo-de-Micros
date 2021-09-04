/***************************************************************************//**
  @file     DRV_Display.c
  @brief    +Descripcion del archivo+
  @author   matia
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "DRV_Display.h"
#include <stdio.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define DISPLAY_DEVELOPMENT_MODE    1

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/



/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

// +ej: unsigned int anio_actual;+


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void print_buffer();


/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

// +ej: static const int temperaturas_medias[4] = {23, 26, 24, 29};+


/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static char char_buffer[BUFFER_LEN];
static uint8_t buffer_current_len = 0;
static uint8_t buffer_pos = 0;


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void initDisplay(){
	printf("Console Display initialized!\n");
	dispClearBuffer();
}

void dispClearBuffer(){
	buffer_current_len = 0;
	buffer_pos = 0;
	print_buffer();
}

void dispWriteBuffer(int c, char* buffer){
#ifdef DISPLAY_DEVELOPMENT_MODE
	if(c > BUFFER_LEN) return;
#endif // DISPLAY_DEVELOPMENT_MODE

	for(int i = 0; i < c; i++){
		char_buffer[i] = buffer[i];
	}
	buffer_current_len = c;
	print_buffer();
}

void dispWriteChar(int n, char character){
#ifdef DISPLAY_DEVELOPMENT_MODE
	if(n >= BUFFER_LEN || n > buffer_current_len) return;
#endif // DISPLAY_DEVELOPMENT_MODE

	char_buffer[n] = character;
	if(n >= buffer_current_len) buffer_current_len = n+1;
	print_buffer();
}
bool dispValidChar(char character){
	return true;
}

void dispScrollLeft(){
	buffer_pos++;
	buffer_pos = (buffer_pos >= buffer_current_len)? 0 : buffer_pos;
	print_buffer();
}


void dispScrollRight(){
	buffer_pos--;
	buffer_pos = (buffer_pos < 0)? buffer_current_len-1 : buffer_pos;
	print_buffer();
}

void dispStartAutoscroll(int speed){}	//TO-DO
void dispStopAutoscroll(){}				//TO-DO


void dispResetScroll(){
	dispSetBufferPos(0);
}
void dispSetBufferPos(int n){
#ifdef DISPLAY_DEVELOPMENT_MODE
	if(n < 0) return;
#endif // DISPLAY_DEVELOPMENT_MODE

	buffer_pos = n%buffer_current_len;
	print_buffer();
}

int dispGetBufferPos(){
	return buffer_pos;
}

void dispClearLED(int n);	//TO-DO
void dispSetLED(int n);		//TO-DO
void dispToggleLED();		//TO-DO


static void print_buffer(){
	printf("Display: |");
	for(int i = 0; i < CHARS_NUM; i++){
		uint8_t index = (i + buffer_pos)%buffer_current_len;
		printf("%c|", (i < buffer_current_len)? char_buffer[index] : ' ');
	}
	printf("\n");
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
 
