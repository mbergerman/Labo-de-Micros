/***************************************************************************//**
  @file     DRV_Stub_Display.c
  @brief    stdout-based display stub driver
  @author   Grupo 1
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


/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static char char_buffer[DISP_BUFFER_LEN];
static uint8_t buffer_current_len = 0;
static uint8_t buffer_pos = 0;

static bool dp_buffer[4] = {0};
static int brightness;
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
	for(int i=0; i<4; i++) {dp_buffer[i]=false;}
	buffer_current_len = 0;
	buffer_pos = 0;
	print_buffer();
}

void dispWriteBuffer(int c, const char* buffer){
#ifdef DISPLAY_DEVELOPMENT_MODE
	if(c > DISP_BUFFER_LEN) return;
#endif // DISPLAY_DEVELOPMENT_MODE

	for(int i = 0; i < c; i++){
		char_buffer[i] = buffer[i];
	}
	buffer_current_len = c;
	print_buffer();
}

void dispWriteChar(int n, const char character){
#ifdef DISPLAY_DEVELOPMENT_MODE
	if(n >= DISP_BUFFER_LEN || n > buffer_current_len) return;
#endif // DISPLAY_DEVELOPMENT_MODE

	char_buffer[n] = character;
	if(n >= buffer_current_len) buffer_current_len = n+1;
	print_buffer();
}
bool dispValidChar(char character){
	return true;
}

void dispScrollRight(){
	buffer_pos++;
	buffer_pos = (buffer_pos >= buffer_current_len)? 0 : buffer_pos;
	print_buffer();
}


void dispScrollLeft(){
	buffer_pos--;
	buffer_pos = (buffer_pos < 0)? buffer_current_len-1 : buffer_pos;
	print_buffer();
}

void dispStartAutoScroll(int speed){
	printf("Speed = %d\n", speed);
}
void dispStopAutoScroll(){}				//TO-DO


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

uint8_t dispGetBufferPos(){
	return buffer_pos;
}

void dispClearLED(int n);	//TO-DO
void dispSetLED(int n);		//TO-DO
void dispToggleLED();		//TO-DO




void dispSetDP(int i) {
	dp_buffer[i] = true;
}
void dispClearDP(int i) {
	dp_buffer[i] = false;
}
void dispToggleDP(int i) {
	dp_buffer[i] ^= 0b1;
}
void dispUpdateBrightness(int b) {
	brightness = b;
	printf("Brightness %d\n", brightness);
}

int dispGetBrightness() {
	return brightness;
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
static void print_buffer(){
	//printf("Display: |");
	for(int i = 0; i < DISP_CHARS_NUM; i++){
		uint8_t index = (i + buffer_pos)%buffer_current_len;
		printf("%c|", (i < buffer_current_len)? char_buffer[index] : ' ');
	}
	printf("\t");
	for(int i = 0; i < DISP_CHARS_NUM; i++){
		printf("%1d|", dp_buffer[i]);
	}
	printf("\n");
}
