/***************************************************************************//**
  @file     DRV_Reader.c
  @brief    +Descripcion del archivo+
  @author   +Nombre del autor (ej: Salvador Allende)+
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "DRV_Reader.h"
#include "DRV_Timers.h"
#include "PDRV_GPIO.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define READER_EN_PIN	PORTNUM2PIN(PD, 0)	// PTD0
#define READER_CLK_PIN	PORTNUM2PIN(PC, 4)	// PTC4
#define READER_DATA_PIN	PORTNUM2PIN(PC, 12)	// PTC12

#define EN_ACTIVE	false
#define DATA_ONE	false
#define DATA_ZERO	(!DATA_ONE)

#define CHAR_BITS		5
#define BUFFER_LEN		40
#define PAN_MAX_LEN		19
#define ADD_CHARS_MIN	7
#define NUM_DATA_LEN	37
#define FS_LEN			1

#define SS_CHAR	';'
#define FS_CHAR	'='
#define ES_CHAR	'?'

#define ES_BITS	0b11111

// Hacer una tabla en vez de una macro?
#define BITS2CHAR(b)	((b == 0b10000) ? '0' : \
						((b == 0b00001) ? '1' : \
						((b == 0b00010) ? '2' : \
						((b == 0b10011) ? '3' : \
						((b == 0b00100) ? '4' : \
						((b == 0b10101) ? '5' : \
						((b == 0b10110) ? '6' : \
						((b == 0b00111) ? '7' : \
						((b == 0b01000) ? '8' : \
						((b == 0b11001) ? '9' : \
						((b == 0b11010) ? ':' : \
						((b == 0b01011) ? ';' : \
						((b == 0b11100) ? '<' : \
						((b == 0b01101) ? '=' : \
						((b == 0b01110) ? '>' : \
						((b == 0b11111) ? '?' : 'X' \
						))))))))))))))))

#define BITPARITY4(b)	((b & 0b1) ^ (b>>1 & 0b1) ^ (b>>2 & 0b1) ^ (b>>3 & 0b1))

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

enum{
	START,
	CHAR,
	END
};

enum{
	SS,
	PAN,
	AD,
	LRC,
	SUCCESS
};


/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void reader_enable_irq();
static void reader_clock_irq();
static void parse_buffer(char*, uint8_t*);
static void reader_timeout_callback();

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/



/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static readerCallback_t reader_finished_callback;
static char reading_buffer[BUFFER_LEN];
static uint8_t reading_buffer_pos;
static uint8_t lrc_check;
static bool bit_buffer_flag;
static bool reader_running;
static tim_id_t reader_tim_id;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void initReader(readerCallback_t readerCallback){
	gpioMode(READER_EN_PIN, INPUT);
	gpioMode(READER_CLK_PIN, INPUT);
	gpioMode(READER_DATA_PIN, INPUT);

	reader_finished_callback = readerCallback;

	gpioIRQ(READER_EN_PIN, GPIO_IRQ_MODE_BOTH_EDGES, reader_enable_irq);

	reader_tim_id = timerGetId();
}

bool readerRunning(){
	return reader_running;
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

// sacar la interrupcion por enable
static void reader_enable_irq(){
	if(gpioRead(READER_EN_PIN) == EN_ACTIVE){
		bit_buffer_flag = true;
		reader_running = true;
		timerStart(reader_tim_id, TIMER_MS2TICKS(READER_TIMEOUT), TIM_MODE_SINGLESHOT, reader_timeout_callback);
		gpioIRQ(READER_CLK_PIN, GPIO_IRQ_MODE_FALLING_EDGE, reader_clock_irq);
	}else{
		if(reader_running){
			timerStop(reader_tim_id);
			reader_running = false;

			gpioIRQ(READER_CLK_PIN, GPIO_IRQ_MODE_DISABLE, reader_clock_irq);

			char result_buffer[PAN_MAX_LEN];
			uint8_t result_buffer_len;

			//TO-DO: Hacer el parseo cuando me lo pide el usuario
			parse_buffer(result_buffer, &result_buffer_len);

			(*reader_finished_callback)( result_buffer, result_buffer_len );
		}
	}
}

static void reader_clock_irq(){
	static uint8_t state = START;
	static bool bit_buffer[CHAR_BITS];
	static uint8_t bit_buffer_pos = 0;
	static bool lrc_flag = false;

	if(bit_buffer_flag){
		state = START;
		bit_buffer_pos = 0;
		reading_buffer_pos = 0;
		bit_buffer_flag = false;
	}

	bool data = (gpioRead(READER_DATA_PIN) == DATA_ONE);

	switch(state){
	case START:
		if(data){
			state = CHAR;
			bit_buffer[0] = 1;
			bit_buffer_pos = 1;
			lrc_check = 0b00000;
			lrc_flag = false;
		}
		break;
	case CHAR:
		bit_buffer[bit_buffer_pos] = data;
		bit_buffer_pos += 1;
		if(bit_buffer_pos == CHAR_BITS){
			uint8_t new_char = 0;
			for(uint8_t i = 0; i < CHAR_BITS; i++){
				new_char = (new_char<<1) | (bit_buffer[CHAR_BITS - 1 - i]);
			}
			bit_buffer_pos = 0;
			reading_buffer[reading_buffer_pos] = BITS2CHAR(new_char);
			reading_buffer_pos += 1;

			if(lrc_flag == false)	lrc_check ^= new_char;

			if(new_char == ES_BITS) lrc_flag = true;

			if(reading_buffer_pos == BUFFER_LEN){
				state = END;
			}
		}
		break;
	case END:
		break;
	}
}

static void parse_buffer(char* result_buffer_ptr, uint8_t* result_buffer_len_ptr){
	uint8_t state = SS;
	*result_buffer_len_ptr = 0;
	uint8_t additional_chars = 0;

	for(uint8_t i = 0; i < reading_buffer_pos; i++){
		switch(state){
		case SS:
			if(reading_buffer[i] != SS_CHAR) goto breakParseLoop;
			state = PAN;
			break;
		case PAN:
			if(reading_buffer[i] != FS_CHAR){
				if(*result_buffer_len_ptr >= PAN_MAX_LEN) goto breakParseLoop;
				if(reading_buffer[i] < '0' || reading_buffer[i] > '9') goto breakParseLoop;

				result_buffer_ptr[*result_buffer_len_ptr] = reading_buffer[i];
				(*result_buffer_len_ptr)++;
			}else{
				result_buffer_ptr[*result_buffer_len_ptr] = TERMINATOR_CHAR;
				state = AD;
			}
			break;
		case AD:
			if(reading_buffer[i] == ES_CHAR){
				if(additional_chars < ADD_CHARS_MIN) goto breakParseLoop;
				if(additional_chars > (NUM_DATA_LEN-FS_LEN-(*result_buffer_len_ptr))) goto breakParseLoop;
				state = LRC;
			}else{
				additional_chars++;
			}
			break;
		case LRC:
			if(reading_buffer[i] != BITS2CHAR(lrc_check)) goto breakParseLoop;
			state = SUCCESS;
			break;
		case SUCCESS:
			goto breakParseLoop;
		}
	}

breakParseLoop:
	if(state != SUCCESS) *result_buffer_len_ptr = 0;
	return;
}

static void reader_timeout_callback(){
	if(reader_running){
		timerStop(reader_tim_id);
		reader_running = false;

		gpioIRQ(READER_CLK_PIN, GPIO_IRQ_MODE_DISABLE, reader_clock_irq);
	}
}
