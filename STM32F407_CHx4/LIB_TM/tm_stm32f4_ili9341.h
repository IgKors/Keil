/**
 * @author  Tilen Majerle
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.com
 * @link    http://stm32f4-discovery.com/2014/04/library-08-ili9341-lcd-on-stm32f429-discovery-board/
 * @version v1.3
 * @ide     Keil uVision
 * @license GNU GPL v3
 * @brief   ILI9341 library for STM32F4xx with SPI communication, without LTDC hardware
 *	
@verbatim
   ----------------------------------------------------------------------
    Copyright (C) Tilen Majerle, 2015
    
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.
     
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
   ----------------------------------------------------------------------
@endverbatim
 */
#ifndef TM_ILI9341_H
#define TM_ILI9341_H 130

/**
 * @addtogroup TM_STM32F4xx_Libraries
 * @{
 */

/**
 * @defgroup TM_ILI9341
 * @brief    ILI9341 library for STM32F4xx with SPI communication, without LTDC hardware - http://stm32f4-discovery.com/2014/04/library-08-ili9341-lcd-on-stm32f429-discovery-board/
 * @{
 *
 * This driver works for all STM32F4xx series with built in SPI peripheral.
 *	
 * \par Default pinout
 *
@verbatim
ILI9341      STM32F4xx    DESCRIPTION
		
SDO (MISO    PB4         	Output from LCD for SPI.	Not used, can be left
SCK          PB3         	SPI clock
SDI (MOSI)   PB5         	SPI master output

WRX or D/C   PB6	        Data/Command register
RESET        PB8          Reset LCD
CS           PB7	        Chip select for SPI

LED          3.3V         Backlight
GND          GND          Ground
VCC          3.3V         Positive power supply
@endverbatim
 *		
 * All pins can be changed in your defines.h file
 *		
@verbatim
//Default SPI used is SPI5. Check my SPI library for other pinouts
#define ILI9341_SPI           SPI5
#define ILI9341_SPI_PINS      TM_SPI_PinsPack_1
		
//Default CS pin. Edit this in your defines.h file
#define ILI9341_CS_PORT       GPIOC
#define ILI9341_CS_PIN        GPIO_PIN_2
		
//Default D/C (or WRX) pin. Edit this in your defines.h file
#define ILI9341_WRX_PORT      GPIOD
#define ILI9341_WRX_PIN       GPIO_PIN_13
@endverbatim
 *
 * Reset pin can be disabled, if you need GPIOs for other purpose.
 * To disable RESET pin, add line below to defines.h file
 * If you disable pin, then set LCD's RESET pin to VCC.
 *	
@verbatim
//Disable RESET pin
#define ILI9341_USE_RST_PIN			0
@endverbatim
 *		
 * But if you want to use RESET pin, you can change its settings in defines.h file
 *	
@verbatim
//Default RESET pin. Edit this in your defines.h file
#define ILI9341_RST_PORT			GPIOD
#define ILI9341_RST_PIN				GPIO_PIN_12
@endverbatim
 *
 * \par Changelog
 *
@verbatim
 Version 1.3
  - June 06, 2015
  - Added support for SPI DMA for faster refreshing
 
 Version 1.2
  - March 14, 2015
  - Added support for new GPIO system
  - Added functions TM_ILI9341_DisplayOff() and TM_ILI9341_DisplayOn()
 
 Version 1.0
  - First release
@endverbatim
 *
 * \par Dependencies
 *
@verbatim
 - STM32F4xx
 - STM32F4xx RCC
 - STM32F4xx GPIO
 - STM32F4xx SPI
 - defines.h
 - TM SPI
 - TM DMA
 - TM SPI DMA
 - TM FONTS
 - TM GPIO
@endverbatim
 */

#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "defines.h"
#include "tm_stm32f4_fonts.h"
#include "tm_stm32f4_gpio.h"
#include "tm_stm32f4_spi.h"
#include "tm_stm32f4_dma.h"
#include "tm_stm32f4_spi_dma.h"
#include "tm_stm32f4_delay.h"

/**
 * @defgroup TM_ILI9341_Macros
 * @brief    Library defines
 * @{
 */

/**
 * @brief  This SPI-3 PACK-1 pins are used on STM32F407VET6
 * SDO (MISO    PB4         Output from LCD for SPI.	Not used, can be left
 * SCK          PB3         SPI clock
 * SDI (MOSI)   PB5         SPI master output
 */
#ifndef ILI9341_SPI
#define ILI9341_SPI           SPI3
#define ILI9341_SPI_PINS      TM_SPI_PinsPack_1
#endif

/**
 * @brief  CS PIN for SPI, used as on STM32F407VET6
 * WRX or D/C   PB6	         Data/Command register
 * RESET        PB8          Reset LCD
 * CS           PB7	         Chip select for SPI
*/
#ifndef ILI9341_CS_PIN
#define ILI9341_CS_PORT       GPIOB
#define ILI9341_CS_PIN        GPIO_PIN_7
#endif

/**
 * @brief  WRX PIN for data/command, used as on STM32F407VET6
 */
#ifndef ILI9341_WRX_PIN
#define ILI9341_WRX_PORT      GPIOB
#define ILI9341_WRX_PIN       GPIO_PIN_6
#endif

/**
 * @brief  RESET for LCD
 */
#ifndef ILI9341_RST_PIN
#define ILI9341_RST_PORT      GPIOB
#define ILI9341_RST_PIN       GPIO_PIN_8
#endif

/* LCD settings */
#define ILI9341_WIDTH        240
#define ILI9341_HEIGHT       320
#define ILI9341_PIXEL        76800

/* Colors */
#define ILI9341_COLOR_WHITE			0xFFFF
#define ILI9341_COLOR_BLACK			0x0000
#define ILI9341_COLOR_RED       0xF800
#define ILI9341_COLOR_GREEN			0x3FE9 //0x07E0
#define ILI9341_COLOR_GREEN1		0x0300
#define ILI9341_COLOR_GREEN2		0xB723
#define ILI9341_COLOR_GREEN3		0x3386
#define ILI9341_COLOR_GREEN4		0x7FA3
#define ILI9341_COLOR_BLUE			0x001F //0x008F темнее	// сдвинут на 3 бита 0x00E8 	// эталон 0x001F
#define ILI9341_COLOR_BLUE1			0x000E // Темный
#define ILI9341_COLOR_BLUE2			0x019D
#define ILI9341_COLOR_BLUE3			0x051D
#define ILI9341_COLOR_YELLOW		0xFFE0
#define ILI9341_COLOR_YELLOW1		0xFFEF
#define ILI9341_COLOR_YELLOW2		0xFFC0 //0xFFEC
#define ILI9341_COLOR_ORANGE		0xFBE4
#define ILI9341_COLOR_ORANGE1		0xF364
#define ILI9341_COLOR_CYAN			0x07FF
#define ILI9341_COLOR_MAGENTA		0xA254
#define ILI9341_COLOR_GRAY			0x7BEF
#define ILI9341_COLOR_BROWN			0xBBCAs
/*
#define ILI9341_COLOR_WHITE			0xFFFF
#define ILI9341_COLOR_BLACK			0x0000
#define ILI9341_COLOR_RED       0xC007 //	0xF800
#define ILI9341_COLOR_GREEN			0x3F00 // 0x07E0 Сдвиг на 3 бита <---
#define ILI9341_COLOR_GREEN2		0x3FE9 // 
#define ILI9341_COLOR_GREEN1		0x7FA3 // !!! Хаки
#define ILI9341_COLOR_GREEN3		0xB723 //
#define ILI9341_COLOR_GREEN4		0x11fa
#define ILI9341_COLOR_BLUE0 		0x00f8 // 0x001F Стандартный синий
#define ILI9341_COLOR_BLUE			0x001F // 0x00E8 !!! Средне Темный
#define ILI9341_COLOR_BLUE1			0x00D8 // 0x00В8 Очень  Темный
#define ILI9341_COLOR_BLUE2			0x051D
#define ILI9341_COLOR_BLUE3			0x37fA
#define ILI9341_COLOR_BLUE4			0x11ba // !!! Ярко-синий 
#define ILI9341_COLOR_BLUE5			0x11fe
#define ILI9341_COLOR_YELLOW		0xFF07 // 0xFFE0
#define ILI9341_COLOR_YELLOW2		0xDDC3
#define ILI9341_COLOR_YELLOW3		0xFFD3 // !!! Светло-желтый
#define ILI9341_COLOR_YELLOW4		0xFEC7
#define ILI9341_COLOR_ORANGE		0xFBE4
#define ILI9341_COLOR_CYAN			0x07FF
#define ILI9341_COLOR_MAGENTA		0xA254
#define ILI9341_COLOR_GRAY			0x7BEF
#define ILI9341_COLOR_BROWN			0xBBCA
*/

/* Transparent background, only for strings and chars */
#define ILI9341_TRANSPARENT			0x80000000

/**
 * @}
 */

// перевод цвета RGB в uint16_t
#define COLOR(r, g, b)\
     ((((uint16_t)b) >> 3) |\
            ((((uint16_t)g) << 3) & 0x07E0) |\
            ((((uint16_t)r) << 8) & 0xf800))
						
/**
 * @defgroup TM_ILI9341_Typedefs
 * @brief    Library Typedefs
 * @{
 */


/**
 * @brief  Possible orientations for LCD
 */
typedef enum {
	TM_ILI9341_Orientation_Portrait_1,  /*!< Portrait orientation mode 1 */
	TM_ILI9341_Orientation_Portrait_2,  /*!< Portrait orientation mode 2 */
	TM_ILI9341_Orientation_Landscape_1, /*!< Landscape orientation mode 1 */
	TM_ILI9341_Orientation_Landscape_2  /*!< Landscape orientation mode 2 */
} TM_ILI9341_Orientation_t;

/**
 * @}
 */
typedef enum
{
    FONT_1X = 1,      // ??????? ?????? ?????? 5x7
    FONT_2X = 2,      // ??????????? ?????? ??????
    FONT_3X = 3,
    FONT_4X = 4,
    FONT_5X = 5,
    FONT_6X = 6,
    FONT_7X = 7,
    FONT_8X = 8
} LcdFontSize;
/**
 * @defgroup TM_ILI9341_Functions
 * @brief    Library Functions
 * @{
 */

/**
 * @brief  Initializes ILI9341 LCD with LTDC peripheral
 *         It also initializes external SDRAM
 * @param  None
 * @retval None
 */
void TM_ILI9341_Init(void);

/**
 * @brief  Draws single pixel to LCD
 * @param  x: X position for pixel
 * @param  y: Y position for pixel
 * @param  color: Color of pixel
 * @retval None
 */
void TM_ILI9341_DrawPixel(uint16_t x, uint16_t y, uint32_t color);

/**
 * @brief  Fills entire LCD with color
 * @param  color: Color to be used in fill
 * @retval None
 */

void TM_ILI9341_INT_Fill(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);

void TM_ILI9341_Fill(uint32_t color);

/**
 * @brief  Rotates LCD to specific orientation
 * @param  orientation: LCD orientation. This parameter can be a value of @ref TM_ILI9341_Orientation_t enumeration
 * @retval None
 */
void TM_ILI9341_Rotate(TM_ILI9341_Orientation_t orientation);

/**
 * @brief  Puts single character to LCD
 * @param  x: X position of top left corner
 * @param  y: Y position of top left corner
 * @param  c: Character to be displayed
 * @param  *font: Pointer to @ref TM_FontDef_t used font
 * @param  foreground: Color for char
 * @param  background: Color for char background
 * @retval None
 */
extern void SendChar(uint16_t ch, uint16_t x, uint16_t y, uint16_t color, uint16_t backColor, LcdFontSize textSize); 
void TM_ILI9341_Putc(uint16_t x, uint16_t y, char c, TM_FontDef_t* font, uint32_t foreground, uint32_t background);

/**
 * @brief  Puts string to LCD
 * @param  x: X position of top left corner of first character in string
 * @param  y: Y position of top left corner of first character in string
 * @param  *str: Pointer to first character
 * @param  *font: Pointer to @ref TM_FontDef_t used font
 * @param  foreground: Color for string
 * @param  background: Color for string background
 * @retval None
 */
extern void SendString(const char *dataPtr, uint16_t x, uint16_t y, uint16_t color, uint16_t backColor, LcdFontSize TextSize);
 
void TM_ILI9341_Puts(uint16_t x, uint16_t y, char* str, TM_FontDef_t *font, uint32_t foreground, uint32_t background);

/**
 * @brief  Gets width and height of box with text
 * @param  *str: Pointer to first character
 * @param  *font: Pointer to @ref TM_FontDef_t used font
 * @param  *width: Pointer to variable to store width
 * @param  *height: Pointer to variable to store height
 * @retval None
 */
void TM_ILI9341_GetStringSize(char* str, TM_FontDef_t* font, uint16_t* width, uint16_t* height);

/**
 * @brief  Draws line to LCD
 * @param  x0: X coordinate of starting point
 * @param  y0: Y coordinate of starting point
 * @param  x1: X coordinate of ending point
 * @param  y1: Y coordinate of ending point
 * @param  color: Line color
 * @retval None
 */
void TM_ILI9341_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t color);

/**
 * @brief  Draws rectangle on LCD
 * @param  x0: X coordinate of top left point
 * @param  y0: Y coordinate of top left point
 * @param  x1: X coordinate of bottom right point
 * @param  y1: Y coordinate of bottom right point
 * @param  color: Rectangle color
 * @retval None
 */
void TM_ILI9341_DrawRectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t color);

/**
 * @brief  Draws filled rectangle on LCD
 * @param  x0: X coordinate of top left point
 * @param  y0: Y coordinate of top left point
 * @param  x1: X coordinate of bottom right point
 * @param  y1: Y coordinate of bottom right point
 * @param  color: Rectangle color
 * @retval None
 */
void TM_ILI9341_DrawFilledRectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t color);

/**
 * @brief  Draws circle on LCD
 * @param  x0: X coordinate of center circle point
 * @param  y0: Y coordinate of center circle point
 * @param  r: Circle radius
 * @param  color: Circle color
 * @retval None
 */
void TM_ILI9341_DrawCircle(int16_t x0, int16_t y0, int16_t r, uint32_t color);

/**
 * @brief  Draws filled circle on LCD
 * @param  x0: X coordinate of center circle point
 * @param  y0: Y coordinate of center circle point
 * @param  r: Circle radius
 * @param  color: Circle color
 * @retval None
 */
void TM_ILI9341_DrawFilledCircle(int16_t x0, int16_t y0, int16_t r, uint32_t color);

/**
 * @brief   Enables display
 * @note    After initialization, LCD is enabled and you don't need to call this function
 * @param   None
 * @retval  None
 */
void TM_ILI9341_DisplayOn(void);

/**
 * @brief   Disables display
 * @param   None
 * @retval  None
 */
void TM_ILI9341_DisplayOff(void);

/**
 * @}
 */
static const uint8_t FontLookup [][5] =
{
   { 0x00, 0x00, 0x00, 0x00, 0x00 },   //   0x20  32
   { 0x00, 0x00, 0x5F, 0x00, 0x00 },   // ! 0x21  33
   { 0x00, 0x07, 0x00, 0x07, 0x00 },   // " 0x22  34
   { 0x14, 0x7F, 0x14, 0x7F, 0x14 },   // # 0x23  35
   { 0x24, 0x2A, 0x7F, 0x2A, 0x12 },   // $ 0x24  36
   { 0x4C, 0x2C, 0x10, 0x68, 0x64 },   // % 0x25  37
   { 0x36, 0x49, 0x55, 0x22, 0x50 },   // & 0x26  38
   { 0x00, 0x05, 0x03, 0x00, 0x00 },   // ' 0x27  39
   { 0x00, 0x1C, 0x22, 0x41, 0x00 },   // ( 0x28  40
   { 0x00, 0x41, 0x22, 0x1C, 0x00 },   // ) 0x29  41
   { 0x14, 0x08, 0x3E, 0x08, 0x14 },   // * 0x2A  42
   { 0x08, 0x08, 0x3E, 0x08, 0x08 },   // + 0x2B  43
   { 0x00, 0x00, 0x50, 0x30, 0x00 },   // , 0x2C  44
   { 0x10, 0x10, 0x10, 0x10, 0x10 },   // - 0x2D  45
   { 0x00, 0x60, 0x60, 0x00, 0x00 },   // . 0x2E  46
   { 0x20, 0x10, 0x08, 0x04, 0x02 },   // / 0x2F  47
   { 0x3E, 0x51, 0x49, 0x45, 0x3E },   // 0 0x30  48
   { 0x00, 0x42, 0x7F, 0x40, 0x00 },   // 1 0x31  49
   { 0x42, 0x61, 0x51, 0x49, 0x46 },   // 2 0x32  50
   { 0x21, 0x41, 0x45, 0x4B, 0x31 },   // 3 0x33  51
   { 0x18, 0x14, 0x12, 0x7F, 0x10 },   // 4 0x34  52
   { 0x27, 0x45, 0x45, 0x45, 0x39 },   // 5 0x35  53
   { 0x3C, 0x4A, 0x49, 0x49, 0x30 },   // 6 0x36  54
   { 0x01, 0x71, 0x09, 0x05, 0x03 },   // 7 0x37  55
   { 0x36, 0x49, 0x49, 0x49, 0x36 },   // 8 0x38  56
   { 0x06, 0x49, 0x49, 0x29, 0x1E },   // 9 0x39  57
   { 0x00, 0x36, 0x36, 0x00, 0x00 },   // : 0x3A  58
   { 0x00, 0x56, 0x36, 0x00, 0x00 },   // ; 0x3B  59
   { 0x08, 0x14, 0x22, 0x41, 0x00 },   // < 0x3C  60
   { 0x14, 0x14, 0x14, 0x14, 0x14 },   // = 0x3D  61
   { 0x00, 0x41, 0x22, 0x14, 0x08 },   // > 0x3E  62
   { 0x02, 0x01, 0x51, 0x09, 0x06 },   // ? 0x3F  63
   { 0x32, 0x49, 0x79, 0x41, 0x3E },   // @ 0x40  64
   { 0x7E, 0x11, 0x11, 0x11, 0x7E },   // A 0x41  65
   { 0x7F, 0x49, 0x49, 0x49, 0x36 },   // B 0x42  66
   { 0x3E, 0x41, 0x41, 0x41, 0x22 },   // C 0x43  67
   { 0x7F, 0x41, 0x41, 0x22, 0x1C },   // D 0x44  68
   { 0x7F, 0x49, 0x49, 0x49, 0x41 },   // E 0x45  69
   { 0x7F, 0x09, 0x09, 0x09, 0x01 },   // F 0x46  70
   { 0x3E, 0x41, 0x49, 0x49, 0x7A },   // G 0x47  71
   { 0x7F, 0x08, 0x08, 0x08, 0x7F },   // H 0x48  72
   { 0x00, 0x41, 0x7F, 0x41, 0x00 },   // I 0x49  73
   { 0x20, 0x40, 0x41, 0x3F, 0x01 },   // J 0x4A  74
   { 0x7F, 0x08, 0x14, 0x22, 0x41 },   // K 0x4B  75
   { 0x7F, 0x40, 0x40, 0x40, 0x40 },   // L 0x4C  76
   { 0x7F, 0x02, 0x0C, 0x02, 0x7F },   // M 0x4D  77
   { 0x7F, 0x04, 0x08, 0x10, 0x7F },   // N 0x4E  78
   { 0x3E, 0x41, 0x41, 0x41, 0x3E },   // O 0x4F  79
   { 0x7F, 0x09, 0x09, 0x09, 0x06 },   // P 0x50  80
   { 0x3E, 0x41, 0x51, 0x21, 0x5E },   // Q 0x51  81
   { 0x7F, 0x09, 0x19, 0x29, 0x46 },   // R 0x52  82
   { 0x46, 0x49, 0x49, 0x49, 0x31 },   // S 0x53  83
   { 0x01, 0x01, 0x7F, 0x01, 0x01 },   // T 0x54  84
   { 0x3F, 0x40, 0x40, 0x40, 0x3F },   // U 0x55  85
   { 0x1F, 0x20, 0x40, 0x20, 0x1F },   // V 0x56  86
   { 0x3F, 0x40, 0x38, 0x40, 0x3F },   // W 0x57  87
   { 0x63, 0x14, 0x08, 0x14, 0x63 },   // X 0x58  88
   { 0x07, 0x08, 0x70, 0x08, 0x07 },   // Y 0x59  89
   { 0x61, 0x51, 0x49, 0x45, 0x43 },   // Z 0x5A  90
   { 0x00, 0x7F, 0x41, 0x41, 0x00 },   // [ 0x5B  91
   { 0x02, 0x04, 0x08, 0x10, 0x20 },   // \ 0x5C  92
   { 0x00, 0x41, 0x41, 0x7F, 0x00 },   // ] 0x5D  93
   { 0x04, 0x02, 0x01, 0x02, 0x04 },   // ^ 0x5E  94
   { 0x40, 0x40, 0x40, 0x40, 0x40 },   // _ 0x5F  95
   { 0x00, 0x01, 0x02, 0x04, 0x00 },   // ` 0x60  96
   { 0x20, 0x54, 0x54, 0x54, 0x78 },   // a 0x61  97
   { 0x7F, 0x48, 0x44, 0x44, 0x38 },   // b 0x62  98
   { 0x38, 0x44, 0x44, 0x44, 0x20 },   // c 0x63  99
   { 0x38, 0x44, 0x44, 0x48, 0x7F },   // d 0x64 100
   { 0x38, 0x54, 0x54, 0x54, 0x18 },   // e 0x65 101
   { 0x08, 0x7E, 0x09, 0x01, 0x02 },   // f 0x66 102
   { 0x0C, 0x52, 0x52, 0x52, 0x3E },   // g 0x67 103
   { 0x7F, 0x08, 0x04, 0x04, 0x78 },   // h 0x68 104
   { 0x00, 0x44, 0x7D, 0x40, 0x00 },   // i 0x69 105
   { 0x20, 0x40, 0x44, 0x3D, 0x00 },   // j 0x6A 106
   { 0x7F, 0x10, 0x28, 0x44, 0x00 },   // k 0x6B 107
   { 0x00, 0x41, 0x7F, 0x40, 0x00 },   // l 0x6C 108
   { 0x7C, 0x04, 0x18, 0x04, 0x78 },   // m 0x6D 109
   { 0x7C, 0x08, 0x04, 0x04, 0x78 },   // n 0x6E 110
   { 0x38, 0x44, 0x44, 0x44, 0x38 },   // o 0x6F 111
   { 0x7C, 0x14, 0x14, 0x14, 0x08 },   // p 0x70 112
   { 0x08, 0x14, 0x14, 0x18, 0x7C },   // q 0x71 113
   { 0x7C, 0x08, 0x04, 0x04, 0x08 },   // r 0x72 114
   { 0x48, 0x54, 0x54, 0x54, 0x20 },   // s 0x73 115
   { 0x04, 0x3F, 0x44, 0x40, 0x20 },   // t 0x74 116
   { 0x3C, 0x40, 0x40, 0x20, 0x7C },   // u 0x75 117
   { 0x1C, 0x20, 0x40, 0x20, 0x1C },   // v 0x76 118
   { 0x3C, 0x40, 0x30, 0x40, 0x3C },   // w 0x77 119
   { 0x44, 0x28, 0x10, 0x28, 0x44 },   // x 0x78 120
   { 0x0C, 0x50, 0x50, 0x50, 0x3C },   // y 0x79 121
   { 0x44, 0x64, 0x54, 0x4C, 0x44 },   // z 0x7A 122
   { 0x00, 0x08, 0x36, 0x41, 0x00 },   // { 0x7B 123
   { 0x00, 0x00, 0x7F, 0x00, 0x00 },   // | 0x7C 124
   { 0x00, 0x41, 0x36, 0x08, 0x00 },   // } 0x7D 125
   { 0x08, 0x04, 0x08, 0x10, 0x08 },   // ~ 0x7E 126
   { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },   //  0x7F 127

   { 0x7C, 0x12, 0x11, 0x12, 0x7C },   // А 0xC0 192
   { 0x7F, 0x49, 0x49, 0x49, 0x31 },   // Б 0xC1 193
   { 0x7F, 0x49, 0x49, 0x49, 0x36 },   // В 0xC2 194
   { 0x7F, 0x01, 0x01, 0x01, 0x01 },   // Г 0xC3 195
   { 0x60, 0x3F, 0x21, 0x3F, 0x60 },   // Д 0xC4 196
   { 0x7F, 0x49, 0x49, 0x49, 0x41 },   // Е 0xC5 197
   { 0x77, 0x08, 0x7F, 0x08, 0x77 },   // Ж 0xC6 198
   { 0x22, 0x41, 0x49, 0x49, 0x36 },   // З 0xC7 199
   { 0x7F, 0x10, 0x08, 0x04, 0x7F },   // И 0xC8 200
   { 0x7E, 0x10, 0x09, 0x04, 0x7E },   // Й 0xC9 201
   { 0x7F, 0x08, 0x14, 0x22, 0x41 },   // К 0xCA 202
   { 0x40, 0x3E, 0x01, 0x01, 0x7F },   // Л 0xCB 203
   { 0x7F, 0x02, 0x0C, 0x02, 0x7F },   // М 0xCC 204
   { 0x7F, 0x08, 0x08, 0x08, 0x7F },   // Н 0xCD 205
   { 0x3E, 0x41, 0x41, 0x41, 0x3E },   // О 0xCE 206
   { 0x7F, 0x01, 0x01, 0x01, 0x7F },   // П 0xCF 207
   { 0x7F, 0x09, 0x09, 0x09, 0x06 },   // Р 0xD0 208
   { 0x3E, 0x41, 0x41, 0x41, 0x22 },   // С 0xD1 209
   { 0x01, 0x01, 0x7F, 0x01, 0x01 },   // Т 0xD2 210
   { 0x07, 0x48, 0x48, 0x48, 0x3F },   // У 0xD3 211
   { 0x0E, 0x11, 0x7F, 0x11, 0x0E },   // Ф 0xD4 212
   { 0x63, 0x14, 0x08, 0x14, 0x63 },   // Х 0xD5 213
   { 0x3F, 0x20, 0x20, 0x3F, 0x60 },   // Ц 0xD6 214
   { 0x07, 0x08, 0x08, 0x08, 0x7F },   // Ч 0xD7 215
   { 0x7F, 0x40, 0x7E, 0x40, 0x7F },   // Ш 0xD8 216
   { 0x3F, 0x20, 0x3F, 0x20, 0x7F },   // Щ 0xD9 217
   { 0x01, 0x7F, 0x48, 0x48, 0x30 },   // Ь 0xDA 218
	 { 0x7F, 0x48, 0x78, 0x00, 0x7F },   // Ы 0xDB 219
   { 0x00, 0x7F, 0x48, 0x48, 0x30 },   // Ь 0xDC 220
   { 0x22, 0x41, 0x49, 0x49, 0x3E },   // Э 0xDD 221
   { 0x7F, 0x08, 0x3E, 0x41, 0x3E },   // Ю 0xDE 222
   { 0x46, 0x29, 0x19, 0x09, 0x7F },   // Я 0xDF 223
   { 0x20, 0x54, 0x54, 0x54, 0x78 },   // а 0xE0 224
   { 0x3C, 0x4A, 0x4A, 0x4A, 0x31 },   // б 0xE1 225
   { 0x7C, 0x54, 0x54, 0x28, 0x00 },   // в 0xE2 226
   { 0x7C, 0x04, 0x04, 0x0C, 0x00 },   // г 0xE3 227
   { 0x60, 0x3C, 0x24, 0x3C, 0x60 },   // д 0xE4 228
   { 0x38, 0x54, 0x54, 0x54, 0x18 },   // е 0xE5 229
   { 0x6C, 0x10, 0x7C, 0x10, 0x6C },   // ж 0xE6 230
   { 0x00, 0x44, 0x54, 0x54, 0x28 },   // з 0xE7 231
   { 0x7C, 0x20, 0x10, 0x08, 0x7C },   // и 0xE8 232
   { 0x7C, 0x21, 0x12, 0x09, 0x7C },   // й 0xE9 233
   { 0x7C, 0x10, 0x28, 0x44, 0x00 },   // к 0xEA 234
   { 0x40, 0x38, 0x04, 0x04, 0x7C },   // л 0xEB 235
   { 0x7C, 0x08, 0x10, 0x08, 0x7C },   // м 0xEC 236
   { 0x7C, 0x10, 0x10, 0x10, 0x7C },   // н 0xED 237
   { 0x38, 0x44, 0x44, 0x44, 0x38 },   // о 0xEE 238
   { 0x7C, 0x04, 0x04, 0x04, 0x7C },   // п 0xEF 239
   { 0x7C, 0x14, 0x14, 0x14, 0x08 },   // р 0xF0 240
   { 0x38, 0x44, 0x44, 0x44, 0x00 },   // с 0xF1 241
   { 0x04, 0x04, 0x7C, 0x04, 0x04 },   // т 0xF2 242
   { 0x0C, 0x50, 0x50, 0x50, 0x3C },   // у 0xF3 243
   { 0x08, 0x14, 0x7C, 0x14, 0x08 },   // ф 0xF4 244
   { 0x44, 0x28, 0x10, 0x28, 0x44 },   // х 0xF5 245
   { 0x3C, 0x20, 0x20, 0x3C, 0x60 },   // ц 0xF6 246
   { 0x0C, 0x10, 0x10, 0x10, 0x7C },   // ч 0xF7 247
   { 0x7C, 0x40, 0x7C, 0x40, 0x7C },   // ш 0xF8 248
   { 0x3C, 0x20, 0x3C, 0x20, 0x7C },   // щ 0xF9 249
   { 0x00, 0x44, 0x7D, 0x44, 0x00 },   // ь 0xFA 250
   { 0x7C, 0x50, 0x20, 0x00, 0x7C },   // ы 0xFB 251
   { 0x00, 0x7C, 0x50, 0x50, 0x20 },   // ъ 0xFC 252
   { 0x28, 0x44, 0x54, 0x54, 0x38 },   // э 0xFD 253
   { 0x7C, 0x10, 0x38, 0x44, 0x38 },   // ю 0xFE 254
   { 0x48, 0x54, 0x34, 0x14, 0x7C }    // я 0xFF 255
};
/**
 * @}
 */
 
/**
 * @}
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif
