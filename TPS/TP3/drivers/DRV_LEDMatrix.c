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
#include "DRV_LEDMatrix.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define LED_BITS 24

#define GREEN_CHECK 0
#define RED_CHECK	1
#define BLUE_CHECK	2

#define COLOR_RESOLUTION 8

#define LED_T0H 0.4
#define LED_T0L 0.85
#define LED_0T LED_T0H + LED_T0L

#define LED_T1H 0.8
#define LED_T1L 0.45
#define LED_1T LED_T1H + LED_T1L

#define FTM_MODULO 63

#define MATRIX0	(uint16_t) ((LED_T0H/LED_0T) * FTM_MODULO)
#define MATRIX1 (uint16_t) ((LED_T1H/LED_1T) * FTM_MODULO)

#define RESET_MODULO 3000

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef uint16_t PWM_Array_t[HEIGHT*WIDTH*LED_BITS];

/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

void LED2PWM();

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static FTM_Config_t ftm_config;
static FTM_Config_t ftm_reset_config;
static DMA_config_t dma_config;

static LEDMatrix_t * my_matrix;
static PWM_Array_t my_pwm_array;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void LEDMatrix_init(LEDMatrix_t * matrix)
{

	ftm_config.channel = FTM_Channel_0;
	ftm_config.mode = FTM_PWM;
	ftm_config.prescale = FTM_Prescale_1;
	ftm_config.CLK_source = FTM_SysCLK;
	ftm_config.PWM_logic = FTM_High;
	ftm_config.modulo = FTM_MODULO;
	ftm_config.PWM_DC = 0;
	ftm_config.active_low = false;
	ftm_config.DMA_on = true;
	ftm_config.interrupt_on = true;

	FTM_init(0, ftm_config);
	FTM_modifyDC(0, 0);

	ftm_reset_config = ftm_config;
	ftm_reset_config.modulo = RESET_MODULO;
	ftm_config.PWM_DC = 0;

	dma_config.source_buffer = my_pwm_matrix;
	dma_config.destination_buffer = FTM_getCounterPointer(0, FTM_Channel_0);
	dma_config.request_source = FTM0CH0;
	dma_config.source_offset = sizeof(uint16_t);
	dma_config.destination_offset = 0;
	dma_config.transfer_bytes = sizeof(uint16_t);
	dma_config.major_cycles = HEIGHT*WIDTH;
	dma_config.wrap_around = sizeof(my_pwm_array);

	FTM_start(0);
	FTM_modifyDC(0, 0);
	DMA_init(0, dma_config);

	my_matrix = matrix;
	LED2PWM();
}

void LEDMatrix_update()
{
	LED2PWM();
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void LED2PWM()
{
	uint8_t color_checker = GREEN_CHECK;

	uint8_t matrix_counter = 0;

	int i;

	for (i = 0; i < HEIGHT*WIDTH*LED_BITS; i++)
	{
		if (color_checker == GREEN_CHECK)
		{
			my_pwm_matrix[i] = ((*(my_matrix + matrix_counter).green >> i & 0x01) == 0) ? MATRIX0 : MATRIX1;
		}
		if (color_checker == RED_CHECK)
		{
			my_pwm_matrix[i] = ((*(my_matrix + matrix_counter).red >> i & 0x01) == 0) ? MATRIX0 : MATRIX1;
		}
		if (color_checker == BLUE_CHECK)
		{
			my_pwm_matrix[i] = ((*(my_matrix + matrix_counter).blue >> i & 0x01) == 0) ? MATRIX0 : MATRIX1;
		}

		i++;

		if ( i % COLOR_RESOLUTION == 0)
		{
			color_checker++;
			if (color_checker > BLUE_CHECK)
			{
				color_checker = GREEN_CHECK;
				matrix_counter++;
			}
		}
	}
}

/******************************************************************************/
