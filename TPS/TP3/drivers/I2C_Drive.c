/*
 * I2C.c
 *
 *  Created on: 25/10/2016
 *      Author: USUARO
 */
#include <I2C_Drive.h>
#include <PORT_Drive.h>


#define MUL 2		//1		//mult=2
#define ICR 0x3E	//0x14 //scl divisor=80	//en total divido por 160 al clock de 16Mhz

#define SCL_PORT	PORTE
#define SCL_PIN		24
#define SDA_PORT	PORTE
#define SDA_PIN		25

#define	I2C_ERROR_NO_SLAVE -1
#define	I2C_ERROR_BUS_BUSY -2

/********************************Funciones****************************************/
static void I2C_portInit (void);
static void I2C_clearInterrupt(void);
static bool I2C_getReceiveACK(void);
static void I2C_write(uint8_t data);
static void I2C_sendNAK(void);
static void I2C_sendACK(void);
static uint8_t I2C_read(void);
static void I2C_Stop(void);
void I2C_setMode(int mode);
static int I2C_Start(void);
static void I2C_setSlaveAddress(uint8_t address);
static void I2C_BaudRate (void);
/**************************************************************************************************/

void I2C_init(void)
{

	SIM->SCGC4 |= SIM_SCGC4_I2C0_MASK;			//Le doy clock
	//NVIC_EnableIRQ(I2C0_IRQn);

	I2C0->C1 = (I2C0->C1 & ~I2C_C1_IICEN_MASK) | I2C_C1_IICEN(1);	//Habilito Mi I2C

	I2C_BaudRate ();												//Inicializo la frecuencia del I2C con los defines (MUL , ICR)
	I2C_portInit ();
}

static void I2C_portInit (void)
{
	PCRstr UserPCR;

	UserPCR.PCR=false;					// Default All false, Set only those needed

	UserPCR.FIELD.DSE=true;				// Drive strength
	UserPCR.FIELD.MUX=PORT_mAlt5;		// I2C0  ********************** OJO, seria diferente capaz para otro I2C
	UserPCR.FIELD.IRQC=PORT_eDisabled;  // No irqs

	PORT_Configure2 (SCL_PORT,SCL_PIN,UserPCR);  // I2C0 SCL
	PORT_Configure2 (SDA_PORT,SDA_PIN,UserPCR);  // I2C0 SDA

	PORT_SetOpenDrainMode(SCL_PORT,SCL_PIN,1);
	PORT_SetOpenDrainMode(SDA_PORT,SDA_PIN,1);

}



// Devuelve un 1 si hay timeout.
bool timeoutStatus(uint32_t var)
{
    uint32_t j, out = 100000;				// tiempo de timeout (esto genera que sea bloqueante el sistema)
    											//queda esperando hasta que llegue el evento de finalizar indicado por mask
    for (j = 0; j < out; j++)
    {
        if (I2C0->S & var)
            return false;
    }
    return true;
}


//Esta funci�n se queda esperando hasta que finalize la transferencia de datos.
//Retorna:
//    0: si se resive ACK
//    1: OK no se recive  ACK (NCK)
//    2: Error
int I2C_wait_TXtransfer_finished()
{
    // Espera al flag de interrupci�n
    if (timeoutStatus(I2C_S_IICIF_MASK))
    {
        return 2;
    }

    I2C_clearInterrupt();

    // Espera a que se complete la transacci�n
    if (timeoutStatus(I2C_S_TCF_MASK)) {
        return 2;
    }

    //Devuelve si se recive un ACK
    return I2C_getReceiveACK() ? 0 : 1;
}

//Esta funci�n se queda esperando hasta que finalize el ciclo de lectura de datos
//    0: OK
//    1: Error
int I2C_wait_RXtransfer_finished() {
    // Espera a que termine la transferencia de Rx
    if (timeoutStatus(I2C_S_IICIF_MASK))
    {
        return 1;
    }

    I2C_clearInterrupt();

    return 0;
}

int I2C_doWrite(int value)
{
    I2C_write(value);

    //Comienza la escritura y se queda esperando que finalice
    return I2C_wait_TXtransfer_finished();
}

int I2C_doRead(char * data, int last)
{
	if (last)
	{
		I2C_sendNAK();
	}
	else
	{
		I2C_sendACK();
	}

    *data = (I2C_read() & 0xFF);

    // start rx transfer and wait the end of the transfer
    return I2C_wait_RXtransfer_finished();
}



int I2C_read_Transmition(int address, char *data, int length, int stop)
{
    int count;
    char dummy_read, *ptr;

    if (I2C_Start())
    {
        I2C_Stop();
        return I2C_ERROR_BUS_BUSY;
    }

    if (I2C_doWrite((address | 0x01)))
    {
        I2C_Stop();
        return I2C_ERROR_NO_SLAVE;
    }

    // Se pone en modo receptor
    I2C_setMode(Receive);

    // Lee de a grupos de Bytes
    for (count = 0; count < (length); count++)
    {
        ptr = (count == 0) ? &dummy_read : &data[count - 1];
        uint8_t stop_ = (count == (length - 1)) ? 1 : 0;
        if (I2C_doRead(ptr, stop_))
        {
            I2C_Stop();
            return count;
        }
    }

    // En caso de que no se quiera seguir mandando datos al mismo addres, mando un stop, sino un cero representaria un repeat start.
    if (stop)
        I2C_Stop();

    // last read
    data[count-1] = I2C_read();

    return length;
}

//recive la direecci�n de comunicaci�n un punterro al dato , el largo del mismo y, en caso de querer cortar la conversaci�n
//se deve colocar stop en 1
int I2C_write_Transmition(int address, const char *data, int length, int stop)
{
    int i;

    /*if (I2C_Start()) {
    	I2C_Stop();
        return I2C_ERROR_BUS_BUSY;
    }

    if (I2C_doWrite((address & 0xFE))) {
        I2C_Stop();
        return I2C_ERROR_NO_SLAVE;
    }*/

    I2C0->C1 = I2C0->C1 | I2C_C1_MST_MASK | I2C_C1_TX_MASK;
    I2C0->D = (address & 0xFE);
    I2C_wait_TXtransfer_finished();


    for (i = 0; i < length; i++)				//realiza la escritura del dato
    {
        if(I2C_doWrite(data[i]))
        {
        	I2C_Stop();
            return i;
        }
    }

    if (stop)									//manda un stop al finalizar la transmisi�n
    	I2C_Stop();								//en caso de querer seguir transmitiendo el stop debe estar en 0

    return length;
}

void I2C_reset() {
	I2C_Stop();
}

int I2C_byteRead(int last)
{
    char data;

    // set rx mode
    I2C_setMode(Receive);

    // Setup read
    I2C_doRead(&data, last);

    // set tx mode
    I2C_setMode(Transmit);
    return I2C_read();
}

int I2C_byteWrite(int data)
{
    // set tx mode
	I2C_setMode(Transmit);

    return !I2C_doWrite((data & 0xFF));
}

// FALTA LO D SLAVE ***********************************************

/*******************CAPA DE MAS ABAJO****************/
/****************************************************/
/****************************************************/
/****************************************************/
/****************************************************/
/****************************************************/
/****************************************************/
/****************************************************/
/****************************************************/
/****************************************************/
//Se pone en modo recive y esclavo
static void I2C_Stop(void)
{
	volatile uint32_t n = 0;

	I2C0->C1 &= ~(I2C_C1_MST_MASK | I2C_C1_TX_MASK);

    // It seems that there are timing problems
    // when there is no waiting time after a STOP.
    // This wait is also included on the samples
    // code provided with the freedom board
    for (n = 0; n < 200; n++) __NOP();
}


//Funci�n que indica el comienzo de la transmisi�n del mensaje
static int I2C_Start(void)
{

    /* me fijo si ya estamos en modo maestro.*/
    if (I2C0->C1 & I2C_C1_MST_MASK)
    {
        /*Si ya es maestro lo que quiere es mandar un bit de restart (RSTA)*/
    	I2C0->C1 = I2C0->C1 | I2C_C1_RSTA_MASK;

    }
    else
    {
        /*Si aun no es maestro, se coloca en modo maestro (MST) y en modo trasmitir (TX)*/
    	I2C0->C1 = I2C0->C1 | I2C_C1_MST_MASK | I2C_C1_TX_MASK;
    }

    return 0;
}

//Setea la direcci�n del esclavo correspondiente a 7 bits
static void I2C_setSlaveAddress(uint8_t address)
{

	//Lo shifteo para que me queden los 7 bits menos significativos como address
	I2C0->A1 = address << 1u;

    /*Enmascara el registro para utilizarlo en modo de 7 bits (ADEXT)*/
	I2C0->C2 = I2C0->C2 & ~I2C_C2_ADEXT_MASK;
}


//Seteo el clock mediante la formula BaudRate= clock period (Hz)/(mul x SCL divider)
static void I2C_BaudRate (void)
{
		//si se quiere cambiar el baud rate se debera modificar los defines segun los valores
		//deseados correspondientes al manual de referencia pagina 1500 y 1515
	    I2C0->F = ( I2C0->F & ~(I2C_F_MULT_MASK | I2C_F_ICR_MASK)) | (I2C_F_MULT(MUL)) | (I2C_F_ICR(ICR)) ;
}


//Seteo el modo (Puede ser modo receptor o transmisor)
void I2C_setMode(int mode)
{
	if(mode==Transmit)
		I2C0->C1 |= I2C_C1_TX_MASK;
	else
		I2C0->C1 &= ~I2C_C1_TX_MASK;
}

/*****************************************************************************/

//Devuelve si esta configurado en modo transmit o receive
int I2C_getMode()
{
    return I2C0->C1&I2C_C1_TX_MASK ? Transmit : Receive;
}


//Habilita El I2C
static inline void I2C_enable()
{
	I2C0->C1 |= I2C_C1_IICEN_MASK;
}


//Deshabilita el I2C
static inline void I2C_disable()
{
	I2C0->C1 &= ~I2C_C1_IICEN_MASK;
}

/*
//Funci�n que habilita o no el DMA (DMAEN)
//se le debe pasar 1 para habilitarlo
static inline void I2C_HAL_setDMAEnable(int enable)
{
	if(enable)
		MY_I2C->C1 |= I2C_C1_DMAEN_MASK;
	else
		MY_I2C->C1 &= ~I2C_C1_DMAEN_MASK;
}

*/

/*!
 * @brief Controls the stop mode hold off.
 *
 * This function lets you enable the hold off entry to low power stop mode when any data transmission
 * or reception is occurring.
 *
 * @param instance The I2C peripheral instance number
 * @param enable false - Stop hold off is disabled. The MCU's entry to stop mode is not gated.<br>
 *     true - Stop hold off is enabled.
 */

//Habilita o deshabilita el hold off para bajo consumo mientras se trasmiten datos
static inline void I2C_setStopHoldoff(int enable)
{
    if(enable)
    	I2C0->FLT |= I2C_FLT_SHEN_MASK;
    else
    	I2C0->FLT &= ~I2C_FLT_SHEN_MASK;
}


/*!
 * @brief Slave baud rate control
 *
 * Enables an independent slave mode baud rate at the maximum frequency. This forces clock stretching
 * on the SCL in very fast I2C modes.
 *
 * @param enable true - Slave baud rate is independent of the master baud rate;<br>
 *     false - The slave baud rate follows the master baud rate and clock stretching may occur.
 *//*
static inline void I2C_HAL_setIndependentSlaveBaud(bool enable)
{
	if(enable)
		MY_I2C->C2 |= I2C_C2_SBRC_MASK;
	else
		MY_I2C->C2 &= ~I2C_C2_SBRC_MASK;
}
*/

//Funci�n que manda el ACK	(Ctivo bajo)
static void I2C_sendACK(void)
{
	I2C0->C1 &= ~I2C_C1_TXAK_MASK;
}

//Funci�n que manda el NAK
static void I2C_sendNAK(void)
{
	I2C0->C1 |= I2C_C1_TXAK_MASK;
}

//Lee los datos
static uint8_t I2C_read(void)
{
    return I2C0->D;
}

//Escribe los datos
static void I2C_write(uint8_t data)
{
	I2C0->D=data;
}

// FALTAN los de SLAVE  ***************************

//Pregunta si esta configurado como esclavo o maestro
static inline bool I2C_get_Master_Mode(void)
{
    if(I2C0->C1&I2C_C1_MST_MASK)
    	return true;
    else
    	return false;
}

//Indica si la transferencia esta en proceso o si esta completa (TCF)
static inline bool I2C_isTransferComplete()
{
	if(I2C0->S&I2C_S_TCF_MASK)
		return true;
	else
		return false;
}

//Indica si el bus esta siendo ocupado por alguien o no
static inline bool I2C_isBusBusy()
{
	if(I2C0->S&I2C_S_BUSY_MASK)
		return true;
	else
		return false;
}

/*!
 * @brief Returns whether the arbitration procedure was lost.
 *
 * @retval true Loss of arbitration
 * @retval false Standard bus operation
 *//*
static inline bool I2C_HAL_wasArbitrationLost()
{
	if(MY_I2C->S&I2C_S_ARBL_MASK)
		return true;
	else
		return false;
}
*/

/*!
 * @brief Clears the arbitration lost flag.
 *
 */
/*
static inline void I2C_HAL_clearArbitrationLost()
{
    MY_I2C->S |= I2C_S_ARBL_MASK;
}
*/

//devuelve si ACK fue recivido
static bool I2C_getReceiveACK(void)
{
    if(I2C0->S&I2C_S_RXAK_MASK)
    	return false;
    else
    	return true;
}

//Habilita las interrupciones del I2C
static inline void I2C_enableInterrupt()
{
	I2C0->C1 |= I2C_C1_IICIE_MASK;
}

//Dehabilita las interrupciones del I2C
static inline void I2C_disableInterrupt()
{
    I2C0->C1 &= ~I2C_C1_IICIE_MASK;
}

//Devuelve si esta habilitada o no la interrupci�n
static inline bool I2C_isInterruptEnabled()
{
	if(I2C0->C1&I2C_C1_IICIE_MASK)
		return true;
	else
		return false;
}

//Devuelve si hay una interrupci�n pendiente
static inline bool I2C_isInterruptPending()
{
    if(I2C0->S&I2C_S_IICIF_MASK)
    	return true;
    else
    	return false;
}

//Borra el flag de interrupci�n pendiente
static void I2C_clearInterrupt(void)
{
	I2C0->S |= I2C_S_IICIF_MASK;
}

//Devuelve 1 si se registra un evento de stop
static inline bool I2C_getStopDetect()
{
	if(I2C0->FLT & I2C_FLT_STOPF_MASK)
		return true;
	else
		return false;
}

//Borra el flag de stop detectado
static inline void I2C_clearStopDetect()
{
	I2C0->FLT |= I2C_FLT_STOPF_MASK;
}

//Habilita la posibilidad de interrumpir al detectar start o stop al bus
static inline void I2C_enableBusStopInterrupt()
{
	I2C0->FLT |= I2C_FLT_SSIE_MASK;
}

//Deshabilita la posibilidad de interrumpir al detectar start o stop al bus
static inline void I2C_disableBusStopInterrupt()
{
	I2C0->FLT &= ~I2C_FLT_SSIE_MASK;
}


//Indica si el esta habilitado o no
static inline bool I2C_isBusStopInterruptEnabled()
{
	if(I2C0->FLT & I2C_FLT_SSIE_MASK)
		return true;
	else
		return false;
}

