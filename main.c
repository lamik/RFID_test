/*
 * main.c
 *
 *  Created on: 17-12-2015
 *      Author: Mefiu
 */
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>

#include "SPI/SPI.h"
#include "OLED_SSD1306/SSD1306.h"
#include "RFID_MFRC522/MFRC522.h"

extern const uint8_t font5x7[];

int main(void)
{
	ssd1306_ChangeFont(font5x7);

	ssd1306_init(SSD1306_SWITCHCAPVCC, REFRESH_MIN);
	ssd1306_cls();
	ssd1306_display();

	_delay_ms(100);

	MFRC522_InitSPI_Pins();
	_delay_ms(100);
	InitSPI();
	_delay_ms(100);

	MFRC522_Init();

	while(1)
	{
		uint8_t status;
		uint8_t data[16];
		uint8_t serial[5];
		uint8_t type[2];

		ssd1306_cls();
		ssd1306_ChangeTextSize(1);
		ssd1306_Set_Cursor(0,1);
		ssd1306_Puts("RFID test",1,0);

		status = MFRC522_RequestTag(MF1_REQIDL,data);

		if(status == MI_OK)
		{
			type[0] = data[0];
			type[1] = data[1];
			status = MFRC522_AntiCollision(data);
			memcpy(serial, data, 5);
		}

		ssd1306_Set_Cursor(0,20);
		ssd1306_Puts("Type:",1,0);
		ssd1306_Put_int(type[0],1,0);
		ssd1306_Puts(",",1,0);
		ssd1306_Put_int(type[1],1,0);

		ssd1306_Set_Cursor(0,30);
		ssd1306_Puts("Serial:",1,0);
	    for(int i = 0; i < 3; i++) {
	      ssd1306_Put_int(serial[i],1,0);
	      ssd1306_Puts(" ",1,0);
	    }
	    ssd1306_Put_int(serial[3],1,0);

		ssd1306_display();
		_delay_ms(100);
	}
}

