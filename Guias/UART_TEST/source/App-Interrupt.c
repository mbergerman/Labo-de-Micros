/***************************************************************************//**
  @file     App.c
  @brief    Application functions
  @author   Pablo Gonzalez
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

	SIM->SCGC4 |= SIM_SCGC4_UART2_MASK;

	PORTD->PCR[UART2_RX_PIN] = (uint32_t)0;
	PORTD->PCR[UART2_RX_PIN] = PORT_PCR_MUX(0b11); //ALT3
	PORTD->PCR[UART2_TX_PIN] = (uint32_t)0;
	PORTD->PCR[UART2_TX_PIN] = PORT_PCR_MUX(0b11); //ALT3

	//UART2->C1 |= UART_C1_PE(1) | UART_C1_PT(0); // activo paridad par
	//UART2->S2 |= UART_S2_MSBF(0); //  0 = LSB first ; 1 = MSB first

	UART2->C5 &= ~UART_C5_TDMAS_MASK;
	UART2->C2 = UART_C2_TE_MASK |  UART_C2_RE_MASK | UART_C2_TIE_MASK;

	NVIC_EnableIRQ(UART2_RX_TX_IRQn);

	UART_SetBaudRate(UART2, 9600); //use 150, 9600 and 115200 baudios for different results
}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
{
	//connect TX(PTD3) with RX (PTD2)
	if (((UART2->S1) & UART_S1_RDRF_MASK)) // Si llego algo
	{
	char ch=UART2->D; // Extraigo el carácter recibido
	// Imprimo el carácter recibido
	printf("%c",ch);
	}

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

__ISR__ UART2_RX_TX_IRQHandler(void) {
	//si estoy acá es porque C2[TIE]==1 y S1[TDRE]==1.
	//No tengo que fijarme si puedo transmitir.
	static int i = 0;

	UART2->S1;//read to clear TDRE !!

	if(i<message_len){
		setLED(GREEN,1); //use for testing
		UART2->D = message[i]; // Transmito
		i++;
		setLED(GREEN,0);
	}
	else if(i==message_len) {
		UART2->D = CR;
		i++;
	}
	else {
		UART2->D = LF;
		i=0;
	}
}

/*******************************************************************************
 ******************************************************************************/
