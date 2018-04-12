/*
 * SPI.h
 *
 *  Created on: 19-12-2015
 *      Author: Mefiu
 */

#ifndef SPI_H_
#define SPI_H_

//#include <avr/io.h>

#define FOSC 64 //dzielnik SPI

#if defined (__AVR_ATmega644P__)
#define MOSI PB5
#define SCK PB7
#define MISO PB6
//CS i reszta definiowane w bibliotece ukladu z ktorym sie komunikuje
#endif

void InitSPI();
void SendSPI(uint8_t bajt);
uint8_t ReceiveSPI();
uint8_t TransferSPI(uint8_t bajt);


#endif /* SPI_H_ */
