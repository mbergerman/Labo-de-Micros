/***************************************************************************//**
  @file     PDRV_SPI.c
  @brief    SPI driver. 
  @author   Grupo 1
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "PDRV_SPI.h"
#include "MK64F12.h"
#include "hardware.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

//SPI0 PORT, PINOUT, AND MUX ALT
#define SPI0_SCLK_PORT   3
#define SPI0_SOUT_PORT   3
#define SPI0_SIN_PORT    3

#define SPI0_PCS0_PORT   3
#define SPI0_PCS1_PORT   2
#define SPI0_PCS2_PORT   2
#define SPI0_PCS3_PORT   2
#define SPI0_PCS4_PORT   2
#define SPI0_PCS5_PORT   1

#define SPI0_SCLK_PIN   ((SPI0_SCLK_PORT<<5) + 1)   // PTD1
#define SPI0_SOUT_PIN   ((SPI0_SOUT_PORT<<5) + 2)   // PTD2
#define SPI0_SIN_PIN    ((SPI0_SIN_PORT<<5) + 3)   // PTD3

#define SPI0_PCS0_PIN   ((SPI0_PCS0_PORT<<5) + 0)   // PTD0
#define SPI0_PCS1_PIN   ((SPI0_PCS1_PORT<<5) + 3)   // PTC3
#define SPI0_PCS2_PIN   ((SPI0_PCS2_PORT<<5) + 2)   // PTC2
#define SPI0_PCS3_PIN   ((SPI0_PCS3_PORT<<5) + 1)   // PTC1
#define SPI0_PCS4_PIN   ((SPI0_PCS4_PORT<<5) + 0)   // PTC0
#define SPI0_PCS5_PIN   ((SPI0_PCS4_PORT<<5) + 23)   // PTB23

#define SPI0_SCLK_ALT    2
#define SPI0_SOUT_ALT    2
#define SPI0_SIN_ALT     2

#define SPI0_PCS0_ALT    2
#define SPI0_PCS1_ALT    2
#define SPI0_PCS2_ALT    2
#define SPI0_PCS3_ALT    2
#define SPI0_PCS4_ALT    2
#define SPI0_PCS5_ALT    3

// SPI1 PORT, PINOUT, AND MUX ALT (NOT ACCESSIBLE IN FRDM-K64F DEVELOPMENT BOARD)
#define SPI1_SCLK_PORT   4
#define SPI1_SOUT_PORT   4
#define SPI1_SIN_PORT    4

#define SPI1_PCS0_PORT   4
#define SPI1_PCS1_PORT   4
#define SPI1_PCS2_PORT   4
#define SPI1_PCS3_PORT   4

#define SPI1_SCLK_PIN   ((SPI1_SCLK_PORT<<5) + 2)   // PTE2
#define SPI1_SOUT_PIN   ((SPI1_SOUT_PORT<<5) + 1)   // PTE1
#define SPI1_SIN_PIN    ((SPI1_SIN_PORT<<5) + 3)   // PTE3

#define SPI1_PCS0_PIN   ((SPI1_PCS0_PORT<<5) + 4)   // PTE4
#define SPI1_PCS1_PIN   ((SPI1_PCS1_PORT<<5) + 0)   // PTE4
#define SPI1_PCS2_PIN   ((SPI1_PCS2_PORT<<5) + 5)   // PTE4
#define SPI1_PCS3_PIN   ((SPI1_PCS3_PORT<<5) + 6)   // PTE4

#define SPI1_SCLK_ALT    2
#define SPI1_SOUT_ALT    2
#define SPI1_SIN_ALT     2

#define SPI1_PCS0_ALT    2
#define SPI1_PCS1_ALT    2
#define SPI1_PCS2_ALT    2
#define SPI1_PCS3_ALT    2

// SPI2 PORT, PINOUT, AND MUX ALT (NOT ACCESSIBLE IN FRDM-K64F DEVELOPMENT BOARD)
#define SPI2_SCLK_PORT   1
#define SPI2_SOUT_PORT   1
#define SPI2_SIN_PORT    1
#define SPI2_PCS0_PORT   1

#define SPI2_SCLK_PIN   ((SPI2_SCLK_PORT<<5) + 21)   // PTB21
#define SPI2_SOUT_PIN   ((SPI2_SOUT_PORT<<5) + 22)   // PTB22
#define SPI2_SIN_PIN    ((SPI2_SIN_PORT<<5) + 23)   // PTB23
#define SPI2_PCS0_PIN   ((SPI2_PCS0_PORT<<5) + 20)   // PTB20

#define SPI2_SCLK_ALT    2
#define SPI2_SOUT_ALT    2
#define SPI2_SIN_ALT     2
#define SPI2_PCS0_ALT    2

// PORT SELECT MACROS
#define SPI_SCLK_PORT(spi_port)     ((spi_port == 0)? SPI0_SCLK_PORT: \
                                    ((spi_port == 1)? SPI1_SCLK_PORT: \
                                                SPI2_SCLK_PORT))

#define SPI_SOUT_PORT(spi_port)     ((spi_port == 0)? SPI0_SOUT_PORT: \
                                    ((spi_port == 1)? SPI1_SOUT_PORT: \
                                                SPI2_SOUT_PORT))

#define SPI_SIN_PORT(spi_port)      ((spi_port == 0)? SPI0_SIN_PORT: \
                                    ((spi_port == 1)? SPI1_SIN_PORT: \
                                                SPI2_SIN_PORT))

#define SPI_PCS0_PORT(spi_port)     ((spi_port == 0)? SPI0_PCS0_PORT: \
                                    ((spi_port == 1)? SPI1_PCS0_PORT: \
                                                SPI2_PCS0_PORT))

#define SPI_PCS1_PORT(spi_port)     ((spi_port == 0)? SPI0_PCS1_PORT: SPI1_PCS1_PORT)
#define SPI_PCS2_PORT(spi_port)     ((spi_port == 0)? SPI0_PCS2_PORT: SPI1_PCS2_PORT)
#define SPI_PCS3_PORT(spi_port)     ((spi_port == 0)? SPI0_PCS3_PORT: SPI1_PCS3_PORT)

// PIN SELECT MACROS
#define SPI_SCLK_PIN(spi_port)     ((spi_port == 0)? SPI0_SCLK_PIN: \
                                    ((spi_port == 1)? SPI1_SCLK_PIN: \
                                                SPI2_SCLK_PIN))

#define SPI_SOUT_PIN(spi_port)     ((spi_port == 0)? SPI0_SOUT_PIN: \
                                    ((spi_port == 1)? SPI1_SOUT_PIN: \
                                                SPI2_SOUT_PIN))

#define SPI_SIN_PIN(spi_port)      ((spi_port == 0)? SPI0_SIN_PIN: \
                                    ((spi_port == 1)? SPI1_SIN_PIN: \
                                                SPI2_SIN_PIN))

#define SPI_PCS0_PIN(spi_port)     ((spi_port == 0)? SPI0_PCS0_PIN: \
                                    ((spi_port == 1)? SPI1_PCS0_PIN: \
                                                SPI2_PCS0_PIN))

#define SPI_PCS1_PIN(spi_port)     ((spi_port == 0)? SPI0_PCS1_PIN: SPI1_PCS1_PIN)
#define SPI_PCS2_PIN(spi_port)     ((spi_port == 0)? SPI0_PCS2_PIN: SPI1_PCS2_PIN)
#define SPI_PCS3_PIN(spi_port)     ((spi_port == 0)? SPI0_PCS3_PIN: SPI1_PCS3_PIN)

// ALT SELECT MACROS
#define SPI_SCLK_ALT(spi_port)     ((spi_port == 0)? SPI0_SCLK_ALT: \
                                    ((spi_port == 1)? SPI1_SCLK_ALT: \
                                                SPI2_SCLK_ALT))

#define SPI_SOUT_ALT(spi_port)     ((spi_port == 0)? SPI0_SOUT_ALT: \
                                    ((spi_port == 1)? SPI1_SOUT_ALT: \
                                                SPI2_SOUT_ALT))

#define SPI_SIN_ALT(spi_port)      ((spi_port == 0)? SPI0_SIN_ALT: \
                                    ((spi_port == 1)? SPI1_SIN_ALT: \
                                                SPI2_SIN_ALT))

#define SPI_PCS0_ALT(spi_port)     ((spi_port == 0)? SPI0_PCS0_ALT: \
                                    ((spi_port == 1)? SPI1_PCS0_ALT: \
                                                SPI2_PCS0_ALT))

#define SPI_PCS1_ALT(spi_port)     ((spi_port == 0)? SPI0_PCS1_ALT: SPI1_PCS1_ALT)
#define SPI_PCS2_ALT(spi_port)     ((spi_port == 0)? SPI0_PCS2_ALT: SPI1_PCS2_ALT)
#define SPI_PCS3_ALT(spi_port)     ((spi_port == 0)? SPI0_PCS3_ALT: SPI1_PCS3_ALT)

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

static SPI_Type* const SPI_PTRS[] = SPI_BASE_PTRS;
static PORT_Type* const PORT_PTRS[] = PORT_BASE_PTRS;

static SPI_mode_t port_mode[3];

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void SPI_Init(uint8_t spi_port, SPI_config_t config)
{
    port_mode[spi_port] = config.mode; //remember port mode
    config.chip_selects = ((config.chip_selects == 0)? 1 : config.chip_selects); //Minimum number of peripherals is 1
    
    //Clock Gating
    switch(spi_port)
    {
        case 0:
            SIM->SCGC6 |= SIM_SCGC6_SPI0(1);
            NVIC_EnableIRQ(SPI0_IRQn);
            break;
        case 1:
            SIM->SCGC6 |= SIM_SCGC6_SPI1(1);
            NVIC_EnableIRQ(SPI1_IRQn);
            break;
        case 2:
            SIM->SCGC3 |= SIM_SCGC3_SPI0(1);
            NVIC_EnableIRQ(SPI2_IRQn);
            break;
    }

    //Pins Config
    PORT_PTRS[SPI_SCLK_PORT(spi_port)]->PCR[SPI_SCLK_PIN(spi_port)] = 0x0; //Clear
    PORT_PTRS[SPI_SCLK_PORT(spi_port)]->PCR[SPI_SCLK_PIN(spi_port)] |= PORT_PCR_MUX(SPI_SCLK_ALT(spi_port));

    PORT_PTRS[SPI_SOUT_PORT(spi_port)]->PCR[SPI_SOUT_PIN(spi_port)] = 0x0; //Clear  
    PORT_PTRS[SPI_SOUT_PORT(spi_port)]->PCR[SPI_SOUT_PIN(spi_port)] |= PORT_PCR_MUX(SPI_SOUT_ALT(spi_port));

    PORT_PTRS[SPI_SIN_PORT(spi_port)]->PCR[SPI_SIN_PIN(spi_port)] = 0x0; //Clear
    PORT_PTRS[SPI_SIN_PORT(spi_port)]->PCR[SPI_SIN_PIN(spi_port)] |= PORT_PCR_MUX(SPI_SIN_ALT(spi_port));
    
    // Initialize peripheral chip selects only in master mode
    if(config.mode)
    {
        PORT_PTRS[SPI_PCS0_PORT(spi_port)]->PCR[SPI_PCS0_PIN(spi_port)] = 0x0; //Clear
        PORT_PTRS[SPI_PCS0_PORT(spi_port)]->PCR[SPI_PCS0_PIN(spi_port)] |= PORT_PCR_MUX(SPI_PCS0_ALT(spi_port));

        if (config.chip_selects > 1 && spi_port != 2)
        {
            PORT_PTRS[SPI_PCS1_PORT(spi_port)]->PCR[SPI_PCS1_PIN(spi_port)] = 0x0; //Clear
            PORT_PTRS[SPI_PCS1_PORT(spi_port)]->PCR[SPI_PCS1_PIN(spi_port)] |= PORT_PCR_MUX(SPI_PCS1_ALT(spi_port));
        }
        if (config.chip_selects > 2 && spi_port != 2)
        {
            PORT_PTRS[SPI_PCS2_PORT(spi_port)]->PCR[SPI_PCS2_PIN(spi_port)] = 0x0; //Clear
            PORT_PTRS[SPI_PCS2_PORT(spi_port)]->PCR[SPI_PCS2_PIN(spi_port)] |= PORT_PCR_MUX(SPI_PCS2_ALT(spi_port));
        }
        if (config.chip_selects > 3 && spi_port != 2)
        {
            PORT_PTRS[SPI_PCS3_PORT(spi_port)]->PCR[SPI_PCS3_PIN(spi_port)] = 0x0; //Clear
            PORT_PTRS[SPI_PCS3_PORT(spi_port)]->PCR[SPI_PCS3_PIN(spi_port)] |= PORT_PCR_MUX(SPI_PCS3_ALT(spi_port));
        }
        if (config.chip_selects > 4 && spi_port == 0)
        {
            PORT_PTRS[SPI0_PCS4_PORT]->PCR[SPI0_PCS4_PIN] = 0x0; //Clear
            PORT_PTRS[SPI0_PCS4_PORT]->PCR[SPI0_PCS4_PIN] |= PORT_PCR_MUX(SPI0_PCS4_ALT);
        }
        if (config.chip_selects > 5 && spi_port == 0)
        {
            PORT_PTRS[SPI0_PCS5_PORT]->PCR[SPI0_PCS5_PIN] = 0x0; //Clear
            PORT_PTRS[SPI0_PCS5_PORT]->PCR[SPI0_PCS5_PIN] |= PORT_PCR_MUX(SPI0_PCS5_ALT);
        }
    }

    // SPI Registers Config
    // MCR Config
    SPI_PTRS[spi_port]->MCR = 0x0 | SPI_MCR_HALT(1); //Clear and Halt - SPI is enabled at the end of the Init
    SPI_PTRS[spi_port]->MCR |= SPI_MCR_MSTR(config.mode); //Define SPI mode Slave or Master
    SPI_PTRS[spi_port]->MCR |= SPI_MCR_PCSIS(config.PCSIS); //Define SPI chip select inactive status as Active High or Active Low
    //SPI_PTRS[spi_port]->MCR |= SPI_MCR_CONT_SCKE(1);  //Turn SCLK Continuous mode on
    SPI_PTRS[spi_port]->MCR |= SPI_MCR_ROOE(1);  //RX FIFO overflow data into shift register 

    // CTAR Config
    if(config.mode)
    {
        // CTAR Master Config
        SPI_PTRS[spi_port]->CTAR[1] = 0x0;
        SPI_PTRS[spi_port]->CTAR[1] |= SPI_CTAR_DBR(0); // 50/50 Clock duty cycle
        SPI_PTRS[spi_port]->CTAR[1] |= SPI_CTAR_FMSZ(config.frame_size); // User defined frame size
        SPI_PTRS[spi_port]->CTAR[1] |= SPI_CTAR_LSBFE(config.LSB_first); // User defined LSB first in transfer or not
        SPI_PTRS[spi_port]->CTAR[1] |= SPI_CTAR_CPHA(config.clock_phase); // User defined SCK phase
        SPI_PTRS[spi_port]->CTAR[1] |= SPI_CTAR_CPOL(config.clock_polarity); // User defined SCK polarity

    }
    else
    {
        // CTAR Slave Config
        SPI_PTRS[spi_port]->CTAR[0] |= SPI_CTAR_SLAVE_FMSZ(config.frame_size); // User defined frame size
        SPI_PTRS[spi_port]->CTAR[0] |= SPI_CTAR_SLAVE_CPHA(config.clock_phase); // User defined SCK phase
        SPI_PTRS[spi_port]->CTAR[0] |= SPI_CTAR_SLAVE_CPOL(config.clock_polarity); // User defined SCK polarity

    }
    //Rest flags
    SPI_PTRS[spi_port]->SR = 0x0;
    SPI_PTRS[spi_port]->SR |= SPI_SR_TCF(1);
    //Set Transfer Counter to 0
    SPI_PTRS[spi_port]->TCR |= SPI_TCR_SPI_TCNT(0);
    //Enable SPI port
    SPI_PTRS[spi_port]->MCR |= SPI_MCR_HALT(0);

}

uint16_t SPI_Read(uint8_t spi_port)
{
    return SPI_PTRS[spi_port]->POPR;
}

void SPI_Write(uint8_t spi_port, uint16_t spi_data)
{
    SPI_PTRS[spi_port]->PUSHR = spi_data;
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/



/******************************************************************************/
