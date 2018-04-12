/*
 * SSD_1306.c
 *
 *  Created on: 17-12-2015
 *      Author: Mefiu
 */



#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>

#include "SSD1306.h"

#if SSD1306_USE_I2C == 1
#include "../I2C_TWI/i2c_twi.h"
#endif

static uint8_t ssd1306_buffer[SSD1306_LCDHEIGHT * SSD1306_LCDWIDTH / 8];
const uint8_t *font;
uint8_t txt_size;
int cursor_x, cursor_y;

//*********Funkcje inicjujace SPI**********
#if SSD1306_USE_SPI == 1
static void SPIwrite(uint8_t data)
{
	uint8_t i;
	for(i = 0x80; i; i >>= 1)
	{
		SCK_LO;
		if(data & i) MOSI_HI;
		else MOSI_LO;
		SCK_HI;
	}
}

void ssd1306_SPIinit()
{
	MOSI_DDR |= MOSI;

	SCK_DDR |= SCK;
	SCK_PORT |= SCK;
#if USE_RST == 1
	RST_DDR |= RST;
	RST_PORT |= RST;
#endif

	DC_DDR |= DC;

#if USE_CS == 1
	CS_DDR |= CS;
	CS_PORT |= CS;
#endif
}
#endif


//***********Wyslanie komendy**********
void ssd1306_cmd(uint8_t cmd)
{
#if SSD1306_USE_SPI == 1
#if SSD1306_USE_CS == 1
	CS_HI;
#endif

	DC_LO;

#if SSD1306_USE_CS == 1
	CS_LO;
#endif
	SPIwrite(cmd);

#if SSD1306_USE_CS == 1
	CS_LO;
#endif
#endif
#if SSD1306_USE_I2C ==1
	uint8_t control = 0x00;
	TWI_start();
	TWI_write(SSD1306_I2C_ADDRESS);
	TWI_write(control);
	TWI_write(cmd);
	TWI_stop();
#endif
}

//***********Wyslanie danych**********
void ssd1306_data(uint8_t data)
{
#if SSD1306_USE_SPI == 1
#if SSD1306_USE_CS == 1
	CS_HI;
#endif

	DC_HI;

#if SSD1306_USE_CS == 1
	CS_LO;
#endif

	SPIwrite(data);

#if SSD1306_USE_CS == 1
	CS_LO;
#endif
#endif
#if SSD1306_USE_I2C ==1
	uint8_t control = 0x40;
	TWI_start();
	TWI_write(SSD1306_I2C_ADDRESS);
	TWI_write(control);
	TWI_write(data);
	TWI_stop();
#endif
}

//***********Inicjalizacja LCD**********
void ssd1306_init(uint8_t vcc, uint8_t refresh)
{
#if SSD1306_USE_SPI == 1
	ssd1306_SPIinit();

#if SSD1306_USE_RST == 1
	RST_HI;
	_delay_ms(25);
	RST_LO;
	_delay_ms(25);
	RST_HI;
#endif
#if SSD1306_USE_CS ==1
	CS_HI;
	_delay_ms(25);
	CS_LO;
	_delay_ms(25);
	CS_HI;
#endif
#endif
#if SSD1306_USE_I2C ==1
	i2cSetBitrate(400);
	I2C_DIR |= (1<<SCL)|(1<<SDA);
	_delay_ms(50);
#endif

	ssd1306_cmd(SSD1306_DISPLAYOFF);
	ssd1306_cmd(SSD1306_SETDISPLAYCLOCKDIV);
	ssd1306_cmd(refresh);

	ssd1306_cmd(SSD1306_SETDISPLAYOFFSET);
	ssd1306_cmd(0x0);
	ssd1306_cmd(SSD1306_SETSTARTLINE | 0x0);
	ssd1306_cmd(SSD1306_CHARGEPUMP);
    if (vcc == SSD1306_EXTERNALVCC) ssd1306_cmd(0x10);
    else ssd1306_cmd(0x14);

	ssd1306_cmd(SSD1306_MEMORYMODE);
	ssd1306_cmd(0x00);
	ssd1306_cmd(SSD1306_SEGREMAP | 0x1);
	ssd1306_cmd(SSD1306_COMSCANDEC);

	ssd1306_cmd(SSD1306_SETCONTRAST);

	if (vcc == SSD1306_EXTERNALVCC) ssd1306_cmd(0x9F);
	    else ssd1306_cmd(0xCF);

	ssd1306_cmd(SSD1306_SETPRECHARGE);

#if defined SSD1306_128_32
	ssd1306_cmd(SSD1306_SETMULTIPLEX);
	ssd1306_cmd(0x1F);
	ssd1306_cmd(SSD1306_SETCOMPINS);
	ssd1306_cmd(0x02);
#endif

#if defined SSD1306_128_64
	ssd1306_cmd(SSD1306_SETMULTIPLEX);
	ssd1306_cmd(0x3F);
	ssd1306_cmd(SSD1306_SETCOMPINS);
	ssd1306_cmd(0x12);
#endif

	ssd1306_cmd(SSD1306_SETVCOMDETECT);
	ssd1306_cmd(0x40);

	ssd1306_cmd(SSD1306_DISPLAYALLON_RESUME);
	ssd1306_cmd(SSD1306_NORMALDISPLAY);

	ssd1306_cmd(SSD1306_DISPLAYON);

}

//***********Wyslanie bufora do LCD**********
void ssd1306_display()
{
	ssd1306_cmd(SSD1306_SETLOWCOLUMN | 0x0);
	ssd1306_cmd(SSD1306_SETHIGHCOLUMN | 0x0);
	ssd1306_cmd(SSD1306_SETSTARTLINE | 0x0);

#if SSD1306_USE_SPI == 1
#if SSD1306_USE_CS == 1
	CS_HI;
#endif
	DC_HI;
#if SSD1306_USE_CS == 1
	CS_LO;
#endif

		for(uint16_t i = 0; i<(SSD1306_LCDWIDTH*SSD1306_LCDHEIGHT/8); i++)
			SPIwrite(ssdssd1306_buffer);
#if SSD1306_USE_CS == 1
	CS_HI;
#endif
#endif
#if SSD1306_USE_I2C == 1
	TWI_write_buf(SSD1306_I2C_ADDRESS, 0x40, (SSD1306_LCDHEIGHT*SSD1306_LCDWIDTH/8), ssd1306_buffer);
#endif
}

void ssd1306_cls()
{
	memset(ssd1306_buffer, 0x00, (SSD1306_LCDHEIGHT*SSD1306_LCDWIDTH/8));
	//cursor_y = cursor_x = 0;
}

//**********Ustawienie pixela w buforze**********
void ssd1306_DrawPixel(int x, int y, uint8_t color)
{
	if ((x < 0) || (x >= SSD1306_LCDWIDTH) || (y < 0) || (y >= SSD1306_LCDHEIGHT)) return;

	if(color) ssd1306_buffer[x+ (y/8)*SSD1306_LCDWIDTH] |= (1<<(y%8));
		else ssd1306_buffer[x+ (y/8)*SSD1306_LCDWIDTH] &= ~(1<<(y%8));
}

//**********Rysowanie bitmapy z Flash w buforze**********
void ssd1306_DrawBitmap_P(int x, int y, const uint8_t *bitmap, uint8_t w, uint8_t h, uint8_t color)
{
	int i, j, byteWidth = (w + 7)/8;

	for(j=0; j<h; j++) {
	    for(i=0; i<w; i++ ) {
	      if(pgm_read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7))) {
	        ssd1306_DrawPixel(x+i, y+j, color);
	      }
	    }
	  }
}

//**********Rysowanie pojedynczego znaku w buforze**********
void ssd1306_DrawChar(char c, uint8_t color, uint8_t bg)
{
	  if((cursor_x >= SSD1306_LCDWIDTH) || (cursor_y >= SSD1306_LCDHEIGHT) || ((cursor_x + 6 * txt_size - 1) < 0) || ((cursor_y + 8 * txt_size - 1) < 0))
	    return;

	  uint8_t line;

	  for (int8_t i=0; i<6; i++ )
	  {
	      if (i == 5) line = 0x0;
	      else line = pgm_read_byte(font+(c*5)+i);

	      for (int8_t j = 0; j<8; j++)
	      {
	        if (line & 0x1)
	        {
	          if (txt_size == 1)
	        	  ssd1306_DrawPixel(cursor_x+i, cursor_y+j, color);
	          else
	          {
	        	  ssd1306_FillRect(cursor_x+(i*txt_size), cursor_y+(j*txt_size), txt_size, txt_size, color);
	          }
	        }
	        else if (bg != color)
	        {
	          if (txt_size == 1)
	        	  ssd1306_DrawPixel(cursor_x+i, cursor_y+j, bg);
	          else
	          {
	        	  ssd1306_FillRect(cursor_x+i*txt_size, cursor_y+j*txt_size, txt_size, txt_size, bg);
	          }
	        }
	        line >>= 1;
	      }
	   }
	  cursor_x += 6*txt_size;
}


void ssd1306_FillRect(int x, int y, int w, int h, uint8_t color)
{
	  for (int8_t i=x; i<x+w; i++)
	    ssd1306_DrawFastVLine(i, y, h, color);
}


void ssd1306_DrawFastVLine(int x, int y, int h, uint8_t color)
{
	ssd1306_DrawLine(x, y, x, y+h-1, color);
}


void ssd1306_DrawLine(int x0, int y0, int x1, int y1, uint8_t color)
{
	int steep = abs(y1 -y0) > abs(x1 - x0);
	        if (steep)
	        {
	        	swap(x0, y0);
	        	swap(x1, y1);
	        }

	        if (x0 > x1)
	        {
	        	swap(x0, x1);
	        	swap(y0, y1);
	        }

	        int dx, dy;
	        dx = x1 - x0;
	        dy = abs(y1 - y0);

	        int err = dx / 2;
	        int ystep;

	        if (y0 < y1)
	        {
	        	ystep = 1;
	        }
	        else
	        {
	        	ystep = -1;
	        }

	        for (; x0 <= x1; x0++)
	        {
	        	if (steep)
	        	{
	        		ssd1306_DrawPixel(y0, x0, color);
	        	}
	        	else
	        	{
	        		ssd1306_DrawPixel(x0, y0, color);
	        	}
	            err -= dy;
	            if (err < 0)
	            {
	            	y0 += ystep;
	            	err += dx;
	            }
	        }
}


void ssd1306_DrawFastHLine(int x, int y, int w, uint8_t color)
{
  ssd1306_DrawLine(x, y, x+w-1, y, color);
}

void ssd1306_ChangeFont(const uint8_t *newfont)
{
	font = newfont;
}

void ssd1306_ChangeTextSize(uint8_t newsize)
{
	txt_size = newsize;
}

void ssd1306_Set_Cursor(int x, int y)
{
	cursor_x = x;
	cursor_y = y;
}


void ssd1306_Puts(char *str, uint8_t color, uint8_t bg)
{
	while( *str )
	{
		ssd1306_DrawChar(*str++, color, bg);
		//cursor_x += 6*txt_size;
	}
}

void ssd1306_Put_int(int data, uint8_t color, uint8_t bg)
{
	char buf[16];
	ssd1306_Puts(itoa(data, buf, 10), color, bg);
}
