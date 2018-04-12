/*
 * SSD_1306.h
 *
 *  Created on: 17-12-2015
 *      Author: Mefiu
 */

#ifndef SSD_1306_H_
#define SSD_1306_H_

#define SSD1306_128_64
//#define SSD1306_128_32

#define SSD1306_USE_I2C 1
#define SSD1306_USE_SPI 0



#if SSD1306_USE_I2C == 1
#define SSD1306_I2C_ADDRESS   0x78 // lub 0x7A
#define I2C_DIR DDRC
#define SCL PC0
#define SDA PC1
#endif

#if SSD1306_USE_SPI == 1

#define USE_CS 0
#define USE_RST 0

#define SCK (1<<7)
#define MOSI (1<<6)
#define RST (1<<5)
#define DC (1<<4)
#define CS (1<<5)

#define SCK_PORT PORTC
#define SCK_DDR DDRC

#define MOSI_PORT PORTC
#define MOSI_DDR DDRC

#define RST_PORT PORTC
#define RST_DDR DDRC

#define DC_PORT PORTC
#define DC_DDR DDRC

#define CS_PORT PORTC
#define CS_DDR DDRC
#if USE_RST == 1
#define RST_LO RST_PORT &= ~RST
#define RST_HI RST_PORT |= RST
#endif
#if USE_CS == 1
#define CS_LO CS_PORT &= ~CS
#define CS_HI CS_PORT |= CS
#endif

#define DC_LO DC_PORT &= ~DC
#define DC_HI DC_PORT |= DC

#define SCK_LO SCK_PORT &= ~SCK
#define SCK_HI SCK_PORT |= SCK
#endif
#define SSD1306_LCDWIDTH 128

#if defined SSD1306_128_64
#define SSD1306_LCDHEIGHT 64
#endif
#if defined SSD1306_128_32
#define SSD1306_LCDHEIGHT                 32
#endif

#define REFRESH_MIN 0x80
#define REFRESH_MID 0xB0
#define REFRESH_MAX 0xF0

#define SSD1306_SETCONTRAST 0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF

#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETCOMPINS 0xDA

#define SSD1306_SETVCOMDETECT 0xDB

#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9

#define SSD1306_SETMULTIPLEX 0xA8

#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10

#define SSD1306_SETSTARTLINE 0x40

#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLUMNADDR 0x21
#define SSD1306_PAGEADDR   0x22

#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8

#define SSD1306_SEGREMAP 0xA0

#define SSD1306_CHARGEPUMP 0x8D

#define SSD1306_EXTERNALVCC 0x1
#define SSD1306_SWITCHCAPVCC 0x2

// Scrolling #defines
#define SSD1306_ACTIVATE_SCROLL 0x2F
#define SSD1306_DEACTIVATE_SCROLL 0x2E
#define SSD1306_SET_VERTICAL_SCROLL_AREA 0xA3
#define SSD1306_RIGHT_HORIZONTAL_SCROLL 0x26
#define SSD1306_LEFT_HORIZONTAL_SCROLL 0x27
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL 0x2A

#define swap(a, b) {uint16_t t = a; a = b; b = t;}

//Deklaracje zmiennych
const uint8_t *font;
extern uint8_t txt_size;
extern int cursor_x, cursor_y;
extern const uint8_t font_5x7;

//Deklaracje funkcji
void ssd1306_cmd(uint8_t cmd);
void ssd1306_data(uint8_t data);
void ssd1306_init(uint8_t vcc, uint8_t refresh);

void ssd1306_display();
void ssd1306_cls();
void ssd1306_DrawPixel(int x, int y, uint8_t color);
void ssd1306_DrawBitmap_P(int x, int y, const uint8_t *bitmap, uint8_t w, uint8_t h, uint8_t color);
void ssd1306_FillRect(int x, int y, int w, int h, uint8_t color);
void ssd1306_DrawLine(int x0, int y0, int x1, int y1, uint8_t color);
void ssd1306_DrawFastVLine(int x, int y, int h, uint8_t color);
void ssd1306_DrawFastHLine(int x, int y, int w, uint8_t color);

void ssd1306_ChangeFont(const uint8_t *newfont);
void ssd1306_ChangeTextSize(uint8_t newsize);
void ssd1306_Set_Cursor(int x, int y);

void ssd1306_DrawChar(char c, uint8_t color, uint8_t bg);
void ssd1306_Puts(char *str, uint8_t color, uint8_t bg);
void ssd1306_Put_int(int data, uint8_t color, uint8_t bg);


#endif /* SSD_1306_H_ */
