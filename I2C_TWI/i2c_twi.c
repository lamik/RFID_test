/*
 * i2c_twi.c
 *
 *  Created on: 2010-09-07
 *       Autor: Miros³aw Kardaœ
 */
#include <avr/io.h>

#include "i2c_twi.h"

void i2cSetBitrate(uint16_t bitrateKHz) {
	uint8_t bitrate_div;

	bitrate_div = ((F_CPU/1000l)/bitrateKHz);
	if(bitrate_div >= 16)
		bitrate_div = (bitrate_div-16)/2;

	TWBR = bitrate_div;
}

void TWI_start(void) {
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTA);
	while (!(TWCR&(1<<TWINT)));
}

void TWI_stop(void) {
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
	while ( (TWCR&(1<<TWSTO)) );
}

void TWI_write(uint8_t bajt) {
	TWDR = bajt;
	TWCR = (1<<TWINT)|(1<<TWEN);
	while ( !(TWCR&(1<<TWINT)));
}

uint8_t TWI_read(uint8_t ack) {
	TWCR = (1<<TWINT)|(ack<<TWEA)|(1<<TWEN);
	while ( !(TWCR & (1<<TWINT)));
	return TWDR;
}



void TWI_write_buf( uint8_t SLA, uint8_t adr, uint16_t len, uint8_t *buf ) {

	TWI_start();
	TWI_write(SLA);
	TWI_write(adr);
	while (len--) TWI_write(*buf++);
	TWI_stop();
}



void TWI_read_buf(uint8_t SLA, uint8_t adr, uint8_t len, uint8_t *buf) {

	TWI_start();
	TWI_write(SLA);
	TWI_write(adr);
	TWI_start();
	TWI_write(SLA + 1);
	while (len--) *buf++ = TWI_read( len ? ACK : NACK );
	TWI_stop();
}
