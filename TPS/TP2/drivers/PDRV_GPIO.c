/***************************************************************************//**
  @file     gpio.c
  @brief    GPIO Driver
  @author   Pablo González
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "MK64F12.h"
#include "PDRV_GPIO.h"
#include "hardware.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
// See 12.2.12 System Clock Gating Control Register 5 (SIM_SCGC5)
#define SIM_SCGC5_PORT_MASK 0x3E00
#define SIM_SCGC5_PORT(port,b) (((uint32_t)(((uint32_t)(b)) << (9+port))) & SIM_SCGC5_PORT_MASK)
#define PORTX_IRQn(p) (PORTA_IRQn+p)
#define PINS_PER_PORT 32
#define ARRAY_SIZE (FSL_FEATURE_SOC_PORT_COUNT*PINS_PER_PORT)

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
static PORT_Type* const PORT_PTRS[FSL_FEATURE_SOC_PORT_COUNT] = PORT_BASE_PTRS;
static GPIO_Type* const GPIO_PTRS[FSL_FEATURE_SOC_PORT_COUNT] = GPIO_BASE_PTRS;

static const uint32_t IRQC_MODES[] = {0b0000,0b1000,0b1001,0b1010,0b1011,0b1100};

static pinIrqFun_t CALLBACKS[ARRAY_SIZE];

static void IRQHandler(int32_t port);

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void gpioMode (pin_t pin, uint8_t mode) {
	if ( pin > PORTNUM2PIN(PE,31) ) return;
	if ( mode > INPUT_PULLDOWN) return;

	uint32_t port = PIN2PORT(pin);

	// 1) Clock enable
	SIM->SCGC5 |= SIM_SCGC5_PORT(port,1);

	// 2) PORT configuration
	PORT_Type* port_ptr = PORT_PTRS[port];
	port_ptr->PCR[PIN2NUM(pin)] = 0x0;//!!!
	port_ptr->PCR[PIN2NUM(pin)] |= PORT_PCR_MUX(0b001);
	port_ptr->PCR[PIN2NUM(pin)] |= PORT_PCR_DSE(0b1);

	if ((mode==INPUT_PULLDOWN)||(mode==INPUT_PULLUP)) {
		port_ptr->PCR[PIN2NUM(pin)] |= PORT_PCR_PE(0b1);
		port_ptr->PCR[PIN2NUM(pin)] |= PORT_PCR_PS( mode==INPUT_PULLDOWN ? 0b0 : 0b1 );
	}

	// 3) GPIO configuration
	GPIO_Type* gpio_ptr = GPIO_PTRS[port];
	gpio_ptr->PDDR |= ((mode == OUTPUT) ? 1 : 0) << PIN2NUM(pin);

	return;
}

bool gpioIRQ (pin_t pin, uint8_t irqMode, pinIrqFun_t irqFun) {
	if ( pin > PORTNUM2PIN(PE,31) ) return false;
	if ( irqMode >= GPIO_IRQ_CANT_MODES ) return false;

	uint32_t port = PIN2PORT(pin);
	PORT_Type* port_ptr = PORT_PTRS[port];

	// 1)
	uint32_t pcr_clear_irqc = (port_ptr->PCR[PIN2NUM(pin)] & ~PORT_PCR_IRQC_MASK);
	port_ptr->PCR[PIN2NUM(pin)] = pcr_clear_irqc | PORT_PCR_IRQC(IRQC_MODES[irqMode]) | PORT_PCR_ISF_MASK;

	// 2)
	if (irqMode == GPIO_IRQ_MODE_DISABLE) {	NVIC_DisableIRQ(PORTX_IRQn(port)); }
	else { NVIC_EnableIRQ(PORTX_IRQn(port)); }

	CALLBACKS[PINS_PER_PORT*port + PIN2NUM(pin)] = irqFun;

	return true;
}

void gpioWrite (pin_t pin, bool value){
	if ( pin > PORTNUM2PIN(PE,31) ) return;

	GPIO_Type* gpio_ptr = GPIO_PTRS[PIN2PORT(pin)];
	if(value) {
		gpio_ptr->PSOR = (uint32_t)( 1 << PIN2NUM(pin) );
	}
	else {
		gpio_ptr->PCOR = (uint32_t)(1 << PIN2NUM(pin) );
	}
}

void gpioToggle (pin_t pin){
	if ( pin > PORTNUM2PIN(PE,31) ) return;

	GPIO_Type* gpio_ptr = GPIO_PTRS[PIN2PORT(pin)];
	gpio_ptr->PTOR = (uint32_t)(1 << PIN2NUM(pin));
}

bool gpioRead (pin_t pin){
	if ( pin > PORTNUM2PIN(PE,31) ) return 0;

	//GPIO_Type* gpio_ptr = GPIO_PTRS[PIN2PORT(pin)];
	uint32_t pdir = GPIO_PTRS[PIN2PORT(pin)]->PDIR;
	bool bit = pdir>>PIN2NUM(pin) & 0b1;
	return bit;
}

/*******************************************************************************
 ******************************************************************************/

static void IRQHandler(int32_t port) {
	PORT_Type* port_ptr = PORT_PTRS[port];
	uint32_t ISFR = port_ptr->ISFR;
	for(int pin = 0; pin<PINS_PER_PORT; pin++) {
		if (ISFR>>pin & 0b1) {
			port_ptr->ISFR |= 1<<pin; //w1c
			(*CALLBACKS[PINS_PER_PORT*port + pin])();
		}
	}
}

__ISR__ PORTA_IRQHandler(void) { IRQHandler(PA); }
__ISR__ PORTB_IRQHandler(void) { IRQHandler(PB); }
__ISR__ PORTC_IRQHandler(void) { IRQHandler(PC); }
__ISR__ PORTD_IRQHandler(void) { IRQHandler(PD); }
__ISR__ PORTE_IRQHandler(void) { IRQHandler(PE); }
