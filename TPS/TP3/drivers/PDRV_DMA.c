/***************************************************************************//**
  @file     PDRV_DMA.c
  @brief    SPI driver.
  @author   Grupo 1
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "PDRV_DMA.h"
#include "MK64F12.h"
#include "hardware.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void dma_irq_handler(uint8_t channel);

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static DMA_config_t DMA_channel_config[16];
static dma_callback_t dma_callbacks[16];

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void DMA_init(uint8_t DMA_channel, DMA_config_t config)
{
	// Remember channel config
	DMA_channel_config[DMA_channel] = config;

	// Clock gating
	SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;
	SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;


	if(config.request_source >= 58){
		DMAMUX->CHCFG[DMA_channel] |= DMAMUX_CHCFG_ENBL_MASK | DMAMUX_CHCFG_TRIG_MASK | DMAMUX_CHCFG_SOURCE((uint8_t)config.request_source);
	}else{
		DMAMUX->CHCFG[DMA_channel] |= DMAMUX_CHCFG_ENBL_MASK | DMAMUX_CHCFG_SOURCE((uint8_t)config.request_source);
	}

	// Clear all the pending events
	NVIC_ClearPendingIRQ(DMA0_IRQn + DMA_channel);
	// Enable the DMA interrupts
	NVIC_EnableIRQ(DMA0_IRQn + DMA_channel);

	// Set source
	DMA0->TCD[DMA_channel].SADDR = (uint32_t)(config.source_buffer);

    // Set destination
	DMA0->TCD[DMA_channel].DADDR = (uint32_t)(config.destination_buffer);


	// Set an offset for source and destination address
	DMA0->TCD[DMA_channel].SOFF = config.source_offset;
	DMA0->TCD[DMA_channel].DOFF = config.destination_offset;

	// Set source and destination data transfer size
	switch(config.transfer_bytes)
	{
		case(1):
			DMA0->TCD[DMA_channel].ATTR = DMA_ATTR_SSIZE(0) | DMA_ATTR_DSIZE(0);
		break;
		case(2):
			DMA0->TCD[DMA_channel].ATTR = DMA_ATTR_SSIZE(1) | DMA_ATTR_DSIZE(1);
		break;
		case(4):
			DMA0->TCD[DMA_channel].ATTR = DMA_ATTR_SSIZE(2) | DMA_ATTR_DSIZE(2);
		break;

	}

	// Number of bytes to be transfered in each service request of the channel
	DMA0->TCD[DMA_channel].NBYTES_MLNO = config.transfer_bytes;

	// Number of major transfer cycles
	DMA0->TCD[DMA_channel].CITER_ELINKNO = DMA_CITER_ELINKNO_CITER(config.major_cycles);
	DMA0->TCD[DMA_channel].BITER_ELINKNO = DMA_BITER_ELINKNO_BITER(config.major_cycles);

	// Set Wrap Around
	DMA0->TCD[DMA_channel].SLAST = -config.wrap_around;


    // DLASTSGA DLAST Scatter and Gather
    DMA0->TCD[DMA_channel].DLAST_SGA = 0x00;

	// Setup control and status register.
    //DMA0->TCD[DMA_channel].CSR &= ~DMA_CSR_INTMAJOR_MASK;
	DMA0->TCD[DMA_channel].CSR |= DMA_CSR_INTMAJOR_MASK;		//Enable Major Interrupt.

	// Enable request signal
	switch(DMA_channel)
	{
		case 0:
			DMA0->ERQ = DMA_ERQ_ERQ0_MASK;
		break;
		case 1:
			DMA0->ERQ = DMA_ERQ_ERQ1_MASK;
		break;
		case 2:
			DMA0->ERQ = DMA_ERQ_ERQ2_MASK;
		break;
		case 3:
			DMA0->ERQ = DMA_ERQ_ERQ3_MASK;
		break;
		case 4:
			DMA0->ERQ = DMA_ERQ_ERQ4_MASK;
		break;
		case 5:
			DMA0->ERQ = DMA_ERQ_ERQ5_MASK;
		break;
		case 6:
			DMA0->ERQ = DMA_ERQ_ERQ6_MASK;
		break;
		case 7:
			DMA0->ERQ = DMA_ERQ_ERQ7_MASK;
		break;
		case 8:
			DMA0->ERQ = DMA_ERQ_ERQ8_MASK;
		break;
		case 9:
			DMA0->ERQ = DMA_ERQ_ERQ9_MASK;
		break;
		case 10:
			DMA0->ERQ = DMA_ERQ_ERQ10_MASK;
		break;
		case 11:
			DMA0->ERQ = DMA_ERQ_ERQ11_MASK;
		break;
		case 12:
			DMA0->ERQ = DMA_ERQ_ERQ12_MASK;
		break;
		case 13:
			DMA0->ERQ = DMA_ERQ_ERQ13_MASK;
		break;
		case 14:
			DMA0->ERQ = DMA_ERQ_ERQ14_MASK;
		break;
		case 15:
			DMA0->ERQ = DMA_ERQ_ERQ15_MASK;
		break;
	}

}

void DMA_setMajorCallback(uint8_t channel, dma_callback_t callback_fn){
	dma_callbacks[channel] = callback_fn;
}

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void dma_irq_handler(uint8_t channel){
	/* Clear the interrupt flag. */
	DMA0->CINT |= DMA_CINT_CINT(channel);
	if(dma_callbacks[channel]){
		dma_callbacks[channel]();
	}
}

void DMA0_IRQHandler(void){ dma_irq_handler(0); }
void DMA1_IRQHandler(void){ dma_irq_handler(1); }
void DMA2_IRQHandler(void){ dma_irq_handler(2); }
void DMA3_IRQHandler(void){ dma_irq_handler(3); }
void DMA4_IRQHandler(void){ dma_irq_handler(4); }
void DMA5_IRQHandler(void){ dma_irq_handler(5); }
void DMA6_IRQHandler(void){ dma_irq_handler(6); }
void DMA7_IRQHandler(void){ dma_irq_handler(7); }
void DMA8_IRQHandler(void){ dma_irq_handler(8); }
void DMA9_IRQHandler(void){ dma_irq_handler(9); }
void DMA10_IRQHandler(void){ dma_irq_handler(10); }
void DMA11_IRQHandler(void){ dma_irq_handler(11); }
void DMA12_IRQHandler(void){ dma_irq_handler(12); }
void DMA13_IRQHandler(void){ dma_irq_handler(13); }
void DMA14_IRQHandler(void){ dma_irq_handler(14); }
void DMA15_IRQHandler(void){ dma_irq_handler(15); }

void DMA_Error_IRQHandler(void){
	/* Clear all error indicators.*/
	DMA0->CERR = DMA_CERR_CAEI(1);
}

/******************************************************************************/
