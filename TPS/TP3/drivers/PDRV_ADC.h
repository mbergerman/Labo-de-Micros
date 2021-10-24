
#ifndef SOURCES_TEMPLATE_ADC_H_
#define SOURCES_TEMPLATE_ADC_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

typedef enum
{
	ADC_b8,
	ADC_b12,
	ADC_b10,
	ADC_b16,
} ADCBits_t;

typedef enum
{
	ADC_c24,
	ADC_c16,
	ADC_c10,
	ADC_c6,
	ADC_c4,
} ADCCycles_t;

typedef enum
{
	ADC_t4,
	ADC_t8,
	ADC_t16,
	ADC_t32,
	ADC_t1,
} ADCTaps_t;

typedef enum
{
	ADC_mA,
	ADC_mB,
} ADCMux_t;

typedef enum
{
	input_clock,
	input_clock2,	// Input Clock/2
	input_clock4,	// Input Clock/4
	input_clock8,	// Input Clock/8

} ADCClock_Divide;

typedef enum
{
	bus_clock,
	bus_clock2, // BusClock/2
	alternate_clock,
	adack,

}ADC_adiclock;


typedef enum ADC_Type
{
	ADC0_t,
	ADC1_t,
} ADC_n;

typedef uint8_t ADCChannel_t; /* Channel 0-23 */
typedef uint16_t ADCData_t;

void ADC_Init (ADC_n, ADCBits_t, ADCCycles_t, ADCClock_Divide, ADCMux_t, ADCChannel_t);

void 		ADC_SetInterruptMode   (ADC_n, bool);
bool 		ADC_IsInterruptPending (ADC_n);
void 		ADC_ClearInterruptFlag (ADC_n);

void 		ADC_SetResolution 	   (ADC_n, ADCBits_t);
ADCBits_t 	ADC_GetResolution 	   (ADC_n);
void 		ADC_SetCycles	 	   (ADC_n, ADCCycles_t);

void 		ADC_StartConversion 	(ADC_n adc_, ADCChannel_t channel);

ADCData_t 	ADC_getData 		   (ADC_n);
ADCData_t 	ADC_getValue		   (ADC_n adc_);
bool 		ADC_Calibrate 		   (ADC_n);

#endif /* SOURCES_TEMPLATE_ADC_H_ */
