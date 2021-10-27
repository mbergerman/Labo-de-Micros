/***************************************************************************//**
  @file     DRV_Sensor.c
  @brief    +Descripcion del archivo+
  @author   matia
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <DRV_Sensors.h>
#include "DRV_Timers.h"
#include "PDRV_ADC.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define CONVERSION_TICKS	TIMER_MS2TICKS(10)

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/



/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/



/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void ADC_StartConversion_0();
static void ADC_StartConversion_1();

static void adc_callback0();
static void adc_callback1();


/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/



/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static tim_id_t conversion_tim_id[2];


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void initSensor(uint8_t num){
	if(num == 0){
		ADC_Init(ADC0_t, ADC_b8, ADC_c4, input_clock, ADC_mA, 0);
		ADC_SetInterruptMode(ADC0_t, true);
		ADC_SetInterruptCallback(ADC0_t, adc_callback0);
	}else{
		ADC_Init(ADC1_t, ADC_b8, ADC_c4, input_clock, ADC_mA, 0);
		ADC_SetInterruptMode(ADC1_t, true);
		ADC_SetInterruptCallback(ADC1_t, adc_callback1);
	}

	conversion_tim_id[num] = timerGetId();
}

void sensorStartReading(uint8_t num){
	if(num == 0){
		ADC_StartConversion(ADC0_t, 0);
	}else{
		ADC_StartConversion(ADC1_t, 0);
	}
}

sensorData_t sensorGetValue(uint8_t num){
	if(num == 0){
		return ADC_getValue(ADC0_t);
	}else{
		return ADC_getValue(ADC1_t);
	}
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void ADC_StartConversion_0(){
	ADC_StartConversion(ADC0_t, 0);
}

static void ADC_StartConversion_1(){
	ADC_StartConversion(ADC1_t, 0);
}

static void adc_callback0(){
	timerStart(conversion_tim_id[0], CONVERSION_TICKS, TIM_MODE_SINGLESHOT, ADC_StartConversion_0);
}

static void adc_callback1(){
	timerStart(conversion_tim_id[1], CONVERSION_TICKS, TIM_MODE_SINGLESHOT, ADC_StartConversion_1);
}
