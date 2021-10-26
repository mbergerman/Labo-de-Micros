/***************************************************************************//**
  @file     PDRV_DAC.c
  @brief    +Descripcion del archivo+
  @author   matia
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "PDRV_DAC.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

//#define DAC_USE_VREF

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



/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/


/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

// +ej: static int temperaturas_actuales[4];+


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void initDAC(void)
{
	static bool alreadyInit = false;
	if (alreadyInit) return;

	SIM->SCGC2 |= SIM_SCGC2_DAC0_MASK;

#ifdef DAC_USE_VREF
	SIM->SCGC4 |= SIM_SCGC4_VREF_MASK;

	//The DAC selects DACREF_1 as the reference voltage
	DAC0->C0 = (DAC_C0_DACEN_MASK | DAC_C0_DACTRGSEL_MASK) | & (~(DAC_C0_DACRFS_MASK));

	VREF->SC = VREF_SC_VREFEN_MASK | VREF_SC_MODE_LV(0b01) | VREF_SC_REGEN_MASK | VREF_SC_ICOMPEN_MASK;
	VREF->TRM = VREF_TRM_TRIM(0b111111) | VREF_TRM_CHOPEN_MASK;
#else
	DAC0->C0 = (DAC_C0_DACEN_MASK | DAC_C0_DACTRGSEL_MASK) | DAC_C0_DACRFS_MASK;
#endif

}

void dacEnableDMA(){
	DAC0->C1 = DAC_C1_DMAEN_MASK;	// Enable DMA
}

void dacWrite(uint16_t data)
{
	DAC0->DAT[0].DATL = DAC_DATL_DATA0(data);
	DAC0->DAT[0].DATH = DAC_DATH_DATA1(data >> 8);
}

void dacSetAmplitude(uint8_t amp){
#ifdef DAC_USE_VREF
	VREF->TRM &= (~VREF_TRM_TRIM_MASK) | VREF_TRM_TRIM( amp>>2 );
#endif
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

