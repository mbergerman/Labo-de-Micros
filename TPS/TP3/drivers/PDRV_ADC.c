
#include <PDRV_ADC.h>
#include "MK64F12.h"
#include "hardware.h"
#include "DRV_Timers.h"
#define TWO_POW_NUM_OF_CAL (1 << 4)

bool ADC_interrupt[2] = {false, false};
typedef ADC_Type *ADC_t;
static ADCData_t current_value[2] = {0, 0};
static ADCChannel_t channels[2]= {0, 0};

void ADC_Init (ADC_n adc_,ADCBits_t resolution, ADCCycles_t cycles, ADCClock_Divide divide_select, ADCMux_t mux, ADCChannel_t channel){

	ADC_t adc = (adc_ == ADC0_t) ? ADC0 : ADC1;

	if(adc_ == ADC0_t)
	{
		SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK;

		NVIC_EnableIRQ(ADC0_IRQn);
	}
	else {

		SIM->SCGC3 |= SIM_SCGC3_ADC1_MASK;

		ADC1->CFG1 = ADC_CFG1_ADIV(divide_select);

		NVIC_EnableIRQ(ADC1_IRQn);
	}

	// Usamos siempre el clock del bus
	adc->CFG1 = (adc->CFG1 & ~ADC_CFG1_ADICLK_MASK) | ADC_CFG1_ADICLK(bus_clock) ;
	adc->CFG1 = ADC_CFG1_ADIV(divide_select);
	ADC_SetResolution(adc_, resolution);
	ADC_SetCycles(adc_, cycles);
	adc->CFG2 = (adc->CFG2 & ~ADC_CFG2_MUXSEL_MASK) | ADC_CFG2_MUXSEL(mux);
	channels[adc_] = channel;
}

void ADC_SetResolution (ADC_n adc_, ADCBits_t bits)
{
	ADC_t adc = (adc_==ADC0_t) ? ADC0 : ADC1;
	adc->CFG1 = (adc->CFG1 & ~ADC_CFG1_MODE_MASK) | ADC_CFG1_MODE(bits);
}

void ADC_SetCycles (ADC_n adc_, ADCCycles_t cycles)
{
	ADC_t adc = (adc_==ADC0_t) ? ADC0 : ADC1;
	if (cycles & ~ADC_CFG2_ADLSTS_MASK) // Solo entra al if cuando clycles es 4 en el enum, o sea cuando vale c4, "Short Sample Time"
	{
		adc->CFG1 &= ~ADC_CFG1_ADLSMP_MASK; //Acá se setea que va a ser short
	}
	else //"Long sample Time", mínimo más de 6
	{
		adc->CFG1 |= ADC_CFG1_ADLSMP_MASK;	//Acá se seta que va a ser long
		adc->CFG2 = (adc->CFG2 & ~ADC_CFG2_ADLSTS_MASK) | ADC_CFG2_ADLSTS(cycles);
	}
}

void ADC_SetInterruptMode (ADC_n adc_, bool mode)
{
	if (adc_ == ADC0_t)
		ADC_interrupt[0] = mode;
	else if (adc_ == ADC1_t)
		ADC_interrupt[1] = mode;
}

bool ADC_IsInterruptPending (ADC_n adc_)
{
	ADC_t adc = (adc_==ADC0_t) ? ADC0 : ADC1;
	return adc->SC1[0] & ADC_SC1_COCO_MASK;
}


void ADC_ClearInterruptFlag (ADC_n adc_)
{
	ADC_t adc = (adc_==ADC0_t) ? ADC0 : ADC1;
	adc->SC1[0] = 0x00;
}

ADCBits_t ADC_GetResolution (ADC_n adc_)
{
	ADC_t adc = (adc_==ADC0_t) ? ADC0 : ADC1;
	return adc->CFG1 & ADC_CFG1_MODE_MASK;
}

void ADC_StartConversion (ADC_n adc_, ADCChannel_t channel)
{
	ADC_t adc = (adc_== ADC0_t) ? ADC0 : ADC1;
	adc->SC1[0] = ADC_SC1_AIEN(ADC_interrupt[adc_]) | ADC_SC1_ADCH(channel);
}


ADCData_t ADC_getData (ADC_n adc_)
{
	ADC_t adc = (adc_==ADC0_t) ? ADC0 : ADC1;
	return adc->R[0];
}

ADCData_t ADC_getValue(ADC_n adc_){
	return current_value[adc_];
}

bool ADC_Calibrate (ADC_n adc_)
{
	ADC_t adc = (adc_==ADC0_t) ? ADC0 : ADC1;
	int32_t  Offset		= 0;
	uint32_t Minus	[7] = {0,0,0,0,0,0,0};
	uint32_t Plus	[7] = {0,0,0,0,0,0,0};
	uint8_t  i;
	uint32_t scr3;

	/// SETUP
	adc->SC1[0] = 0x1F;
	scr3 = adc->SC3;
	adc->SC3 &= (ADC_SC3_AVGS(0x03) | ADC_SC3_AVGE_MASK);

	/// INITIAL CALIBRATION
	adc->SC3 &= ~ADC_SC3_CAL_MASK;
	adc->SC3 |=  ADC_SC3_CAL_MASK;
	while (!(adc->SC1[0] & ADC_SC1_COCO_MASK));
	if (adc->SC3 & ADC_SC3_CALF_MASK)
	{
		adc->SC3 |= ADC_SC3_CALF_MASK;
		return false;
	}
	adc->PG  = (0x8000 | ((adc->CLP0+adc->CLP1+adc->CLP2+adc->CLP3+adc->CLP4+adc->CLPS) >> (1 + TWO_POW_NUM_OF_CAL)));
	adc->MG  = (0x8000 | ((adc->CLM0+adc->CLM1+adc->CLM2+adc->CLM3+adc->CLM4+adc->CLMS) >> (1 + TWO_POW_NUM_OF_CAL)));

	// FURTHER CALIBRATIONS
	for (i = 0; i < TWO_POW_NUM_OF_CAL; i++)
	{
		adc->SC3 &= ~ADC_SC3_CAL_MASK;
		adc->SC3 |=  ADC_SC3_CAL_MASK;
		while (!(adc->SC1[0] & ADC_SC1_COCO_MASK));
		if (adc->SC3 & ADC_SC3_CALF_MASK)
		{
			adc->SC3 |= ADC_SC3_CALF_MASK;
			return 1;
		}
		Offset += (short)adc->OFS;
		Plus[0] += (unsigned long)adc->CLP0;
		Plus[1] += (unsigned long)adc->CLP1;
		Plus[2] += (unsigned long)adc->CLP2;
		Plus[3] += (unsigned long)adc->CLP3;
		Plus[4] += (unsigned long)adc->CLP4;
		Plus[5] += (unsigned long)adc->CLPS;
		Plus[6] += (unsigned long)adc->CLPD;
		Minus[0] += (unsigned long)adc->CLM0;
		Minus[1] += (unsigned long)adc->CLM1;
		Minus[2] += (unsigned long)adc->CLM2;
		Minus[3] += (unsigned long)adc->CLM3;
		Minus[4] += (unsigned long)adc->CLM4;
		Minus[5] += (unsigned long)adc->CLMS;
		Minus[6] += (unsigned long)adc->CLMD;
	}
	adc->OFS = (Offset >> TWO_POW_NUM_OF_CAL);
	adc->PG  = (0x8000 | ((Plus[0] +Plus[1] +Plus[2] +Plus[3] +Plus[4] +Plus[5] ) >> (1 + TWO_POW_NUM_OF_CAL)));
	adc->MG  = (0x8000 | ((Minus[0]+Minus[1]+Minus[2]+Minus[3]+Minus[4]+Minus[5]) >> (1 + TWO_POW_NUM_OF_CAL)));
	adc->CLP0 = (Plus[0] >> TWO_POW_NUM_OF_CAL);
	adc->CLP1 = (Plus[1] >> TWO_POW_NUM_OF_CAL);
	adc->CLP2 = (Plus[2] >> TWO_POW_NUM_OF_CAL);
	adc->CLP3 = (Plus[3] >> TWO_POW_NUM_OF_CAL);
	adc->CLP4 = (Plus[4] >> TWO_POW_NUM_OF_CAL);
	adc->CLPS = (Plus[5] >> TWO_POW_NUM_OF_CAL);
	adc->CLPD = (Plus[6] >> TWO_POW_NUM_OF_CAL);
	adc->CLM0 = (Minus[0] >> TWO_POW_NUM_OF_CAL);
	adc->CLM1 = (Minus[1] >> TWO_POW_NUM_OF_CAL);
	adc->CLM2 = (Minus[2] >> TWO_POW_NUM_OF_CAL);
	adc->CLM3 = (Minus[3] >> TWO_POW_NUM_OF_CAL);
	adc->CLM4 = (Minus[4] >> TWO_POW_NUM_OF_CAL);
	adc->CLMS = (Minus[5] >> TWO_POW_NUM_OF_CAL);
	adc->CLMD = (Minus[6] >> TWO_POW_NUM_OF_CAL);

	/// UN-SETUP
	adc->SC3 = scr3;

	return true;
}

__ISR__ ADC0_IRQHandler(void){
	ADCData_t data = ADC_getData(ADC0_t);
	if(data != current_value[ADC0_t]){
		current_value[ADC0_t] = data;
	}
	ADC_StartConversion (ADC0_t, channels[ADC0_t]);
}

__ISR__ ADC1_IRQHandler(void){
	ADCData_t data = ADC_getData(ADC1_t);
	if(data != current_value[ADC1_t]){
		current_value[ADC1_t] = data;
	}
	ADC_StartConversion (ADC1_t, channels[ADC1_t]);
}
