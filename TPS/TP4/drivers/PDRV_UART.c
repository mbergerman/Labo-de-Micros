/***************************************************************************//**
  @file     UART_DRV.c
  @brief    Driver para el manejo del periférico de comunicación UART
  @author   Grupo 1
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "os.h"
#include "MK64F12.h"
#include "PDRV_UART.h"
#include "hardware.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define SCGC_MASKS(id)	((id==0)?(SIM_SCGC4_UART0(1)): \
						((id==1)?(SIM_SCGC4_UART1(1)): \
						((id==2)?(SIM_SCGC4_UART2(1)): \
						((id==3)?(SIM_SCGC4_UART3(1)): \
						((id==4)?(SIM_SCGC1_UART4(1)): \
						((id==5)?(SIM_SCGC1_UART5(1)):0))))))

#define UART_RX_PIN(id) ((id==0)?(16): \
						 (id==2)?(2): \
						 (id==3)?(16): (2))
#define UART_TX_PIN(id) ((id==0)?(17): \
						 (id==2)?(3): \
						 (id==3)?(17):(3))

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef struct {
	char buffer[BUFFER_SIZE];
	char* buffer_ptr;
	union {
		char* writer_ptr;
		char* reader_ptr;
	};
	uint32_t len;
	union {
		bool done;
		bool read;
	};
} buffer_t;

/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/



/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void UART_SetBaudRate (UART_Type* uart, uint32_t baudrate);
void uart_irq_handler(uint8_t id);

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/


/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

//Init
static PORT_Type* const PORT_PTRS[6] = {PORTB,PORTA,PORTD,PORTC,PORTA,PORTA};
static UART_Type* const UART_PTRS[6] = UART_BASE_PTRS;
//static uint8_t const PORT_INDEX[] = {PA, PE, PD, PC, PE, PE};
static const IRQn_Type IRQn[] = {UART0_RX_TX_IRQn, UART1_RX_TX_IRQn, UART2_RX_TX_IRQn, UART3_RX_TX_IRQn, UART4_RX_TX_IRQn, UART5_RX_TX_IRQn};

//

static buffer_t tx[UART_CANT_IDS];
static buffer_t rx[UART_CANT_IDS];

static uart_callback_t uart_callbacks[6];

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void initUart (uint8_t id, uart_cfg_t config) {

	PORT_Type* const PORTX = PORT_PTRS[id];
	UART_Type* const UARTX = UART_PTRS[id];

	switch(id) {
	case 0: case 1: case 2: case 3:
		SIM->SCGC4 |= SCGC_MASKS(id);
		break;
	case 4: case 5:
		SIM->SCGC1 |= SCGC_MASKS(id);
	}

	PORTX->PCR[UART_RX_PIN(id)] = (uint32_t)0; //clear
	PORTX->PCR[UART_RX_PIN(id)] = PORT_PCR_MUX(0b11); //Alt3
	PORTX->PCR[UART_TX_PIN(id)] = (uint32_t)0; //clear
	PORTX->PCR[UART_TX_PIN(id)] = PORT_PCR_MUX(0b11); //Alt3

	//UART2->C1 |= UART_C1_PE(1) | UART_C1_PT(0); // activo paridad par
	//UART2->S2 |= UART_S2_MSBF(0); //  0 = LSB first ; 1 = MSB first

	// When setting TXFE both C2[TE] and C2[RE] must be cleared.
	//UARTX->PFIFO |= UART_PFIFO_TXFE(1) | UART_PFIFO_RXFE(1);
	// TXFLUSH and RXFLUSH commands must be issued immediately after TXFE and RXFE
	//UARTX->CFIFO |= UART_CFIFO_RXFLUSH(1) | UART_CFIFO_TXFLUSH(1);
	// TXFIFOSIZE = RXFIFOSIZE = 010 (8 datawords)
	//txfifosize = (UARTX->PFIFO & UART_PFIFO_TXFIFOSIZE_MASK) >> UART_PFIFO_TXFIFOSIZE_SHIFT;
	//rxfifosize = (UARTX->PFIFO & UART_PFIFO_RXFIFOSIZE_MASK) >> UART_PFIFO_RXFIFOSIZE_SHIFT;
	//UARTX->TWFIFO = txfifosize*4-1; //TXWATER < TXFIFOSIZE_DATAWORDS = 8
	//UARTX->RWFIFO = rxfifosize*4-1;

	UARTX->C5 &= ~UART_C5_TDMAS_MASK;
	UARTX->C2 = UART_C2_TE_MASK | UART_C2_RE_MASK | UART_C2_RIE_MASK;

	NVIC_EnableIRQ(IRQn[id]);

	UART_SetBaudRate(UARTX, config.baudrate);


	// init buffers
	for(int id=0; id<UART_CANT_IDS; id++){
		rx[id].buffer_ptr = rx[id].buffer;
		rx[id].reader_ptr = rx[id].buffer;
		tx[id].buffer_ptr = tx[id].buffer;
		tx[id].reader_ptr = tx[id].buffer;
		tx[id].done = true;
	}

}

uint8_t uartIsRxMsg(uint8_t id) {
	return rx[id].read;
}

uint32_t uartGetRxMsgLength(uint8_t id) {
	return rx[id].len;
}

uint8_t uartReadMsg(uint8_t id, char* msg, uint8_t cant) {
	//rx[id].read = false;
	if (rx[id].read == false) {
		return 0;
	}
	uint32_t len = rx[id].len;

	for(int i=0 ; i<cant && i<len; i++) {
		*(msg+i) = *rx[id].reader_ptr;
		rx[id].reader_ptr++;
		rx[id].len--;
		if(rx[id].reader_ptr - rx[id].buffer >= BUFFER_SIZE) {
			rx[id].reader_ptr = rx[id].buffer;
		}
	}
	return (cant < len) ? cant : len;
}



uint8_t uartWriteMsg(uint8_t id, const char* msg, uint8_t cant) {

	if(cant > BUFFER_SIZE || id >= UART_CANT_IDS) {
		return 1;
	}

	tx[id].done = false;

	//copy message
	for(uint32_t i = 0; i < cant && i<(BUFFER_SIZE-tx[id].len); i++) {
		//tx[id].buffer[i] = *(msg+i);
		*tx[id].writer_ptr = *(msg+i);
		tx[id].writer_ptr++;
		tx[id].len++;
		if(tx[id].writer_ptr - tx[id].buffer >= BUFFER_SIZE) {
			tx[id].writer_ptr = tx[id].buffer;
		}
	}

	UART_Type* const UARTX = UART_PTRS[id];

	//enable transmission
	UARTX->C2 |= UART_C2_TIE_MASK;
	return 0;
}

bool uartIsTxMsgComplete(uint8_t id) {
	return tx[id].done;
}


void uartSetCallback(uint8_t id, uart_callback_t callback_fn){
	uart_callbacks[id] = callback_fn;
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void UART_SetBaudRate (UART_Type* uart, uint32_t baudrate) {
	uint16_t sbr, brfa;
	uint32_t clock;
	clock = ((uart==UART0 || uart==UART1))?(__CORE_CLOCK__):(__CORE_CLOCK__>>1);

	sbr = clock / (baudrate<<4);
	brfa = (clock<<1) / baudrate - (sbr<<5);

	uart->BDH = UART_BDH_SBR(sbr>>8);
	uart->BDL = UART_BDL_SBR(sbr);
	uart->C4 = (uart->C4 & ~UART_C4_BRFA_MASK) | UART_C4_BRFA(brfa);
}

void uart_irq_handler(uint8_t id) {
	CPU_SR_ALLOC();

	CPU_CRITICAL_ENTER();
	OSIntEnter();                                           /* Tell uC/OS-III that we are starting an ISR             */
	CPU_CRITICAL_EXIT();


	UART_Type* const UARTX = UART_PTRS[id];

	//interrupted by transmitter
	if (UARTX->S1 & UART_S1_TDRE_MASK) {
		// reading S1 with TDRE set clears S1[TDRE]
		if(tx[id].writer_ptr - tx[id].buffer_ptr != 0){
			UARTX->D = *tx[id].buffer_ptr; // Transmito
			tx[id].buffer_ptr++;
			tx[id].len--;
			if(tx[id].buffer_ptr - tx[id].buffer == BUFFER_SIZE) {
				tx[id].buffer_ptr = tx[id].buffer;
			}
		}
		else {
			tx[id].done = true;
			// disable transmission
			UARTX->C2 &= ~UART_C2_TIE_MASK;
			//UARTX->C2 |= UART_C2_RIE_MASK;
		}
	}

	//interrupt by receiver
	if (UARTX->S1 & UART_S1_RDRF_MASK) {
		// reading S1 with RDRF set and then reading D clears S1[RDRF]
		*rx[id].buffer_ptr = UARTX->D; // Guardo el byte recibido
		rx[id].buffer_ptr++;
		rx[id].len = (rx[id].len + 1) % BUFFER_SIZE;
		if(rx[id].buffer_ptr - rx[id].buffer == BUFFER_SIZE) {
			rx[id].buffer_ptr = rx[id].buffer;
		}
		rx[id].read = true;
		//UARTX->C2 &= ~UART_C2_RIE_MASK;
	}

	// Make call to callback function
	if(uart_callbacks[id]){
		uart_callbacks[id]();
	}


	OSIntExit();                                            /* Tell uC/OS-III that we are leaving the ISR             */
}

__ISR__ UART0_RX_TX_IRQHandler(void) {uart_irq_handler(0);}
__ISR__ UART1_RX_TX_IRQHandler(void) {uart_irq_handler(1);}
__ISR__ UART2_RX_TX_IRQHandler(void) {uart_irq_handler(2);}
__ISR__ UART3_RX_TX_IRQHandler(void) {uart_irq_handler(3);}
__ISR__ UART4_RX_TX_IRQHandler(void) {uart_irq_handler(4);}
__ISR__ UART5_RX_TX_IRQHandler(void) {uart_irq_handler(5);}
