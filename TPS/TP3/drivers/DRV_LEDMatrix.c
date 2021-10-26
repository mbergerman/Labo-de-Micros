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

#define PMW_ARRAY_LEN	HEIGHT*WIDTH*LED_BITS + 1

#define MAX_BR 100.0

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

void LEDMatrix_RGB2PWM();
void refresh();

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static FTM_Config_t ftm_config;
static DMA_config_t dma_config;

static PWM_Array_t pwm_led_matrix;
static LEDMatrix_t rgb_led_matrix ={{{0, 0, 50},{0, 0, 50},{0, 0, 50},{0, 0, 50},{0, 0, 50},{0, 0, 50},{0, 0, 50},{0, 0, 50}},
									{{0, 0, 50},{0, 0, 50},{0, 0, 50},{0, 0, 50},{0, 0, 50},{0, 0, 50},{0, 0, 50},{0, 0, 50}},
									{{0, 0, 50},{0, 0, 50},{0, 0, 50},{0, 0, 50},{0, 0, 50},{0, 0, 50},{0, 0, 50},{0, 0, 50}},
									{{0, 0, 50},{0, 0, 50},{0, 0, 50},{0, 50, 0},{0, 50, 0},{0, 0, 50},{0, 0, 50},{0, 0, 50}},
									{{0, 0, 50},{0, 0, 50},{0, 0, 50},{0, 50, 0},{0, 50, 0},{0, 0, 50},{0, 0, 50},{0, 0, 50}},
									{{0, 0, 50},{0, 0, 50},{0, 0, 50},{0, 0, 50},{0, 0, 50},{0, 0, 50},{0, 0, 50},{0, 0, 50}},
									{{0, 0, 50},{0, 0, 50},{0, 0, 50},{0, 0, 50},{0, 0, 50},{0, 0, 50},{0, 0, 50},{0, 0, 50}},
									{{0, 0, 50},{0, 0, 50},{0, 0, 50},{0, 0, 50},{0, 0, 50},{0, 0, 50},{0, 0, 50},{0, 0, 50}}};

static tim_id_t refresh_id;

static uint8_t refresh_rate = 5;
static bool refreshing = true;

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

	refresh_id = timerGetId();

	//timerStart(refresh_id, TIMER_MS2TICKS((uint8_t)(refresh_rate/2.0)), TIM_MODE_PERIODIC, refresh);

	LEDMatrix_RGB2PWM();

	FTM_init(0, ftm_config);
	DMA_init(0, dma_config);

	FTM_start(0);
}

void LEDMatrix_updateLED(color_t led, uint8_t height, uint8_t width){
	rgb_led_matrix[height][width] = led;
	LEDMatrix_RGB2PWM();
}

void LEDMatrix_updateBrightness(uint8_t brightness)
{
	timerStop(refresh_id);
	refresh_rate = (uint8_t) ((MAX_BR - brightness) / MAX_BR);
	timerStart(refresh_id, TIMER_MS2TICKS((uint8_t)(refresh_rate/2.0)), TIM_MODE_PERIODIC, refresh);
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void LEDMatrix_RGB2PWM()
{
	uint8_t color_checker = GREEN_CHECK;
	uint8_t PWM_counter = 0;

	// Last value is the reset condition
	pwm_led_matrix[PMW_ARRAY_LEN-1] = MATRIXR;

	for(uint8_t x = 0; x < HEIGHT; x++){
		for(uint8_t y = 0; y < WIDTH; y++){
			for(uint8_t i = 0; i < LED_BITS; i++){
				switch(color_checker){
				case GREEN_CHECK:
					pwm_led_matrix[(PWM_counter * LED_BITS) + i] = ((rgb_led_matrix[x][y].g >> i%COLOR_RESOLUTION & 0x01) == 0) ? MATRIX0 : MATRIX1;
					break;
				case RED_CHECK:
					pwm_led_matrix[(PWM_counter * LED_BITS) + i] = ((rgb_led_matrix[x][y].r >> i%COLOR_RESOLUTION & 0x01) == 0) ? MATRIX0 : MATRIX1;
					break;
				default:
					pwm_led_matrix[(PWM_counter * LED_BITS) + i] = ((rgb_led_matrix[x][y].b >> i%COLOR_RESOLUTION & 0x01) == 0) ? MATRIX0 : MATRIX1;
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

void refresh()
{
	FTM_stop(0);
	if (!refreshing)
	{
		FTM_start(0);
		refreshing = true;
	}
	refreshing = false;
}

/******************************************************************************/
