/***************************************************************************//**
  @file     DRV_WaveGen.c
  @brief    +Descripcion del archivo+
  @author   matia
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "DRV_WaveGen.h"
#include "PDRV_DAC.h"
#include "PDRV_DMA.h"
#include "PDRV_PIT.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define SINE_TABLE_LEN	256
#define MAX_AMPLITUDE	100.0

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

static void start_dma_transfer();

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static const uint16_t original_sine_table[] = {
		2048,2098,2148,2198,2248,2298,2348,2398,2447,2496,2545,2594,2642,2690,2737,2784,
		2831,2877,2923,2968,3013,3057,3100,3143,3185,3226,3267,3307,3346,3385,3423,3459,
		3495,3530,3565,3598,3630,3662,3692,3722,3750,3777,3804,3829,3853,3876,3898,3919,
		3939,3958,3975,3992,4007,4021,4034,4045,4056,4065,4073,4080,4085,4089,4093,4094,
		4095,4094,4093,4089,4085,4080,4073,4065,4056,4045,4034,4021,4007,3992,3975,3958,
		3939,3919,3898,3876,3853,3829,3804,3777,3750,3722,3692,3662,3630,3598,3565,3530,
		3495,3459,3423,3385,3346,3307,3267,3226,3185,3143,3100,3057,3013,2968,2923,2877,
		2831,2784,2737,2690,2642,2594,2545,2496,2447,2398,2348,2298,2248,2198,2148,2098,
		2048,1997,1947,1897,1847,1797,1747,1697,1648,1599,1550,1501,1453,1405,1358,1311,
		1264,1218,1172,1127,1082,1038,995,952,910,869,828,788,749,710,672,636,
		600,565,530,497,465,433,403,373,345,318,291,266,242,219,197,176,
		156,137,120,103,88,74,61,50,39,30,22,15,10,6,2,1,
		0,1,2,6,10,15,22,30,39,50,61,74,88,103,120,137,
		156,176,197,219,242,266,291,318,345,373,403,433,465,497,530,565,
		600,636,672,710,749,788,828,869,910,952,995,1038,1082,1127,1172,1218,
		1264,1311,1358,1405,1453,1501,1550,1599,1648,1697,1747,1797,1847,1897,1947,1997};

static uint16_t sine_table[] = {
		2048,2098,2148,2198,2248,2298,2348,2398,2447,2496,2545,2594,2642,2690,2737,2784,
		2831,2877,2923,2968,3013,3057,3100,3143,3185,3226,3267,3307,3346,3385,3423,3459,
		3495,3530,3565,3598,3630,3662,3692,3722,3750,3777,3804,3829,3853,3876,3898,3919,
		3939,3958,3975,3992,4007,4021,4034,4045,4056,4065,4073,4080,4085,4089,4093,4094,
		4095,4094,4093,4089,4085,4080,4073,4065,4056,4045,4034,4021,4007,3992,3975,3958,
		3939,3919,3898,3876,3853,3829,3804,3777,3750,3722,3692,3662,3630,3598,3565,3530,
		3495,3459,3423,3385,3346,3307,3267,3226,3185,3143,3100,3057,3013,2968,2923,2877,
		2831,2784,2737,2690,2642,2594,2545,2496,2447,2398,2348,2298,2248,2198,2148,2098,
		2048,1997,1947,1897,1847,1797,1747,1697,1648,1599,1550,1501,1453,1405,1358,1311,
		1264,1218,1172,1127,1082,1038,995,952,910,869,828,788,749,710,672,636,
		600,565,530,497,465,433,403,373,345,318,291,266,242,219,197,176,
		156,137,120,103,88,74,61,50,39,30,22,15,10,6,2,1,
		0,1,2,6,10,15,22,30,39,50,61,74,88,103,120,137,
		156,176,197,219,242,266,291,318,345,373,403,433,465,497,530,565,
		600,636,672,710,749,788,828,869,910,952,995,1038,1082,1127,1172,1218,
		1264,1311,1358,1405,1453,1501,1550,1599,1648,1697,1747,1797,1847,1897,1947,1997};


static float amplitude = MAX_AMPLITUDE;

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void start_dma_transfer();

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void initWaveGen(float amp, uint32_t freq){
	initDAC();

	DMA_config_t dma_config = {	sine_table,
								DAC_DMA_DEST,
								DMAALWAYS63,
								2,
								0,
								2,
								SINE_TABLE_LEN,
								SINE_TABLE_LEN*sizeof(sine_table[0])
								};

	DMA_init(DAC_DMA_CHNNL, dma_config);

	initPIT(0x02FAF07F);	// 1/50Mhz = 20ns   (1s/20ns)-1= 49,999,999 cycles or 0x02FAF07F);
	pitSetCallback(start_dma_transfer);

	wavegenSetAmp(amp);
	wavegenSetFreq(freq);
}

void wavegenSetFreq(uint32_t freq){
	pitSetFreq(freq * SINE_TABLE_LEN);
}

void wavegenSetAmp(float amp){
	amplitude = (amp > MAX_AMPLITUDE) ? MAX_AMPLITUDE : ((amp < 0) ? 0 : amp);
	for(uint16_t i = 0; i < SINE_TABLE_LEN; i++){
		sine_table[i] = (amplitude/MAX_AMPLITUDE) * (original_sine_table[i] - DAC_SCALE/2) + DAC_SCALE/2;
	}
}

float wavegenGetAmp(){
	return amplitude;
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
 
static void start_dma_transfer(){
	pitStartDMAtransfer(DAC_DMA_CHNNL);
}
