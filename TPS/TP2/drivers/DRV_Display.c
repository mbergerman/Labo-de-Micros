/***************************************************************************//**
  @file     DRV_Display.c
  @brief    Display driver
  @author   LM1
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "DRV_Display.h"
#include "DRV_Timers.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

// Numbers
//				  gfedcba
#define ZERO	0b0111111
#define ONE		0b0000110
#define TWO		0b1011011
#define THREE 	0b1001111
#define FOUR	0b1100110
#define FIVE	0b1101101
#define SIX 	0b1111101
#define SEVEN	0b0000111
#define EIGHT	0b1111111
#define NINE	0b1101111

// Letters
//				  gfedcba
#define LET_C	0b0111001
#define LET_A	0b0110111
#define LET_R	0b1010000
#define LET_D	0b1011110

//				  gfedcba
#define LET_P	0b1110011
#define LET_I	0b0110000
#define LET_N	0b1010100

//				  gfedcba
#define LET_E	0b1111001
#define LET_F	0b1110001
#define LET_U	0b0111110
#define LET_H	0b1110110
#define LET_O	0b0111111
#define LET_T	0b1111000
#define LET_G	0b0111101
#define LET_L	0b0111000
#define LET_S	0b1101101
#define LET_J	0b0011110
#define LET_Q	0b1100111
#define LET_Y	0b1101110
#define LET_B 	0b1111100

//				  	  gfedcba
#define SYM__		0b0001000
#define SYM_DASH	0b1000000
#define SYM_BACK	0b0100001
#define SYM_FRWD	0b0001100

// GPIOs
// Seven Segment Selector
#define SEL0     PORTNUM2PIN(PC,17)
#define SEL1     PORTNUM2PIN(PB,9)
// Seven Segment Pins
#define SSEGA    PORTNUM2PIN(PB,18)
#define SSEGB    PORTNUM2PIN(PB,19)
#define SSEGC    PORTNUM2PIN(PC,1)
#define SSEGD    PORTNUM2PIN(PC,8)
#define SSEGE    PORTNUM2PIN(PC,9)
#define SSEGF    PORTNUM2PIN(PC,0)
#define SSEGG    PORTNUM2PIN(PC,7)

#define SSEGDP	PORTNUM2PIN(PC,5)

#define VALIDCHARS	54

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
typedef struct selector_data_t
{
	bool select0;
	bool select1;
}selector_data_t;

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static uint8_t buffer[DISP_BUFFER_LEN];

static bool decimalPoint[DISP_CHARS_NUM];

static int positionCounter = 0;
static int end_of_line = DISP_CHARS_NUM;

static int segment_counter = 0;

static int my_refresh_rate = 5;

static int my_brightness = (5) * (8.0 / 10.0);

static tim_id_t rate_id;
static tim_id_t bright_id;
static tim_id_t scroll_id;

static const char validChars[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'a', 'B', 'b', 'C', 'c',
									'D', 'd', 'E', 'e', 'F', 'f', 'G', 'g', 'H', 'h', 'I', 'i', 'J', 'j', 'L', 'l',
									'N', 'n', 'O', 'o', 'P', 'p', 'Q', 'q', 'R', 'r', 'S', 's', 'T', 't', 'U', 'u',
									'Y', 'y', '_', '-', '<', '>'};

static const uint8_t seven_segment_symbols[] = {ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, LET_A, LET_A, LET_B, LET_B, LET_C, LET_C,
								LET_D, LET_D, LET_E, LET_E, LET_F, LET_F, LET_G, LET_G, LET_H, LET_H, LET_I, LET_I, LET_J, LET_J, LET_L, LET_L,
								LET_N, LET_N, LET_O, LET_O, LET_P, LET_P, LET_Q, LET_Q, LET_R, LET_R, LET_S, LET_S, LET_T, LET_T, LET_U, LET_U,
								LET_Y, LET_Y, SYM__, SYM_DASH, SYM_BACK, SYM_FRWD};

static selector_data_t seven_segment_selector[] = {{LOW, LOW}, {HIGH, LOW}, {LOW, HIGH}, {HIGH, HIGH}};


// isValidChar(char c, int* index) - Checks if c is a valid character to print. Returns index.
static bool isValidChar(char c, int* index);

// update_end_of_line() - Updates End of Line in the buffer
static void update_end_of_line();

// getSevenSegment(char c) - Get seven segment code from a character.
static uint8_t getSevenSegment(int index);

// update_display() - Updates the display.
static void update_display();

// turn_off_seven_segment() - puts current seven segment all low.
static void turn_off_seven_segment();

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

// initDisplay() - Initialize display.
void initDisplay()
{
	gpioMode(SEL0, OUTPUT);
	gpioMode(SEL1, OUTPUT);

	gpioMode(SSEGA, OUTPUT);
	gpioMode(SSEGB, OUTPUT);
	gpioMode(SSEGC, OUTPUT);
	gpioMode(SSEGD, OUTPUT);
	gpioMode(SSEGE, OUTPUT);
	gpioMode(SSEGF, OUTPUT);
	gpioMode(SSEGG, OUTPUT);
	gpioMode(SSEGDP, OUTPUT);

	dispClearBuffer();

	int i;

	for(i=0; i<DISP_CHARS_NUM;i++)
	{
		dispClearDP(i);
	}

	rate_id = timerGetId();
	bright_id = timerGetId();
	scroll_id = timerGetId();
	timerStart(rate_id, TIMER_MS2TICKS(my_refresh_rate), TIM_MODE_PERIODIC, &update_display);
}

// clearDisplay() - Clears the display.
void dispClearBuffer()
{
	int i;
	for(i=0; i<DISP_BUFFER_LEN; i++)
	{
		buffer[i] = 0;
	}
	positionCounter = 0;
	end_of_line = DISP_CHARS_NUM;
}

// clearChar() - Clears the character from position n from the buffer.
void dispClearChar(int n)
{
	if(n < DISP_BUFFER_LEN && n >= 0)
	{
		buffer[n] = 0;
	}
	update_end_of_line();
}

// setChar() - sets character in position n of the buffer.
void dispWriteChar(int n, char character)
{
	if(n < DISP_BUFFER_LEN && n >= 0)
	{
		int index;
		if(isValidChar(character, &index))
		{
			buffer[n] = getSevenSegment(index);
		}
	}
	update_end_of_line();
}

// setWord() - sets word of length c in the buffer.
void dispWriteBuffer(int c, const char* word)
{
	dispClearBuffer();
	if(c < DISP_BUFFER_LEN && c >= 0)
	{
		int i;
		for(i = 0; i < c; i++)
		{
			int index;
			if(isValidChar(word[i], &index))
			{
				buffer[i] = getSevenSegment(index);
			}
		}
	}
	end_of_line = c;

	for(uint8_t i = 0; i < DISP_CHARS_NUM; i++){
		dispClearDP(i);
	}
}

void dispUpdateRefreshRate(int refreshRate)
{
	timerStop(rate_id);
	timerStart(rate_id, TIMER_MS2TICKS(refreshRate), TIM_MODE_PERIODIC, &update_display);
	my_refresh_rate = refreshRate;
}

void dispUpdateBrightness(int brightness)
{
	if(brightness > 9) brightness = 9;
	if(brightness < 1) brightness = 1;
	my_brightness = (my_refresh_rate) * ((brightness+1) / 10.0);
}


int dispGetBrightness(){
	return (my_brightness * 10.0) / my_refresh_rate - 1;
}


// scrollLeft() - Scroll display to the left.
void dispScrollLeft()
{
	if(positionCounter > 0)
	{
		positionCounter--;
	}
}

// scrollRight() - Scroll display to the right.
void dispScrollRight()
{
	if((end_of_line) > positionCounter)
	{
		positionCounter++;
	}
	else
	{
		positionCounter = -DISP_CHARS_NUM + 1;
	}
}

// startAutoScroll(int speed) - Initialize autoscroll with specified speed.
void dispStartAutoScroll(int speed)
{
	timerStart(scroll_id, TIMER_MS2TICKS(speed), TIM_MODE_PERIODIC, &dispScrollRight);
}

// stopAutoScroll() - Stop autoscroll.
void dispStopAutoScroll()
{
	timerStop(scroll_id);
}

// setPosition(int n) - Sets n as the starting print position for the buffer.
void dispSetBufferPos(int n)
{
	if(n < DISP_BUFFER_LEN && n >= 0)
	{
		positionCounter = n;
	}
}

// getPosition() - Get starting print position.
uint8_t dispGetBufferPos()
{
	return positionCounter;
}

// dispSetDP(int n) - Turns on the decimal point of the nth seven segment. n = 0, 1, 2, 3
void dispSetDP(int n)
{
	if(n >= 0 && n < DISP_CHARS_NUM)
	{
		decimalPoint[n] = true;
	}
}

// dispSetDP(int n) - Turns off the decimal point of the nth seven segment. n = 0, 1, 2, 3
void dispClearDP(int n)
{
	if(n >= 0 && n < DISP_CHARS_NUM)
	{
		decimalPoint[n] = false;
	}
}

// dispSetDP(int n) - Toggles the decimal point of the nth seven segment. n = 0, 1, 2, 3
void dispToggleDP(int n)
{
	if(n >= 0 && n < DISP_CHARS_NUM)
	{
		decimalPoint[n] = !decimalPoint[n];
	}
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

// isValidChar(char c, int* index) - Checks if c is a valid character to print. Returns index (for getSevenSegment).
static bool isValidChar(char c, int* index)
{
	bool ret = false;
	int i;
	for(i=0; i < VALIDCHARS; i++)
	{
		if(c == validChars[i])
		{
			ret = true;
			*index = i;
			break;
		}
	}
	return ret;
}

static void update_end_of_line()
{
	int i;
	for(i = 0; i < (DISP_BUFFER_LEN - DISP_CHARS_NUM); i++)
	{
		if(buffer[i] == 0 && buffer[i+1] == 0 && buffer[i+2] == 0 && buffer[i+3] == 0)
		{
			end_of_line = i;
			break;
		}
		else
		{
			end_of_line = DISP_BUFFER_LEN;
			break;
		}
	}
}

// getSevenSegment(int index) - Get seven segment code from an index.
static uint8_t getSevenSegment(int index)
{
	return seven_segment_symbols[index];
}

static void update_display()
{
	// Update Display
	static int i;

	if(segment_counter == DISP_CHARS_NUM)
	{
		segment_counter = 0;
		i = positionCounter;
	}

	timerStart(bright_id, TIMER_MS2TICKS(my_brightness), TIM_MODE_SINGLESHOT, &turn_off_seven_segment);

	gpioWrite(SEL0, seven_segment_selector[segment_counter].select0);
	gpioWrite(SEL1, seven_segment_selector[segment_counter].select1);


	if(0 <= i && i < end_of_line)
	{
		gpioWrite(SSEGA, ((buffer[i] >> 0)  & 0x01));
		gpioWrite(SSEGB, ((buffer[i] >> 1)  & 0x01));
		gpioWrite(SSEGC, ((buffer[i] >> 2)  & 0x01));
		gpioWrite(SSEGD, ((buffer[i] >> 3)  & 0x01));
		gpioWrite(SSEGE, ((buffer[i] >> 4)  & 0x01));
		gpioWrite(SSEGF, ((buffer[i] >> 5)  & 0x01));
		gpioWrite(SSEGG, ((buffer[i] >> 6)  & 0x01));

		gpioWrite(SSEGDP, decimalPoint[segment_counter]);
	}else{
		gpioWrite(SSEGA, LOW);
		gpioWrite(SSEGB, LOW);
		gpioWrite(SSEGC, LOW);
		gpioWrite(SSEGD, LOW);
		gpioWrite(SSEGE, LOW);
		gpioWrite(SSEGF, LOW);
		gpioWrite(SSEGG, LOW);

		gpioWrite(SSEGDP, LOW);
	}


	segment_counter++;
	i++;
}

static void turn_off_seven_segment()
{
	gpioWrite(SSEGA, LOW);
	gpioWrite(SSEGB, LOW);
	gpioWrite(SSEGC, LOW);
	gpioWrite(SSEGD, LOW);
	gpioWrite(SSEGE, LOW);
	gpioWrite(SSEGF, LOW);
	gpioWrite(SSEGG, LOW);
}
