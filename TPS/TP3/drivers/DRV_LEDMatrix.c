/***************************************************************************//**
  @file     DRV_ShiftRegister.c
  @brief    LED marix driver.
  @author   Grupo 1
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "PDRV_DMA.h"
#include "PDRV_FTM.h"

#include "DRV_Timers.h"
#include "DRV_LEDMatrix.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define LED_BITS 24

#define GREEN_CHECK 0
#define RED_CHECK	1
#define BLUE_CHECK	2

#define COLOR_RESOLUTION 8

#define FTM_MODULO 61

#define MATRIX0	(uint16_t) 20
#define MATRIX1 (uint16_t) 43
#define MATRIXR (uint16_t) 1

#define MATRIX_BIT_SYMBOL(brightness, i)	(((brightness >> (COLOR_RESOLUTION-1 - i%COLOR_RESOLUTION) & 0x01) == 0) ? MATRIX0 : MATRIX1)

#define PMW_ARRAY_LEN	HEIGHT*WIDTH*LED_BITS

#define MAX_BRIGHTNESS			(float)100.0

#define LED_MATRIX_DMA_CHNNL	0
#define LED_MATRIX_FTM_CHNNL	0

#define RESET_TICKS		TIMER_MS2TICKS(2)

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef uint16_t PWM_Array_t[PMW_ARRAY_LEN];

/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void LEDMatrix_RGB2PWM();
static void LEDMatrix_reset();
static void LEDMatrix_restart();

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static FTM_Config_t ftm_config;
static DMA_config_t dma_config;

static PWM_Array_t pwm_led_matrix;
static LEDMatrix_t rgb_led_matrix;

static tim_id_t reset_tim_id;

static float matrix_brightness = 100.0;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/


void LEDMatrix_init()
{
	initTimers();

	ftm_config.channel = FTM_Channel_0;
	ftm_config.mode = FTM_PWM;
	ftm_config.prescale = FTM_Prescale_1;
	ftm_config.CLK_source = FTM_SysCLK;
	ftm_config.PWM_logic = FTM_High;
	ftm_config.modulo = FTM_MODULO;
	ftm_config.PWM_DC = 20;
	ftm_config.active_low = false;
	ftm_config.DMA_on = true;
	ftm_config.interrupt_on = true;

	dma_config.source_buffer = pwm_led_matrix;
	dma_config.destination_buffer = FTM_getCounterPointer(0, FTM_Channel_0);
	dma_config.request_source = FTM0CH0;
	dma_config.source_offset = sizeof(uint16_t);
	dma_config.destination_offset = 0;
	dma_config.transfer_bytes = sizeof(uint16_t);
	dma_config.major_cycles = PMW_ARRAY_LEN;
	dma_config.wrap_around = sizeof(pwm_led_matrix);

	reset_tim_id = timerGetId();

	LEDMatrix_RGB2PWM();

	FTM_init(LED_MATRIX_FTM_CHNNL, ftm_config);
	DMA_init(LED_MATRIX_DMA_CHNNL, dma_config);
	DMA_setMajorCallback(LED_MATRIX_DMA_CHNNL, LEDMatrix_reset);

	FTM_start(LED_MATRIX_FTM_CHNNL);
}

void LEDMatrix_updateLED(color_t led, uint8_t height, uint8_t width){
	rgb_led_matrix[height][width] = led;
	LEDMatrix_RGB2PWM();
}

void LEDMatrix_setBrightness(float brightness)
{
	matrix_brightness = (brightness > MAX_BRIGHTNESS) ? MAX_BRIGHTNESS : ((brightness < 0) ? 0 : brightness);
	LEDMatrix_RGB2PWM();
}

float LEDMatrix_getBrightness()
{
	return matrix_brightness;
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void LEDMatrix_RGB2PWM()
{
	uint8_t color_checker = GREEN_CHECK;
	uint8_t PWM_counter = 0;

	for(uint8_t x = 0; x < HEIGHT; x++){
		for(uint8_t y = 0; y < WIDTH; y++){

			uint8_t red_brightness = (uint8_t)((float)rgb_led_matrix[x][y].r * (matrix_brightness / MAX_BRIGHTNESS));
			uint8_t green_brightness = (uint8_t)((float)rgb_led_matrix[x][y].g * (matrix_brightness / MAX_BRIGHTNESS));
			uint8_t blue_brightness = (uint8_t)((float)rgb_led_matrix[x][y].b * (matrix_brightness / MAX_BRIGHTNESS));

			for(uint8_t i = 0; i < LED_BITS; i++){
				switch(color_checker){
				case GREEN_CHECK:
					pwm_led_matrix[(PWM_counter * LED_BITS) + i] = MATRIX_BIT_SYMBOL(green_brightness, i);
					break;
				case RED_CHECK:
					pwm_led_matrix[(PWM_counter * LED_BITS) + i] = MATRIX_BIT_SYMBOL(red_brightness, i);
					break;
				default:
					pwm_led_matrix[(PWM_counter * LED_BITS) + i] = MATRIX_BIT_SYMBOL(blue_brightness, i);
					break;
				}
				if(((i + 1) % COLOR_RESOLUTION == 0) && (i != 0)){
					color_checker++;
					if(color_checker > BLUE_CHECK){
						color_checker = GREEN_CHECK;
					}
				}
			}
			PWM_counter++;
		}
	}
}

static void LEDMatrix_reset(){
	FTM_stop(LED_MATRIX_FTM_CHNNL);
	timerStart(reset_tim_id, RESET_TICKS, TIM_MODE_SINGLESHOT, LEDMatrix_restart);
}

static void LEDMatrix_restart(){
	FTM_start(LED_MATRIX_FTM_CHNNL);
}

/******************************************************************************/
