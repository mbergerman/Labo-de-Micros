/***************************************************************************//**
  @file     PDRV_FTM.c
  @brief    SPI driver.
  @author   Grupo 1
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "PDRV_FTM.h"
#include "MK64F12.h"
#include "hardware.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define MAX_FTM 4

//EXTERNAL CLOCK IN PORT, PINOUT, AND MUX ALT
#define FTM_CLKIN0_PORT 1
#define FTM_CLKIN1_PORT 1

#define FTM_CLKIN0_PIN ((FTM_CLKIN0_PORT<<5) + 16)   // PTB16
#define FTM_CLKIN1_PIN ((FTM_CLKIN1_PORT<<5) + 17)   // PTB17

#define FTM_CLKIN0_ALT 4
#define FTM_CLKIN1_ALT 4

//EXTERNAL CLOCK SELECT MACROS
#define FTM_CLKIN_PORT(clk_in)     ((clk_in == 0)? FTM_CLKIN0_PORT: FTM_CLKIN1_PORT)
#define FTM_CLKIN_PIN(clk_in)     ((clk_in == 0)? FTM_CLKIN0_PIN: FTM_CLKIN1_PIN)
#define FTM_CLKIN_ALT(clk_in)     ((clk_in == 0)? FTM_CLKIN0_ALT: FTM_CLKIN1_ALT)

//FTM0 PORT, PINOUT, AND MUX ALT
#define FTM0_CH0_PORT 2
#define FTM0_CH1_PORT 2
#define FTM0_CH2_PORT 2
#define FTM0_CH3_PORT 2
#define FTM0_CH4_PORT 3
#define FTM0_CH5_PORT 0
#define FTM0_CH6_PORT 0
#define FTM0_CH7_PORT 0

#define FTM0_CH0_PIN ((FTM0_CH0_PORT<<5) + 1)   // PTC1
#define FTM0_CH1_PIN ((FTM0_CH1_PORT<<5) + 2)   // PTC2
#define FTM0_CH2_PIN ((FTM0_CH2_PORT<<5) + 3)   // PTC3
#define FTM0_CH3_PIN ((FTM0_CH3_PORT<<5) + 4)   // PTC4
#define FTM0_CH4_PIN ((FTM0_CH4_PORT<<5) + 4)   // PTD4
#define FTM0_CH5_PIN ((FTM0_CH5_PORT<<5) + 0)   // PTA0
#define FTM0_CH6_PIN ((FTM0_CH6_PORT<<5) + 1)   // PTA1
#define FTM0_CH7_PIN ((FTM0_CH7_PORT<<5) + 2)   // PTA2

#define FTM0_CH0_ALT 4
#define FTM0_CH1_ALT 4
#define FTM0_CH2_ALT 4
#define FTM0_CH3_ALT 4
#define FTM0_CH4_ALT 4
#define FTM0_CH5_ALT 3
#define FTM0_CH6_ALT 3
#define FTM0_CH7_ALT 3

//FTM1 PORT, PINOUT, AND MUX ALT
#define FTM1_CH0_PORT 1
#define FTM1_CH1_PORT 1

#define FTM1_CH0_PIN ((FTM1_CH0_PORT<<5) + 0)   // PTB0
#define FTM1_CH1_PIN ((FTM1_CH1_PORT<<5) + 1)   // PTB1

#define FTM1_CH0_ALT 3
#define FTM1_CH1_ALT 3

//FTM2 PORT, PINOUT, AND MUX ALT
#define FTM2_CH0_PORT 1
#define FTM2_CH1_PORT 1

#define FTM2_CH0_PIN ((FTM2_CH0_PORT<<5) + 18)   // PTB18
#define FTM2_CH1_PIN ((FTM2_CH1_PORT<<5) + 19)   // PTB19

#define FTM2_CH0_ALT 3
#define FTM2_CH1_ALT 3

//FTM3 PORT, PINOUT, AND MUX ALT
#define FTM3_CH0_PORT 3
#define FTM3_CH1_PORT 3
#define FTM3_CH2_PORT 3
#define FTM3_CH3_PORT 3

#define FTM3_CH0_PIN ((FTM3_CH0_PORT<<5) + 0)   // PTD0
#define FTM3_CH1_PIN ((FTM3_CH1_PORT<<5) + 1)   // PTD1
#define FTM3_CH2_PIN ((FTM3_CH2_PORT<<5) + 2)   // PTD2
#define FTM3_CH3_PIN ((FTM3_CH3_PORT<<5) + 3)   // PTD3

#define FTM3_CH0_ALT 4
#define FTM3_CH1_ALT 4
#define FTM3_CH2_ALT 4
#define FTM3_CH3_ALT 4

//FTM PORT SELECT MACROS
#define FTM_CH0_PORT(ftm_port)     ((ftm_port == 0)? FTM0_CH0_PORT: \
                                    ((ftm_port == 1)? FTM1_CH0_PORT: \
                                     ((ftm_port == 1)? FTM2_CH0_PORT: FTM3_CH0_PORT)))

#define FTM_CH1_PORT(ftm_port)     ((ftm_port == 0)? FTM0_CH1_PORT: \
                                    ((ftm_port == 1)? FTM1_CH1_PORT: \
                                     ((ftm_port == 1)? FTM2_CH1_PORT: FTM3_CH1_PORT)))

#define FTM_CH2_PORT(ftm_port)     ((ftm_port == 0)? FTM0_CH2_PORT: FTM3_CH2_PORT)

#define FTM_CH3_PORT(ftm_port)     ((ftm_port == 0)? FTM0_CH3_PORT: FTM3_CH3_PORT)
#define FTM_CH4_PORT(ftm_port)	   FTM0_CH4_PORT
#define FTM_CH5_PORT(ftm_port)	   FTM0_CH5_PORT
#define FTM_CH6_PORT(ftm_port)	   FTM0_CH6_PORT
#define FTM_CH7_PORT(ftm_port)	   FTM0_CH7_PORT

//FTM PIN SELECT MACROS
#define FTM_CH0_PIN(ftm_port)     ((ftm_port == 0)? FTM0_CH0_PIN: \
                                    ((ftm_port == 1)? FTM1_CH0_PIN: \
                                     ((ftm_port == 1)? FTM2_CH0_PIN: FTM3_CH0_PIN)))

#define FTM_CH1_PIN(ftm_port)     ((ftm_port == 0)? FTM0_CH1_PIN: \
                                    ((ftm_port == 1)? FTM1_CH1_PIN: \
                                     ((ftm_port == 1)? FTM2_CH1_PIN: FTM3_CH1_PIN)))

#define FTM_CH2_PIN(ftm_port)     ((ftm_port == 0)? FTM0_CH2_PIN: FTM3_CH2_PIN)

#define FTM_CH3_PIN(ftm_port)     ((ftm_port == 0)? FTM0_CH3_PIN: FTM3_CH3_PIN)
#define FTM_CH4_PIN(ftm_port)	   FTM0_CH4_PIN
#define FTM_CH5_PIN(ftm_port)	   FTM0_CH5_PIN
#define FTM_CH6_PIN(ftm_port)	   FTM0_CH6_PIN
#define FTM_CH7_PIN(ftm_port)	   FTM0_CH7_PIN

//FTM ALT SELECT MACROS
#define FTM_CH0_ALT(ftm_port)     ((ftm_port == 0)? FTM0_CH0_ALT: \
                                    ((ftm_port == 1)? FTM1_CH0_ALT: \
                                     ((ftm_port == 1)? FTM2_CH0_ALT: FTM3_CH0_ALT)))

#define FTM_CH1_ALT(ftm_port)     ((ftm_port == 0)? FTM0_CH1_ALT: \
                                    ((ftm_port == 1)? FTM1_CH1_ALT: \
                                     ((ftm_port == 1)? FTM2_CH1_ALT: FTM3_CH1_ALT)))

#define FTM_CH2_ALT(ftm_port)     ((ftm_port == 0)? FTM0_CH2_ALT: FTM3_CH2_ALT)

#define FTM_CH3_ALT(ftm_port)     ((ftm_port == 0)? FTM0_CH3_ALT: FTM3_CH3_ALT)
#define FTM_CH4_ALT(ftm_port)	   FTM0_CH4_ALT
#define FTM_CH5_ALT(ftm_port)	   FTM0_CH5_ALT
#define FTM_CH6_ALT(ftm_port)	   FTM0_CH6_ALT
#define FTM_CH7_ALT(ftm_port)	   FTM0_CH7_ALT


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static FTM_Config_t ftm_config[MAX_FTM];
static PORT_Type* const PORT_PTRS[] = PORT_BASE_PTRS;
static FTM_Type* const FTM_PTRS[] = FTM_BASE_PTRS;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void FTM_init(uint8_t FTM, FTM_Config_t config)
{
	// Save configuration
	ftm_config[FTM] = config;

    // Enable FTM register write
    FTM_PTRS[FTM]->MODE |= FTM_MODE_WPDIS(1);

	// Clock gating and NVIC
    switch(FTM)
    {
        case 0:
        	SIM->SCGC6 |= SIM_SCGC6_FTM0_MASK;
        	NVIC_EnableIRQ(FTM0_IRQn);
        	FTM0->PWMLOAD = FTM_PWMLOAD_LDOK_MASK | 0x0F;
            break;
        case 1:
        	SIM->SCGC6 |= SIM_SCGC6_FTM1_MASK;
        	NVIC_EnableIRQ(FTM1_IRQn);
        	FTM1->PWMLOAD = FTM_PWMLOAD_LDOK_MASK | 0x0F;
            break;
        case 2:
        	SIM->SCGC6 |= SIM_SCGC6_FTM2_MASK;
			SIM->SCGC3 |= SIM_SCGC3_FTM2_MASK;
			NVIC_EnableIRQ(FTM2_IRQn);
			FTM2->PWMLOAD = FTM_PWMLOAD_LDOK_MASK | 0x0F;
			break;
        case 3:
        	SIM->SCGC3 |= SIM_SCGC3_FTM3_MASK;
        	NVIC_EnableIRQ(FTM3_IRQn);
        	FTM3->PWMLOAD = FTM_PWMLOAD_LDOK_MASK | 0x0F;
        	break;
    }

    //init channel GPIO and polarity
    switch(config.channel)
    {
    	case FTM_Channel_0:
        	PORT_PTRS[FTM_CH0_PORT(FTM)]->PCR[FTM_CH0_PIN(FTM)] = 0x0; //Clear
        	PORT_PTRS[FTM_CH0_PORT(FTM)]->PCR[FTM_CH0_PIN(FTM)] |= PORT_PCR_DSE(1);
        	PORT_PTRS[FTM_CH0_PORT(FTM)]->PCR[FTM_CH0_PIN(FTM)] |= PORT_PCR_MUX(FTM_CH0_ALT(FTM));

        	FTM_PTRS[FTM]->POL = (FTM_PTRS[FTM]->POL & ~FTM_POL_POL0_MASK) | FTM_POL_POL0(config.active_low);
        break;
    	case FTM_Channel_1:
			PORT_PTRS[FTM_CH1_PORT(FTM)]->PCR[FTM_CH1_PIN(FTM)] = 0x0; //Clear
			PORT_PTRS[FTM_CH1_PORT(FTM)]->PCR[FTM_CH1_PIN(FTM)] |= PORT_PCR_DSE(1);
			PORT_PTRS[FTM_CH1_PORT(FTM)]->PCR[FTM_CH1_PIN(FTM)] |= PORT_PCR_MUX(FTM_CH1_ALT(FTM));

        	FTM_PTRS[FTM]->POL = (FTM_PTRS[FTM]->POL & ~FTM_POL_POL1_MASK) | FTM_POL_POL1(config.active_low);
		break;
    	case FTM_Channel_2:
			PORT_PTRS[FTM_CH2_PORT(FTM)]->PCR[FTM_CH2_PIN(FTM)] = 0x0; //Clear
			PORT_PTRS[FTM_CH2_PORT(FTM)]->PCR[FTM_CH2_PIN(FTM)] |= PORT_PCR_DSE(1);
			PORT_PTRS[FTM_CH2_PORT(FTM)]->PCR[FTM_CH2_PIN(FTM)] |= PORT_PCR_MUX(FTM_CH2_ALT(FTM));

        	FTM_PTRS[FTM]->POL = (FTM_PTRS[FTM]->POL & ~FTM_POL_POL2_MASK) | FTM_POL_POL2(config.active_low);
		break;
    	case FTM_Channel_3:
			PORT_PTRS[FTM_CH3_PORT(FTM)]->PCR[FTM_CH3_PIN(FTM)] = 0x0; //Clear
			PORT_PTRS[FTM_CH3_PORT(FTM)]->PCR[FTM_CH3_PIN(FTM)] |= PORT_PCR_DSE(1);
			PORT_PTRS[FTM_CH3_PORT(FTM)]->PCR[FTM_CH3_PIN(FTM)] |= PORT_PCR_MUX(FTM_CH3_ALT(FTM));

        	FTM_PTRS[FTM]->POL = (FTM_PTRS[FTM]->POL & ~FTM_POL_POL3_MASK) | FTM_POL_POL3(config.active_low);
		break;
    	case FTM_Channel_4:
			PORT_PTRS[FTM_CH4_PORT(FTM)]->PCR[FTM_CH4_PIN(FTM)] = 0x0; //Clear
			PORT_PTRS[FTM_CH4_PORT(FTM)]->PCR[FTM_CH4_PIN(FTM)] |= PORT_PCR_DSE(1);
			PORT_PTRS[FTM_CH4_PORT(FTM)]->PCR[FTM_CH4_PIN(FTM)] |= PORT_PCR_MUX(FTM_CH4_ALT(FTM));

        	FTM_PTRS[FTM]->POL = (FTM_PTRS[FTM]->POL & ~FTM_POL_POL4_MASK) | FTM_POL_POL4(config.active_low);
		break;
    	case FTM_Channel_5:
			PORT_PTRS[FTM_CH5_PORT(FTM)]->PCR[FTM_CH5_PIN(FTM)] = 0x0; //Clear
			PORT_PTRS[FTM_CH5_PORT(FTM)]->PCR[FTM_CH5_PIN(FTM)] |= PORT_PCR_DSE(1);
			PORT_PTRS[FTM_CH5_PORT(FTM)]->PCR[FTM_CH5_PIN(FTM)] |= PORT_PCR_MUX(FTM_CH5_ALT(FTM));

        	FTM_PTRS[FTM]->POL = (FTM_PTRS[FTM]->POL & ~FTM_POL_POL5_MASK) | FTM_POL_POL5(config.active_low);
		break;
    	case FTM_Channel_6:
			PORT_PTRS[FTM_CH6_PORT(FTM)]->PCR[FTM_CH6_PIN(FTM)] = 0x0; //Clear
			PORT_PTRS[FTM_CH6_PORT(FTM)]->PCR[FTM_CH6_PIN(FTM)] |= PORT_PCR_DSE(1);
			PORT_PTRS[FTM_CH6_PORT(FTM)]->PCR[FTM_CH6_PIN(FTM)] |= PORT_PCR_MUX(FTM_CH6_ALT(FTM));

        	FTM_PTRS[FTM]->POL = (FTM_PTRS[FTM]->POL & ~FTM_POL_POL6_MASK) | FTM_POL_POL6(config.active_low);
		break;
    	case FTM_Channel_7:
			PORT_PTRS[FTM_CH7_PORT(FTM)]->PCR[FTM_CH7_PIN(FTM)] = 0x0; //Clear
			PORT_PTRS[FTM_CH7_PORT(FTM)]->PCR[FTM_CH7_PIN(FTM)] |= PORT_PCR_DSE(1);
			PORT_PTRS[FTM_CH7_PORT(FTM)]->PCR[FTM_CH7_PIN(FTM)] |= PORT_PCR_MUX(FTM_CH7_ALT(FTM));

        	FTM_PTRS[FTM]->POL = (FTM_PTRS[FTM]->POL & ~FTM_POL_POL7_MASK) | FTM_POL_POL7(config.active_low);
		break;
    }

    // FTM Register config

    // Set Mode
    FTM_PTRS[FTM]->CONTROLS[config.channel].CnSC = (FTM_PTRS[FTM]->CONTROLS[config.channel].CnSC & ~(FTM_CnSC_MSB_MASK | FTM_CnSC_MSA_MASK)) |
    			                      (FTM_CnSC_MSB((config.mode >> 1) & 0X01) | FTM_CnSC_MSA((config.mode >> 0) & 0X01));
    // Set CLK Source
    FTM_PTRS[FTM]->SC = (FTM_PTRS[FTM]->SC & ~FTM_SC_CLKS_MASK) | FTM_SC_CLKS(config.CLK_source);
    if (CLK_source == FTM_ExtCLK)
    {
		// Set external clk gpio
    	PORT_PTRS[FTM_CLKIN_PORT(config.ext_clock)]->PCR[FTM_CLKIN_PIN(config.ext_clock)] = 0x0; //Clear
		PORT_PTRS[FTM_CLKIN_PORT(config.ext_clock)]->PCR[FTM_CLKIN_PIN(config.ext_clock)] |= PORT_PCR_DSE(1);
		PORT_PTRS[FTM_CLKIN_PORT(config.ext_clock)]->PCR[FTM_CLKIN_PIN(config.ext_clock)] |= PORT_PCR_MUX(FTM_CLKIN_ALT(config.ext_clock));
    }
    // Set Prescaler
    FTM_PTRS[FTM]->SC = (FTM_PTRS[FTM]->SC & ~FTM_SC_PS_MASK) | FTM_SC_PS(config.prescale);
    // Set Modulo and Counter
    FTM_PTRS[FTM]->CNTIN = 0x0;
    FTM_PTRS[FTM]->CNT = 0x0;
    FTM_PTRS[FTM]->MOD = FTM_MOD_MOD(config.modulo);
    switch(config.mode)
    {
    	case FTM_InputCapture:
    		FTM_PTRS[FTM]->CONTROLS[config.channel].CnSC =
    						(FTM_PTRS[FTM]->CONTROLS[config.channel].CnSC & ~(FTM_CnSC_ELSB_MASK | FTM_CnSC_ELSA_MASK)) |
    						(FTM_CnSC_ELSB((config.IC_edge >> 1) & 0X01) | FTM_CnSC_ELSA((config.IC_edge >> 0) & 0X01));
    	break;
    	case FTM_OutputCompare:
    		FTM_PTRS[FTM]->CONTROLS[config.channel].CnSC =
    						(FTM_PTRS[FTM]->CONTROLS[config.channel].CnSC & ~(FTM_CnSC_ELSB_MASK | FTM_CnSC_ELSA_MASK)) |
    		    			(FTM_CnSC_ELSB((config.OC_effect >> 1) & 0X01) | FTM_CnSC_ELSA((config.OC_effect >> 0) & 0X01));
    	break;
    	case FTM_PWM:
    		FTM_PTRS[FTM]->CONTROLS[config.channel].CnSC =
    						(FTM_PTRS[FTM]->CONTROLS[config.channel].CnSC & ~(FTM_CnSC_ELSB_MASK | FTM_CnSC_ELSA_MASK)) |
    		    			(FTM_CnSC_ELSB((config.PWM_logic >> 1) & 0X01) | FTM_CnSC_ELSA((config.PWM_logic >> 0) & 0X01));
    	break;
    }
    if(config.mode == FTM_PWM)
    {
    	// Turn percentage duty cycle to counter value
    	config.counter = (uint16_t)((config.PWM_DC / 100.0) * config.modulo);
    }
    FTM_PTRS[FTM]->CONTROLS[config.channel].CnV = FTM_CnV_VAL(config.counter);
    // Set DMA mode
    FTM_PTRS[FTM]->CONTROLS[config.channel].CnSC = (FTM_PTRS[FTM]->CONTROLS[config.channel].CnSC & ~(FTM_CnSC_DMA_MASK)) |
    											   (FTM_CnSC_DMA(config.DMA_on));
    // Set interrupts
    FTM_PTRS[FTM]->CONTROLS[config.channel].CnSC = (FTM_PTRS[FTM]->CONTROLS[config.channel].CnSC & ~FTM_CnSC_CHIE_MASK) |
    											   FTM_CnSC_CHIE(config.interrupt_on);
    // Disable write
    FTM_PTRS[FTM]->MODE = (FTM_PTRS[FTM]->MODE & ~FTM_MODE_WPDIS_MASK) | FTM_MODE_WPDIS(0);
}

void FTM_Start(uint8_t FTM)
{
	FTM_PTRS[FTM]->SC |= FTM_SC_CLKS(0x01);
}

void FTM_Stop(uint8_t FTM)
{
	FTM_PTRS[FTM]->SC &= ~FTM_SC_CLKS(0x01);
}

void FTM_resetCounter(uint8_t FTM)
{
	// Enable write
	FTM_PTRS[FTM]->MODE |= FTM_MODE_WPDIS(1);
	// Reset counter
	FTM_PTRS[FTM]->CNT = 0x0;
	// Disable write
    FTM_PTRS[FTM]->MODE = (FTM_PTRS[FTM]->MODE & ~FTM_MODE_WPDIS_MASK) | FTM_MODE_WPDIS(0);
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/



/******************************************************************************/
