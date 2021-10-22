/*
 * I2C.h
 *
 *  Created on: 25/10/2016
 *      Author: USUARO
 */

#ifndef SOURCES_I2C_H_
#define SOURCES_I2C_H_

#define	Receive		0    /* Master and slave RX*/
#define	Transmit	1    /* Master and slave TX*/

void I2C_init(void);
int I2C_read_Transmition(int address, char *data, int length, int stop);
int I2C_write_Transmition(int address, const char *data, int length, int stop);


#endif /* SOURCES_I2C_H_ */
