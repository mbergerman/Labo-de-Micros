/***************************************************************************//**
  @file     App.c
  @brief    Application functions
  @author   Nicolás Magliola
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "DRV_Board.h"
#include "PDRV_GPIO.h"
#include "MK64F12.h"
#include "hardware.h"
#include <stdio.h>
#include <string.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define CR	(0x0D)
#define LF	(0x0A)


static const char message[] = "The quick brown fox jumps over the lazy dog";
static uint32_t message_len;

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

enum {
	UART2_RTS_b_PIN,
	UART2_CTS_b_PIN,
	UART2_RX_PIN,
	UART2_TX_PIN
};

// PTA1 y PTA2
#define UART0_RX_PIN 1
#define UART0_TX_PIN 2

static uint8_t txfifosize;
static uint8_t rxfifosize;
/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void UART_SetBaudRate (UART_Type* uart, uint32_t baudrate);

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Función que se llama 1 vez, al comienzo del programa */
void App_Init (void)
{
	initBoard();
	setLED(GREEN, 0);

	message_len = strlen(message);

	SIM->SCGC4 |= SIM_SCGC4_UART0(1);

	PORTD->PCR[UART2_RX_PIN] = (uint32_t)0;
	PORTD->PCR[UART2_RX_PIN] = PORT_PCR_MUX(0b11); //ALT3
	PORTD->PCR[UART2_TX_PIN] = (uint32_t)0;
	PORTD->PCR[UART2_TX_PIN] = PORT_PCR_MUX(0b11); //ALT3

	PORTA->PCR[UART0_RX_PIN] = (uint32_t)0;
	PORTA->PCR[UART0_RX_PIN] = PORT_PCR_MUX(0b11);
	PORTA->PCR[UART0_TX_PIN] = (uint32_t)0;
	PORTA->PCR[UART0_TX_PIN] = PORT_PCR_MUX(0b11);

	//UART2->C1 |= UART_C1_PE(1) | UART_C1_PT(0); // activo paridad par
	//UART2->S2 |= UART_S2_MSBF(0); //  0 = LSB first ; 1 = MSB first

	//When setting TXFE both C2[TE] and C2[RE] must be cleared.
	UART0->PFIFO |= UART_PFIFO_TXFE(1) | UART_PFIFO_RXFE(1);
	//TXFLUSH and RXFLUSH commands must be issued immediately after TXFE and RXFE
	UART0->CFIFO |= UART_CFIFO_RXFLUSH(1) | UART_CFIFO_TXFLUSH(1);
	//TXFIFOSIZE = RXFIFOSIZE = 010 (8 datawords)
	txfifosize = (UART0->PFIFO & UART_PFIFO_TXFIFOSIZE_MASK) >> UART_PFIFO_TXFIFOSIZE_SHIFT;
	rxfifosize = (UART0->PFIFO & UART_PFIFO_RXFIFOSIZE_MASK) >> UART_PFIFO_RXFIFOSIZE_SHIFT;
	UART0->TWFIFO = txfifosize*4-1; //TXWATER < TXFIFOSIZE_DATAWORDS = 8
	UART0->RWFIFO = rxfifosize*4-1;

	UART0->C5 &= ~UART_C5_TDMAS_MASK;
	UART0->C2 = UART_C2_TE_MASK |  UART_C2_RE_MASK | UART_C2_TIE_MASK;

	NVIC_EnableIRQ(UART0_RX_TX_IRQn);

	UART_SetBaudRate(UART0, 50); //use 150, 9600 and 115200 baudios for different results

	printf("TXFIFOSIZE = %X\n", txfifosize );
	printf("RXFIFOSIZE = %X\n", (UART0->PFIFO & UART_PFIFO_RXFIFOSIZE_MASK) >> UART_PFIFO_RXFIFOSIZE_SHIFT );
}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
{
	static char message[100];
	static int i = 0;
	//connect TX(PTB16) with RX (PTB17)
	if (UART0->RCFIFO == 5) // Si tengo un paquete de rxfifosize
	{
		for(int k=1 ; k<=7 ; k++) {
			message[i++]=UART0->D; // Extraigo el carácter recibido
			// Imprimo el carácter recibido
		}
	}
	i = i%100;
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

__ISR__ UART0_RX_TX_IRQHandler(void) {
	//si estoy acá es porque C2[TIE]==1 y S1[TDRE]==1.
	//No tengo que fijarme si puedo transmitir.
	static int i = 0;


	for(int k=1 ; k<=5; k++){

		if(k == 7) {
			UART0->S1;//read to clear TDRE !! See S1[TDRE]
		}

		if(i<message_len){
			setLED(GREEN,1); //use for testing
			UART0->D = message[i]; // Transmito
			i++;
			setLED(GREEN,0);
		}
		else if(i==message_len) {
			UART0->D = CR;
			i++;
		}
		else {
			UART0->D = LF;
			i=0;
		}
	}

}

/*******************************************************************************
 ******************************************************************************/
