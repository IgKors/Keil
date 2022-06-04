/**	
 * |----------------------------------------------------------------------
 * | Copyright (C) Tilen Majerle, 2014
 * | 
 * | This program is free software: you can redistribute it and/or modify
 * | it under the terms of the GNU General Public License as published by
 * | the Free Software Foundation, either version 3 of the License, or
 * | any later version.
 * |  
 * | This program is distributed in the hope that it will be useful,
 * | but WITHOUT ANY WARRANTY; without even the implied warranty of
 * | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * | GNU General Public License for more details.
 * | 
 * | You should have received a copy of the GNU General Public License
 * | along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * |----------------------------------------------------------------------
 */
#include "tm_stm32f4_ili9341.h"

typedef struct Digole_XLAT{
uint16_t unicode;
char win;
}DigoleXLAT;

DigoleXLAT Digolechars[66]={
{0xd191,0xb8},
{0xd190,0xa8},
{0xd090,0xc0},
{0xd091,0xc1},
{0xd092,0xc2},
{0xd093,0xc3},
{0xd094,0xc4},
{0xd095,0xc5},
{0xd096,0xc6},
{0xd097,0xc7},
{0xd098,0xc8},
{0xd099,0xc9},
{0xd09a,0xca},
{0xd09b,0xcb},
{0xd09c,0xcc},
{0xd09d,0xcd},
{0xd09e,0xce},
{0xd09f,0xcf},
{0xd0a0,0xd0},
{0xd0a1,0xd1},
{0xd0a2,0xd2},
{0xd0a3,0xd3},
{0xd0a4,0xd4},
{0xd0a5,0xd5},
{0xd0a6,0xd6},
{0xd0a7,0xd7},
{0xd0a8,0xd8},
{0xd0a9,0xd9},
{0xd0aa,0xda},
{0xd0ab,0xdb},
{0xd0ac,0xdc},
{0xd0ad,0xdd},
{0xd0ae,0xde},
{0xd0af,0xdf},
{0xd0b0,0xe0},
{0xd0b1,0xe1},
{0xd0b2,0xe2},
{0xd0b3,0xe3},
{0xd0b4,0xe4},
{0xd0b5,0xe5},
{0xd0b6,0xe6},
{0xd0b7,0xe7},
{0xd0b8,0xe8},
{0xd0b9,0xe9},
{0xd0ba,0xea},
{0xd0bb,0xeb},
{0xd0bc,0xec},
{0xd0bd,0xed},
{0xd0be,0xee},
{0xd0bf,0xef},
{0xd180,0xf0},
{0xd181,0xf1},
{0xd182,0xf2},
{0xd183,0xf3},
{0xd184,0xf4},
{0xd185,0xf5},
{0xd186,0xf6},
{0xd187,0xf7},
{0xd188,0xf8},
{0xd189,0xf9},
{0xd18a,0xfa},
{0xd18b,0xfb},
{0xd18c,0xfc},
{0xd18d,0xfd},
{0xd18e,0xfe},
{0xd18f,0xff},
};

/**
 * @brief  Orientation
 * @note   Used private
 */
typedef enum {
	TM_ILI9341_Landscape,
	TM_ILI9341_Portrait
} TM_ILI9341_Orientation;

/**
 * @brief  LCD options
 * @note   Used private
 */
typedef struct {
	uint16_t width;
	uint16_t height;
	TM_ILI9341_Orientation orientation; // 1 = portrait; 0 = landscape
} TM_ILI931_Options_t;

/* Pin definitions */
#define ILI9341_RST_SET				GPIO_SetBits(ILI9341_RST_PORT, ILI9341_RST_PIN)
#define ILI9341_RST_RESET			GPIO_ResetBits(ILI9341_RST_PORT, ILI9341_RST_PIN)
#define ILI9341_CS_SET				GPIO_SetBits(ILI9341_CS_PORT, ILI9341_CS_PIN)
#define ILI9341_CS_RESET			GPIO_ResetBits(ILI9341_CS_PORT, ILI9341_CS_PIN)
#define ILI9341_WRX_SET				GPIO_SetBits(ILI9341_WRX_PORT, ILI9341_WRX_PIN)
#define ILI9341_WRX_RESET			GPIO_ResetBits(ILI9341_WRX_PORT, ILI9341_WRX_PIN)

/* Private defines */
#define ILI9341_RESET						0x01
#define ILI9341_SLEEP_OUT				0x11
#define ILI9341_GAMMA						0x26
#define ILI9341_DISPLAY_OFF			0x28
#define ILI9341_DISPLAY_ON			0x29
#define ILI9341_COLUMN_ADDR			0x2A
#define ILI9341_PAGE_ADDR				0x2B
#define ILI9341_GRAM						0x2C
#define ILI9341_MAC							0x36
#define ILI9341_PIXEL_FORMAT		0x3A
#define ILI9341_WDB							0x51
#define ILI9341_WCD							0x53
#define ILI9341_RGB_INTERFACE		0xB0
#define ILI9341_FRC							0xB1
#define ILI9341_BPC							0xB5
#define ILI9341_DFC							0xB6
#define ILI9341_POWER1					0xC0
#define ILI9341_POWER2					0xC1
#define ILI9341_VCOM1						0xC5
#define ILI9341_VCOM2						0xC7
#define ILI9341_POWERA					0xCB
#define ILI9341_POWERB					0xCF
#define ILI9341_PGAMMA					0xE0
#define ILI9341_NGAMMA					0xE1
#define ILI9341_DTCA						0xE8
#define ILI9341_DTCB						0xEA
#define ILI9341_POWER_SEQ				0xED
#define ILI9341_3GAMMA_EN				0xF2
#define ILI9341_INTERFACE				0xF6
#define ILI9341_PRC							0xF7

/* Pin functions */
uint16_t ILI9341_x;
uint16_t ILI9341_y;
TM_ILI931_Options_t ILI9341_Opts;
uint8_t ILI9341_INT_CalledFromPuts = 0;

/* Private functions */
void TM_ILI9341_InitLCD(void);
void TM_ILI9341_SendData(uint8_t data);
void TM_ILI9341_SendCommand(uint8_t data);
void TM_ILI9341_Delay(volatile unsigned int delay);
void TM_ILI9341_SetCursorPosition(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void TM_ILI9341_INT_Fill(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);

void TM_ILI9341_Init() {
	/* Init WRX pin */
	TM_GPIO_Init(ILI9341_WRX_PORT, ILI9341_WRX_PIN, TM_GPIO_Mode_OUT, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_Medium);
	
	/* Init CS pin */
	TM_GPIO_Init(ILI9341_CS_PORT, ILI9341_CS_PIN, TM_GPIO_Mode_OUT, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_Medium);
	
	/* Init RST pin */
	TM_GPIO_Init(ILI9341_RST_PORT, ILI9341_RST_PIN, TM_GPIO_Mode_OUT, TM_GPIO_OType_PP, TM_GPIO_PuPd_UP, TM_GPIO_Speed_Low);

	/* CS high */
	ILI9341_CS_SET;
	
	/* Init SPI */
	TM_SPI_Init(ILI9341_SPI, ILI9341_SPI_PINS);
	
	/* Init DMA for SPI */
	TM_SPI_DMA_Init(ILI9341_SPI);
	
	/* Init LCD */
	TM_ILI9341_InitLCD();	
	
	/* Set default settings */
	ILI9341_x = ILI9341_y = 0;
	ILI9341_Opts.width = ILI9341_WIDTH;
	ILI9341_Opts.height = ILI9341_HEIGHT;
	ILI9341_Opts.orientation = TM_ILI9341_Portrait;
	
	/* Fill with white color */
	TM_ILI9341_Fill(0x0733 ); //ILI9341_COLOR_BLUE  font - ILI9341_COLOR_WHITE
}

void TM_ILI9341_InitLCD(void) {
	/* Force reset */
	//TM_ILI9341_Delay(20000);
	ILI9341_RST_RESET;
	//TM_ILI9341_Delay(20000);
	Delayms(20);
	ILI9341_RST_SET;
	
	/* Delay for RST response */
	//TM_ILI9341_Delay(20000);
	Delay(10);
	
	/* Software reset */
	TM_ILI9341_SendCommand(ILI9341_RESET);
	//TM_ILI9341_Delay(50000);
	Delayms(5);
	
	TM_ILI9341_SendCommand(ILI9341_POWERA);
	TM_ILI9341_SendData(0x39);
	TM_ILI9341_SendData(0x2C);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendData(0x34);
	TM_ILI9341_SendData(0x02);
	TM_ILI9341_SendCommand(ILI9341_POWERB);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendData(0xC1);
	TM_ILI9341_SendData(0x30);
	TM_ILI9341_SendCommand(ILI9341_DTCA);
	TM_ILI9341_SendData(0x85);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendData(0x78);
	TM_ILI9341_SendCommand(ILI9341_DTCB);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendCommand(ILI9341_POWER_SEQ);
	TM_ILI9341_SendData(0x64);
	TM_ILI9341_SendData(0x03);
	TM_ILI9341_SendData(0x12);
	TM_ILI9341_SendData(0x81);
	TM_ILI9341_SendCommand(ILI9341_PRC);
	TM_ILI9341_SendData(0x20);
	TM_ILI9341_SendCommand(ILI9341_POWER1);
	TM_ILI9341_SendData(0x23);
	TM_ILI9341_SendCommand(ILI9341_POWER2);
	TM_ILI9341_SendData(0x10);
	TM_ILI9341_SendCommand(ILI9341_VCOM1);
	TM_ILI9341_SendData(0x3E);
	TM_ILI9341_SendData(0x28);
	TM_ILI9341_SendCommand(ILI9341_VCOM2);
	TM_ILI9341_SendData(0x86);
	TM_ILI9341_SendCommand(ILI9341_MAC);
	TM_ILI9341_SendData(0x48);
	TM_ILI9341_SendCommand(ILI9341_PIXEL_FORMAT);
	TM_ILI9341_SendData(0x55);
	TM_ILI9341_SendCommand(ILI9341_FRC);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendData(0x18);
	TM_ILI9341_SendCommand(ILI9341_DFC);
	TM_ILI9341_SendData(0x08);
	TM_ILI9341_SendData(0x82);
	TM_ILI9341_SendData(0x27);
	TM_ILI9341_SendCommand(ILI9341_3GAMMA_EN);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendCommand(ILI9341_COLUMN_ADDR);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendData(0xEF);
	TM_ILI9341_SendCommand(ILI9341_PAGE_ADDR);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendData(0x01);
	TM_ILI9341_SendData(0x3F);
	TM_ILI9341_SendCommand(ILI9341_GAMMA);
	
	TM_ILI9341_SendData(0x01);
	TM_ILI9341_SendCommand(ILI9341_PGAMMA);
	TM_ILI9341_SendData(0x1f); // 0x0F
	TM_ILI9341_SendData(0x1a); // 0x31
	TM_ILI9341_SendData(0x18); // 0x2B
	TM_ILI9341_SendData(0x0a); // 0x0C
	TM_ILI9341_SendData(0x0f); // 0x0E
	TM_ILI9341_SendData(0x06); // 0x08
	TM_ILI9341_SendData(0x45); // 0x4E
	TM_ILI9341_SendData(0x87); // 0xF1
	TM_ILI9341_SendData(0x32); // 0x37
	TM_ILI9341_SendData(0x0a); // 0x07
	TM_ILI9341_SendData(0x07); // 0x10
	TM_ILI9341_SendData(0x02); // 0x03
	TM_ILI9341_SendData(0x07); // 0x0E
	TM_ILI9341_SendData(0x05); // 0x09
	TM_ILI9341_SendData(0x00); // 0x00
	TM_ILI9341_SendCommand(ILI9341_NGAMMA);
	TM_ILI9341_SendData(0x00); // 0x00
	TM_ILI9341_SendData(0x25); // 0x0E
	TM_ILI9341_SendData(0x27); // 0x14
	TM_ILI9341_SendData(0x05); // 0x03
	TM_ILI9341_SendData(0x10); // 0x11
	TM_ILI9341_SendData(0x09); // 0x07
	TM_ILI9341_SendData(0x3a); // 0x31
	TM_ILI9341_SendData(0x78); // 0xC1
	TM_ILI9341_SendData(0x4d); // 0x48
	TM_ILI9341_SendData(0x05); // 0x08
	TM_ILI9341_SendData(0x18); // 0x0F
	TM_ILI9341_SendData(0x0d); // 0x0C
	TM_ILI9341_SendData(0x38); // 0x31
	TM_ILI9341_SendData(0x3a); // 0x36
	TM_ILI9341_SendData(0x1f); // 0x0F
	
	/*
	// Temnaia
	TM_ILI9341_SendCommand(ILI9341_PGAMMA);
	TM_ILI9341_SendData(0x0F);
	TM_ILI9341_SendData(0x31);
	TM_ILI9341_SendData(0x2B);
	TM_ILI9341_SendData(0x0C);
	TM_ILI9341_SendData(0x0E);
	TM_ILI9341_SendData(0x08);
	TM_ILI9341_SendData(0x4E);
	TM_ILI9341_SendData(0xF1);
	TM_ILI9341_SendData(0x37);
	TM_ILI9341_SendData(0x07);
	TM_ILI9341_SendData(0x10);
	TM_ILI9341_SendData(0x03);
	TM_ILI9341_SendData(0x0E);
	TM_ILI9341_SendData(0x09);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendCommand(ILI9341_NGAMMA);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendData(0x0E);
	TM_ILI9341_SendData(0x14);
	TM_ILI9341_SendData(0x03);
	TM_ILI9341_SendData(0x11);
	TM_ILI9341_SendData(0x07);
	TM_ILI9341_SendData(0x31);
	TM_ILI9341_SendData(0xC1);
	TM_ILI9341_SendData(0x48);
	TM_ILI9341_SendData(0x08);
	TM_ILI9341_SendData(0x0F);
	TM_ILI9341_SendData(0x0C);
	TM_ILI9341_SendData(0x31);
	TM_ILI9341_SendData(0x36);
	TM_ILI9341_SendData(0x0F);
	*/
	TM_ILI9341_SendCommand(ILI9341_SLEEP_OUT);

	//TM_ILI9341_Delay(1000000);
	Delayms(20);

	TM_ILI9341_SendCommand(ILI9341_DISPLAY_ON);
	TM_ILI9341_SendCommand(ILI9341_GRAM);
}

void TM_ILI9341_DisplayOn(void) {
	TM_ILI9341_SendCommand(ILI9341_DISPLAY_ON);
}

void TM_ILI9341_DisplayOff(void) {
	TM_ILI9341_SendCommand(ILI9341_DISPLAY_OFF);
}

void TM_ILI9341_SendCommand(uint8_t data) {
	ILI9341_WRX_RESET;
	ILI9341_CS_RESET;
	TM_SPI_Send(ILI9341_SPI, data);
	ILI9341_CS_SET;
}

void TM_ILI9341_SendData(uint8_t data) {
	ILI9341_WRX_SET;
	ILI9341_CS_RESET;
	TM_SPI_Send(ILI9341_SPI, data);
	ILI9341_CS_SET;
}

void TM_ILI9341_DrawPixel(uint16_t x, uint16_t y, uint32_t color) {
	TM_ILI9341_SetCursorPosition(x, y, x, y);

	TM_ILI9341_SendCommand(ILI9341_GRAM);
	TM_ILI9341_SendData(color >> 8);
	TM_ILI9341_SendData(color & 0xFF);
}


void TM_ILI9341_SetCursorPosition(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
	TM_ILI9341_SendCommand(ILI9341_COLUMN_ADDR);
	TM_ILI9341_SendData(x1 >> 8);
	TM_ILI9341_SendData(x1 & 0xFF);
	TM_ILI9341_SendData(x2 >> 8);
	TM_ILI9341_SendData(x2 & 0xFF);

	TM_ILI9341_SendCommand(ILI9341_PAGE_ADDR);
	TM_ILI9341_SendData(y1 >> 8);
	TM_ILI9341_SendData(y1 & 0xFF);
	TM_ILI9341_SendData(y2 >> 8);
	TM_ILI9341_SendData(y2 & 0xFF);
}

void TM_ILI9341_Fill(uint32_t color) {
	/* Fill entire screen */
	TM_ILI9341_INT_Fill(0, 0, ILI9341_Opts.width - 1, ILI9341_Opts.height, color);
}

void TM_ILI9341_INT_Fill(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color) {
	uint32_t pixels_count;
	
	/* Set cursor position */
	TM_ILI9341_SetCursorPosition(x0, y0, x1, y1);

	/* Set command for GRAM data */
	TM_ILI9341_SendCommand(ILI9341_GRAM);
	
	/* Calculate pixels count */
	pixels_count = (x1 - x0 + 1) * (y1 - y0 + 1);

	/* Send everything */
	ILI9341_CS_RESET;
	ILI9341_WRX_SET;
	
	/* Go to 16-bit SPI mode */
	TM_SPI_SetDataSize(ILI9341_SPI, TM_SPI_DataSize_16b);
	
	/* Send first 65535 bytes, SPI MUST BE IN 16-bit MODE */
	TM_SPI_DMA_SendHalfWord(ILI9341_SPI, color, (pixels_count > 0xFFFF) ? 0xFFFF : pixels_count);
	/* Wait till done */
	while (TM_SPI_DMA_Working(ILI9341_SPI));
	
	/* Check again */
	if (pixels_count > 0xFFFF) {
		/* Send remaining data */
		TM_SPI_DMA_SendHalfWord(ILI9341_SPI, color, pixels_count - 0xFFFF);
		/* Wait till done */
		while (TM_SPI_DMA_Working(ILI9341_SPI));
	}
	
	ILI9341_CS_SET;

	/* Go back to 8-bit SPI mode */
	TM_SPI_SetDataSize(ILI9341_SPI, TM_SPI_DataSize_8b);
}

void TM_ILI9341_Delay(volatile unsigned int delay) {
	for (; delay != 0; delay--); 
}

void TM_ILI9341_Rotate(TM_ILI9341_Orientation_t orientation) {
	TM_ILI9341_SendCommand(ILI9341_MAC);
	if (orientation == TM_ILI9341_Orientation_Portrait_1) {
		TM_ILI9341_SendData(0x58);
	} else if (orientation == TM_ILI9341_Orientation_Portrait_2) {
		TM_ILI9341_SendData(0x88);
	} else if (orientation == TM_ILI9341_Orientation_Landscape_1) {
		TM_ILI9341_SendData(0x28);
	} else if (orientation == TM_ILI9341_Orientation_Landscape_2) {
		TM_ILI9341_SendData(0xE8);
	}
	
	if (orientation == TM_ILI9341_Orientation_Portrait_1 || orientation == TM_ILI9341_Orientation_Portrait_2) {
		ILI9341_Opts.width = ILI9341_WIDTH;
		ILI9341_Opts.height = ILI9341_HEIGHT;
		ILI9341_Opts.orientation = TM_ILI9341_Portrait;
	} else {
		ILI9341_Opts.width = ILI9341_HEIGHT;
		ILI9341_Opts.height = ILI9341_WIDTH;
		ILI9341_Opts.orientation = TM_ILI9341_Landscape;
	}
}

void TM_ILI9341_Puts(uint16_t x, uint16_t y, char *str, TM_FontDef_t *font, uint32_t foreground, uint32_t background) {
	uint16_t startX = x;
	
	/* Set X and Y coordinates */
	ILI9341_x = x;
	ILI9341_y = y;
	
	while (*str) {
		/* New line */
		if (*str == '\n') {
			ILI9341_y += font->FontHeight + 1;
			/* if after \n is also \r, than go to the left of the screen */
			if (*(str + 1) == '\r') {
				ILI9341_x = 0;
				str++;
			} else {
				ILI9341_x = startX;
			}
			str++;
			continue;
		} else if (*str == '\r') {
			str++;
			continue;
		}
		
		/* Put character to LCD */
		TM_ILI9341_Putc(ILI9341_x, ILI9341_y, *str++, font, foreground, background);
	}
}

void TM_ILI9341_GetStringSize(char *str, TM_FontDef_t *font, uint16_t *width, uint16_t *height) {
	uint16_t w = 0;
	*height = font->FontHeight;
	while (*str++) {
		w += font->FontWidth;
	}
	*width = w;
}

void TM_ILI9341_Putc(uint16_t x, uint16_t y, char c, TM_FontDef_t *font, uint32_t foreground, uint32_t background) {
	uint32_t i, b, j;
	/* Set coordinates */
	ILI9341_x = x;
	ILI9341_y = y;
	
	if ((ILI9341_x + font->FontWidth) > ILI9341_Opts.width) {
		/* If at the end of a line of display, go to new line and set x to 0 position */
		ILI9341_y += font->FontHeight;
		ILI9341_x = 0;
	}
	
	/* Draw rectangle for background */
	TM_ILI9341_INT_Fill(ILI9341_x, ILI9341_y, ILI9341_x + font->FontWidth, ILI9341_y + font->FontHeight, background);
	
	/* Draw font data */
	for (i = 0; i < font->FontHeight; i++) {
		b = font->data[(c - 32) * font->FontHeight + i];
		for (j = 0; j < font->FontWidth; j++) {
			if ((b << j) & 0x8000) {
				TM_ILI9341_DrawPixel(ILI9341_x + j, (ILI9341_y + i), foreground);
			}
		}
	}
	
	/* Set new pointer */
	ILI9341_x += font->FontWidth;
}

void SendChar(uint16_t ch, uint16_t x, uint16_t y, uint16_t color, uint16_t backColor, LcdFontSize textSize)
{
	int16_t ii = 0, c = 0, jj = 0,  cnt = 0, k = 0, yjj = 0, i = 0, bitCnt = 0;
	//uint8_t symbol;
	uint64_t mux;
	uint64_t buf[8];
	uint16_t colorofchar = color;
	uint16_t xx = x;
	uint16_t yy = y;

	if ( (ch >= 0x20) && (ch <= 0x7F) )
    {
        // ASCII[0x20-0x7F]
    	ch -= 32;
    }
    else if ( ch >= 0xC0 )
    {
        // CP1251[0xC0-0xFF]
    	ch -= 96;
    }
    else
    {
        //  
    	ch = 95;
    }

	for (i = 0; i < 2; i++)
	buf[i] = 0;


    for ( ii = 0; ii < 5; ii++ )
    {
    	jj = 0;
    	c = FontLookup[ch][ii];
    	k = 0;

    	while (k < 2)
    	{
    		mux = (uint32_t)((1 << textSize) - 1);
    		if (c & 0x01)
   			buf[k] = mux;
   			mux <<= textSize;
   			if (c & 0x02)
   			buf[k] |= mux;
   			mux <<= textSize;
   			if (c & 0x04)
   			buf[k] |= mux;
   			mux <<= textSize;
   			if (c & 0x08)
   			buf[k] |= mux;
   			c >>= 4;
   			k++;
    	}

    	bitCnt = textSize * 4;

    	for (k = 0; k < 2; k++)
    	{
    		yjj = yy + jj;
    		for (jj = 0; jj < bitCnt; jj++)
    		{
    			if ((buf[k] & 0x01) == 0x01)
    			{
    				for (cnt = 0; cnt < textSize; cnt++)
						TM_ILI9341_DrawPixel(xx + textSize * ii + cnt, yjj + jj, colorofchar);
    			}
    			else
    			{
    				for (cnt = 0; cnt < textSize; cnt++)
						TM_ILI9341_DrawPixel(xx + textSize * ii + cnt, yjj + jj, backColor);
    			}
    			buf[k] = buf[k] >> 1;
    		}
    	}
    }
}
// Вывод строки с рускими шрифтами и с размером шрифта
void SendString(const char *dataPtr, uint16_t x, uint16_t y, uint16_t color, uint16_t backColor, LcdFontSize TextSize)
{
	register char c;
	uint16_t cc;
  uint8_t i;
	c = *dataPtr++;	
  while ( (c != '\0') )
	{
		if ((c == 0xD0) || (c == 0xD1))
		{
			cc = (c << 8);
			cc |= *dataPtr++;
			for (i = 0; i < 66; i++)
			{
				if (cc == Digolechars[i].unicode)
				{
					c = Digolechars[i].win;
					break;
				}
			}
		}		
		SendChar((uint8_t) c, x, y, color, backColor, TextSize);
		x += TextSize * 6;
		//if (x > ILI9341_WIDTH - (TextSize * 6))
		if (x > 320 - (TextSize * 6))
		{
			y += TextSize * 8 + 1;
			x = 0;
		}
		c = *dataPtr++;
	}
}

void TM_ILI9341_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t color) {
	/* Code by dewoller: https://github.com/dewoller */
	
	int16_t dx, dy, sx, sy, err, e2; 	
	uint16_t tmp;
	
	/* Check for overflow */
	if (x0 >= ILI9341_Opts.width) {
		x0 = ILI9341_Opts.width - 1;
	}
	if (x1 >= ILI9341_Opts.width) {
		x1 = ILI9341_Opts.width - 1;
	}
	if (y0 >= ILI9341_Opts.height) {
		y0 = ILI9341_Opts.height - 1;
	}
	if (y1 >= ILI9341_Opts.height) {
		y1 = ILI9341_Opts.height - 1;
	}
	
	/* Check correction */
	if (x0 > x1) {
		tmp = x0;
		x0 = x1;
		x1 = tmp;
	}
	if (y0 > y1) {
		tmp = y0;
		y0 = y1;
		y1 = tmp;
	}
	
	dx = x1 - x0;
	dy = y1 - y0;
	
	/* Vertical or horizontal line */
	if (dx == 0 || dy == 0) {
		TM_ILI9341_INT_Fill(x0, y0, x1, y1, color);
		return;
	}
	
	sx = (x0 < x1) ? 1 : -1; 
	sy = (y0 < y1) ? 1 : -1; 
	err = ((dx > dy) ? dx : -dy) / 2; 

	while (1) {
		TM_ILI9341_DrawPixel(x0, y0, color); 
		if (x0 == x1 && y0 == y1) {
			break;
		}
		e2 = err; 
		if (e2 > -dx) {
			err -= dy;
			x0 += sx;
		} 
		if (e2 < dy) {
			err += dx;
			y0 += sy;
		} 
	}
}

void TM_ILI9341_DrawRectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t color) {
	TM_ILI9341_DrawLine(x0, y0, x1, y0, color); //Top
	TM_ILI9341_DrawLine(x0, y0, x0, y1, color);	//Left
	TM_ILI9341_DrawLine(x1, y0, x1, y1, color);	//Right
	TM_ILI9341_DrawLine(x0, y1, x1, y1, color);	//Bottom
}

void TM_ILI9341_DrawFilledRectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t color) {	
	uint16_t tmp;
	
	/* Check correction */
	if (x0 > x1) {
		tmp = x0;
		x0 = x1;
		x1 = tmp;
	}
	if (y0 > y1) {
		tmp = y0;
		y0 = y1;
		y1 = tmp;
	}
	
	/* Fill rectangle */
	TM_ILI9341_INT_Fill(x0, y0, x1, y1, color);
	
	/* CS HIGH back */
	ILI9341_CS_SET;
}

void TM_ILI9341_DrawCircle(int16_t x0, int16_t y0, int16_t r, uint32_t color) {
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

    TM_ILI9341_DrawPixel(x0, y0 + r, color);
    TM_ILI9341_DrawPixel(x0, y0 - r, color);
    TM_ILI9341_DrawPixel(x0 + r, y0, color);
    TM_ILI9341_DrawPixel(x0 - r, y0, color);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        TM_ILI9341_DrawPixel(x0 + x, y0 + y, color);
        TM_ILI9341_DrawPixel(x0 - x, y0 + y, color);
        TM_ILI9341_DrawPixel(x0 + x, y0 - y, color);
        TM_ILI9341_DrawPixel(x0 - x, y0 - y, color);

        TM_ILI9341_DrawPixel(x0 + y, y0 + x, color);
        TM_ILI9341_DrawPixel(x0 - y, y0 + x, color);
        TM_ILI9341_DrawPixel(x0 + y, y0 - x, color);
        TM_ILI9341_DrawPixel(x0 - y, y0 - x, color);
    }
}

void TM_ILI9341_DrawFilledCircle(int16_t x0, int16_t y0, int16_t r, uint32_t color) {
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

    TM_ILI9341_DrawPixel(x0, y0 + r, color);
    TM_ILI9341_DrawPixel(x0, y0 - r, color);
    TM_ILI9341_DrawPixel(x0 + r, y0, color);
    TM_ILI9341_DrawPixel(x0 - r, y0, color);
    TM_ILI9341_DrawLine(x0 - r, y0, x0 + r, y0, color);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        TM_ILI9341_DrawLine(x0 - x, y0 + y, x0 + x, y0 + y, color);
        TM_ILI9341_DrawLine(x0 + x, y0 - y, x0 - x, y0 - y, color);

        TM_ILI9341_DrawLine(x0 + y, y0 + x, x0 - y, y0 + x, color);
        TM_ILI9341_DrawLine(x0 + y, y0 - x, x0 - y, y0 - x, color);
    }
}

