/*
 * SPI.c
 *
 *  Created on: 19-12-2015
 *      Author: Mefiu
 */

#include <avr/io.h>

#include "SPI.h"

void InitSPI()
{
//*******ATmega644P
#if defined (__AVR_ATmega644P__)
	DDRB |= (1<<MOSI)|(1<<SCK); //MISO domyslnie wejsciem

#if FOSC == 2
	SPCR |= (1<<SPE)|(1<<MSTR);
	SPSR |= (1<<SPI2X);
#endif
#if FOSC == 4
	SPCR |= (1<<SPE)|(1<<MSTR);
#endif
#if FOSC == 8
	SPCR |= (1<<SPE)|(1<<MSTR)|(1<<SPR0);
	SPSR |= (1<<SPI2X);
#endif
#if FOSC == 16
	SPCR |= (1<<SPE)|(1<<MSTR)|(1<<SPR0);
#endif
#if FOSC == 32
	SPCR |= (1<<SPE)|(1<<MSTR)|(1<<SPR1);
	SPSR |= (1<<SPI2X);
#endif
#if FOSC == 64
	SPCR |= (1<<SPE)|(1<<MSTR)|(1<<SPR1);
#endif
#if FOSC == 128
	SPCR |= (1<<SPE)|(1<<MSTR)|(1<<SPR1)|(1<<SPR0);
#endif
//*******************************************************************************
#endif
}

void SendSPI(uint8_t bajt)
{
	SPDR = bajt;
	while( !(SPSR & (1<<SPIF)) );
}

uint8_t ReceiveSPI()
{
	while( !(SPSR & (1<<SPIF)) );
	return SPDR;
}

uint8_t TransferSPI(uint8_t bajt)
{
	SPDR = bajt;
	while( !(SPSR & (1<<SPIF)) );
	return SPDR;
}


