
/****************************************************************************************
*	DDS 50хCHANAL на STM32F407VET6	для 50 иголок, **** Модиф.: для МОДУЛЯ	****					*
 *	два варианта:																																				*
 *  1. Меандр (на один контакт меандр, на 50 контактов комутация "-")										*
 *  2. Постоянка (на один контакт "+", на 50 контактов комутация "-")										*
 *	TFT ILI9341 дисплей без тачскрина																										*
 ****************************************************************************************
 
 ****************************************************************************************
 *	@author			Igor Korsunsky																																			*
 *	@email			kig@ua.fm																																*
 *	@ide				Keil uVision 5																													*
 *	@packs			STM32F4xx Keil packs version Keil.STM32F4xx_DFP.2.11.0									*
 *	@stdperiph	STM32F4xx Standard peripheral drivers version 1.4.0 or greater required *
 ****************************************************************************************/

/*  *********************************************************
		Вывода перефирии для STM32F407VET6
		_________________________________________________________
		Дисплей TFT ILI9341
		---------------------------------------------------------
		Vers/ 3.xx
		---------------------------------------------------------
		PINS PACK 1 (SPI3)
		
		MOSI   /MISO/  SCK
		PB5    /PB4/   PB3
		
		PB7 	- CS
		PB6 	- WRX PIN for data/command
		PB8 	- RESET
		+3.3V	- LED, Vcc
		GND		- GND
		_________________________________________________________
		Три КНОПКИ
		---------------------------------------------------------
		//PE11 - BUTTON1 	(SWITCH on GND) Влево
		//PE13 - BUTTON2	(SWITCH on GND) Ок
		//PE15 - BUTTON3	(SWITCH on GND) Вправо
		
		Кнопки для варианта на плате модуля
		PE5 	- BUTTON1 	(SWITCH on GND) Влево
		PA0 	- BUTTON2		(SWITCH on +5V) Ок
		PE4 	- BUTTON3		(SWITCH on GND) Вправо
		__________________________________________________________
		/Два светодиода/
		---------------------------------------------------------
		PA6	- LED1
		PA7	- LED2


		VOLTMETR (ADC 1 - CHANNEL 1, ADC2 - CHANNEL 2)
		----------------------------------------------------------
		иниц. ADC1(PA1), ADC2(PA2) 
		
		CHANNEL   ADC1   ADC2   ADC3
		1         PA1    PA1    PA1			на PA1 потенциометр
		2         PA2    PA2    PA2			на DAC1(PA4) / в этой версии отключено
		__________________________________________________________
		DAC1 - PA4 (Управление амплетудой выходных сигналов DDS)
		----------------------------------------------------------
		__________________________________________________________
		
		PWM (ШИМ)
		----------------------------------------------------------
		TIM2 - CHANNEL 1 PP2 - PA5
		TIM5 - CHANNEL 4 PP1 - PA3
		*********************************************************/
		
/* Include core modules */
#include "stm32f4xx.h"
#include "stm32f4xx_flash.h"
#include <stdio.h>

/* Include my libraries here */
#include "defines.h" 							// Выбор параметров SPI 
#include "tm_stm32f4_ili9341.h"
#include "tm_stm32f4_delay.h"
#include "tm_stm32f4_gpio.h"
#include "tm_stm32f4_adc.h"
#include "tm_stm32f4_dac.h"
#include "tm_stm32f4_pwm.h"
#include "preset.h"

#ifdef RELEASE
#warning "Protection is ON. Debug is OFF"
	if (FLASH_GetReadOutProtectionStatus() == RESET) {
		FLASH_Unlock();
		FLASH_ReadOutProtection(ENABLE);
		FLASH_Lock();
	}
#endif



	/* иниц. ADC - pin PA1 , PA2
	CHANNEL   ADC1   ADC2   ADC3
	0         PA0    PA0    PA0
	1         PA1    PA1    PA1
	2         PA2    PA2    PA2
	3         PA3    PA3    PA3
	4         PA4    PA4    PF6
	5         PA5    PA5    PF7
	6         PA6    PA6    PF8
	7         PA7    PA7    PF9
	8         PB0    PB0    PF10
	9         PB1    PB1    PF3
	10        PC0    PC0    PC0
	11        PC1    PC1    PC1
	12        PC2    PC2    PC2
	13        PC3    PC3    PC3
	14        PC4    PC4    PF4
	15        PC5    PC5    PF5
	*/
	
/* 	========================================================
		== (Перем. Резистор)ADC-PA2 -> (Controller)-> DAC-PA5 ==
		== Для теста перемычка с PA5 на PA1		 								==
		======================================================== */		

#define DDS_Init_ADC1()		TM_ADC_Init(ADC1, ADC_Channel_1)	// ADC1-PA1
#define DDS_READ_ADC1()		TM_ADC_Read(ADC1, ADC_Channel_1)	// на потенциоиетр

//#define DDS_Init_ADC2()		TM_ADC_Init(ADC2, ADC_Channel_2)	// ADC2-PA2
//#define DDS_READ_ADC2()		TM_ADC_Read(ADC2, ADC_Channel_2)  // ADC2-PA2 -> TM_DAC1/DAC-PA4

// была инверсия DAC
// в 41 строке tm_stm32f4_dac.с был буфер DAC_OutputBuffer_Enable поменял на DAC_OutputBuffer_Disable 
// DAC_InitStruct.DAC_OutputBuffer = DAC_OutputBuffer_Disable;


#define DDS_DAC 					TM_DAC1		// DAC-PA4

#define	LED1_OFF()			TM_GPIO_SetPinValue(GPIOA, GPIO_Pin_6,  1)
#define	LED1_ON()				TM_GPIO_SetPinValue(GPIOA, GPIO_Pin_6,  0)
#define	LED2_OFF()			TM_GPIO_SetPinValue(GPIOA, GPIO_Pin_7,  1)
#define	LED2_ON()				TM_GPIO_SetPinValue(GPIOA, GPIO_Pin_7,  0)

// Назначение трех кнопок на отладочной плате STM32_F4VE V2.0 1509 (STM32F407VET6)
#define ReadBUTT1 			TM_GPIO_GetInputPinValue(GPIOE, GPIO_Pin_3)	// LEFT
#define	ReadBUTT2 			TM_GPIO_GetInputPinValue(GPIOA, GPIO_Pin_0)	// OK
#define	ReadBUTT3 			TM_GPIO_GetInputPinValue(GPIOE, GPIO_Pin_4)	// RIGHT	


#define	FonScreen	ILI9341_COLOR_BLUE
enum TM_ILI9341_Orientation_Landscape_1;
int8_t 	Namb = 0,									// Номер пресета
				Menu = 0,									// Номер пункта меню
				Needles=50;								// количество Иголок


int32_t	Timer=60;									// Время на комутацию сигнала на одну иглу
int32_t	TimerFull=0;							// пересчитівается в теле функции пресета Timer * Needles



//int Timer=20;
//int	TimerFull = 1546;

// Флаг вывода частоты на дисплей
// 1 - Выводить, 0 - Нет
int8_t FreqForDispley = 0                                                         ;		

/* BUFER for RTC tim */
char buf[50];
/* Struct for RTC tim */
//TM_RTC_Time_t datatime;

float VoltKF;

//глобальные переменные	
//uint32_t freq 	= 2684355;					  //частота тактовая 2^28/100 = 2 684 354.56
//  double sfreq 	= 4294939;						//частота тактовая 2^32/1000= 4 294 967.3
  float freq 	= 400;									  //частота OUT AD9850
  float FREQ		= 0;
  float FREQ_1	= 0;
  float FREQ_2	= 0;
	float FREQ_Popravka1;
	float FREQ_Popravka2;
	


void FreqSendDisplay (int x, int y, float freq1, float freq2);

void GPIO_ALL_OUT_Init (void);
void Set_PinsNUMB_OUT(int8_t pins, int8_t vals);
void SetFreq1(float FREQ);
void SetFreq2(float FREQ);
void FreqOFF(void);
void Volt(void);
void SetVoltUP(void);
void LED1_PULS (void);
void LED2_PULS (void);
void LED_ALL_PULS (void);
void SetTimer(void);

void Page_00 (void){
	char strbuf[30];
	// Стартовая страница
	TM_ILI9341_Fill(FonScreen);
	
	sprintf(strbuf, " Перебор %.2d кнт.", Needles);		
	SendString(strbuf, 50, 40, ILI9341_COLOR_YELLOW, FonScreen, FONT_2X);
	
	SendString ("Нажмите \"",   50 , 85, ILI9341_COLOR_WHITE , FonScreen, FONT_2X);			 
	SendString("OK", 					  158, 85, ILI9341_COLOR_YELLOW, FonScreen, FONT_2X);
	SendString("\" для",	180, 85, ILI9341_COLOR_WHITE , FonScreen, FONT_2X);
	
	SendString("старта процедуры", 50, 110, ILI9341_COLOR_WHITE, FonScreen, FONT_2X);
	
	SendString("<=", 50, 160, ILI9341_COLOR_YELLOW, FonScreen, FONT_2X);
	SendString("Выбор меню", 98, 160, ILI9341_COLOR_WHITE, FonScreen, FONT_2X);
	SendString("=>", 246, 160, ILI9341_COLOR_YELLOW, FonScreen, FONT_2X);
	
	
	//SendString("<=", 40, 160, ILI9341_COLOR_YELLOW, FonScreen, FONT_2X);
	//SendString("Выбор пресета", 73, 160, ILI9341_COLOR_WHITE, FonScreen, FONT_2X);
	//SendString("=>", 236, 160, ILI9341_COLOR_YELLOW, FonScreen, FONT_2X);
	
	//SendString("Вых.Сиигн:", 40, 197, ILI9341_COLOR_WHITE, FonScreen, FONT_2X);
}

void Page_01 (void){
	// Стартовая страница
	TM_ILI9341_Fill(FonScreen);
	

	SendString("УСТАНОКА", 50, 30, ILI9341_COLOR_GREEN, FonScreen, FONT_2X);
	SendString("Таймера в сек.", 50, 50, ILI9341_COLOR_YELLOW, FonScreen, FONT_2X);
	
	SendString ("Нажмите \"",   50 , 85, ILI9341_COLOR_WHITE , FonScreen, FONT_2X);			 
	SendString("OK", 					  157, 85, ILI9341_COLOR_YELLOW, FonScreen, FONT_2X);
	SendString("\" для",	180, 85, ILI9341_COLOR_WHITE , FonScreen, FONT_2X);
	
	SendString("режима установки", 50, 110, ILI9341_COLOR_WHITE, FonScreen, FONT_2X);
	

	SendString("<=", 50, 160, ILI9341_COLOR_YELLOW, FonScreen, FONT_2X);
	SendString("Выбор меню", 98, 160, ILI9341_COLOR_WHITE, FonScreen, FONT_2X);
	SendString("=>", 246, 160, ILI9341_COLOR_YELLOW, FonScreen, FONT_2X);
	
	//SendString("Вых.Сиигн:", 40, 197, ILI9341_COLOR_WHITE, FonScreen, FONT_2X);
}

void Page_02 (void){
	// Стартовая страница
	TM_ILI9341_Fill(FonScreen);
	
	SendString("На одиночный кнт.", 50, 40, ILI9341_COLOR_YELLOW, FonScreen, FONT_2X);
	
	SendString ("Нажмите \"",   50 , 85, ILI9341_COLOR_WHITE , FonScreen, FONT_2X);			 
	SendString("OK", 					  157, 85, ILI9341_COLOR_YELLOW, FonScreen, FONT_2X);
	SendString("\" для",	180, 85, ILI9341_COLOR_WHITE , FonScreen, FONT_2X);
	
	SendString("старта процедуры", 50, 110, ILI9341_COLOR_WHITE, FonScreen, FONT_2X);
	

	SendString("<=", 50, 160, ILI9341_COLOR_YELLOW, FonScreen, FONT_2X);
	SendString("Выбор меню", 98, 160, ILI9341_COLOR_WHITE, FonScreen, FONT_2X);
	SendString("=>", 246, 160, ILI9341_COLOR_YELLOW, FonScreen, FONT_2X);
	
}


	
void GPIO_ALL_OUT_Init (void) {
	
	
	/* FOR 2 x LED || 8, 9, 10, 11, 12
	Port A | Pin OUT
	-------+--------
		6		 |		LED
		7 	 |		LED
	*/
	TM_GPIO_Init(GPIOA, GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12, 
	TM_GPIO_Mode_OUT, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High);
	
	
	/* out 9, 10, 11, 12, 13, 14, 12, 13, 14 15
	Port B | Pin OUT
	-------+--------
	*/
	TM_GPIO_Init(GPIOB, GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14| GPIO_Pin_15 , 
	TM_GPIO_Mode_OUT, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High);
	
	
	/* 0, 1, 2, 3, 6, 7, 8, 9, 10, 11, 12, 13
	Port C | Pin OUT
	-------+--------
	*/
	TM_GPIO_Init(GPIOC, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12, 
	TM_GPIO_Mode_OUT, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High);

	/*
	Port D | Pin OUT
	-------+--------
	*/
	TM_GPIO_Init(GPIOD, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2  | GPIO_Pin_3  | GPIO_Pin_4  | GPIO_Pin_5  | GPIO_Pin_6 | GPIO_Pin_7 | 
											GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14| GPIO_Pin_15 , 
	TM_GPIO_Mode_OUT, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High);
	
	/* 0, 1, 2, 3, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15
	Port E | Pin OUT
	*/
/* For MODULE	
	TM_GPIO_Init(GPIOE, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_6 | GPIO_Pin_7  | 
											GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14| GPIO_Pin_15 , 
	TM_GPIO_Mode_OUT, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High);
*/

	TM_GPIO_Init(GPIOE, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_7  | 
											GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_12 |GPIO_Pin_14, 
	TM_GPIO_Mode_OUT, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High);

}


void BUTTON_Init (void)
{
	// PINs INPUT
	// BUTTON1, BUTTON3 PE4, PE5

	TM_GPIO_Init(GPIOE, GPIO_Pin_3 | GPIO_Pin_4, 
	TM_GPIO_Mode_IN, TM_GPIO_OType_PP, TM_GPIO_PuPd_UP, TM_GPIO_Speed_High);


	// PINs INPUT
	// BUTTON2, BUTTON3 PA0

	TM_GPIO_Init(GPIOA, GPIO_Pin_0, 
	TM_GPIO_Mode_IN, TM_GPIO_OType_PP, TM_GPIO_PuPd_DOWN, TM_GPIO_Speed_High);	
}

void Set_PinsNUMB_OUT(int8_t pins, int8_t vals) {
	switch (pins)
	{
		case 1:  TM_GPIO_SetPinValue(GPIOE, GPIO_Pin_1,  vals); break;
		case 2:  TM_GPIO_SetPinValue(GPIOE, GPIO_Pin_0,  vals); break;
		case 3:  TM_GPIO_SetPinValue(GPIOB, GPIO_Pin_9,  vals); break;
		case 4:  TM_GPIO_SetPinValue(GPIOC, GPIO_Pin_0,  vals); break;
		case 5:  TM_GPIO_SetPinValue(GPIOC, GPIO_Pin_1,  vals); break;
		case 6:  TM_GPIO_SetPinValue(GPIOC, GPIO_Pin_2,  vals); break;
		case 7:  TM_GPIO_SetPinValue(GPIOC, GPIO_Pin_3,  vals); break;
		case 8:  TM_GPIO_SetPinValue(GPIOD, GPIO_Pin_7,  vals); break;
		case 9:  TM_GPIO_SetPinValue(GPIOD, GPIO_Pin_6,  vals); break;
		case 10: TM_GPIO_SetPinValue(GPIOD, GPIO_Pin_5,  vals); break;
		
		case 11: TM_GPIO_SetPinValue(GPIOD, GPIO_Pin_4, vals); break;
		case 12: TM_GPIO_SetPinValue(GPIOD, GPIO_Pin_3, vals); break;
		case 13: TM_GPIO_SetPinValue(GPIOD, GPIO_Pin_2,  vals); break;
		case 14: TM_GPIO_SetPinValue(GPIOD, GPIO_Pin_1,  vals); break;
		case 15: TM_GPIO_SetPinValue(GPIOD, GPIO_Pin_0,  vals); break;
		case 16: TM_GPIO_SetPinValue(GPIOC, GPIO_Pin_12,  vals); break;
		case 17: TM_GPIO_SetPinValue(GPIOC, GPIO_Pin_11,  vals); break;
		case 18: TM_GPIO_SetPinValue(GPIOC, GPIO_Pin_10,  vals); break;
		case 19: TM_GPIO_SetPinValue(GPIOA, GPIO_Pin_12,  vals); break;
		case 20: TM_GPIO_SetPinValue(GPIOA, GPIO_Pin_11,  vals); break;
		
		case 21: TM_GPIO_SetPinValue(GPIOA, GPIO_Pin_10,  vals); break;
		case 22: TM_GPIO_SetPinValue(GPIOA, GPIO_Pin_9,  vals); break;
		case 23: TM_GPIO_SetPinValue(GPIOA, GPIO_Pin_8,  vals); break;
		case 24: TM_GPIO_SetPinValue(GPIOC, GPIO_Pin_9, vals); break;
		case 25: TM_GPIO_SetPinValue(GPIOC, GPIO_Pin_8, vals); break;
		case 26: TM_GPIO_SetPinValue(GPIOC, GPIO_Pin_7, vals); break;
		case 27: TM_GPIO_SetPinValue(GPIOC, GPIO_Pin_6, vals); break;
		case 28: TM_GPIO_SetPinValue(GPIOD, GPIO_Pin_15, vals); break;
		case 29: TM_GPIO_SetPinValue(GPIOD, GPIO_Pin_14, vals); break;
		case 30: TM_GPIO_SetPinValue(GPIOD, GPIO_Pin_13, vals); break;
		
		case 31: TM_GPIO_SetPinValue(GPIOD, GPIO_Pin_12,  vals); break;
		case 32: TM_GPIO_SetPinValue(GPIOD, GPIO_Pin_11,  vals); break;
		case 33: TM_GPIO_SetPinValue(GPIOD, GPIO_Pin_10,  vals); break;
		case 34: TM_GPIO_SetPinValue(GPIOD, GPIO_Pin_9,  vals); break;
		case 35: TM_GPIO_SetPinValue(GPIOD, GPIO_Pin_8,  vals); break;
		case 36: TM_GPIO_SetPinValue(GPIOB, GPIO_Pin_15,  vals); break;
		case 37: TM_GPIO_SetPinValue(GPIOB, GPIO_Pin_14, vals); break;
		case 38: TM_GPIO_SetPinValue(GPIOB, GPIO_Pin_13, vals); break;
		case 39: TM_GPIO_SetPinValue(GPIOB, GPIO_Pin_12, vals); break;
		case 40: TM_GPIO_SetPinValue(GPIOB, GPIO_Pin_11, vals); break;
		
		case 41: TM_GPIO_SetPinValue(GPIOE, GPIO_Pin_10, vals); break;
		case 42: TM_GPIO_SetPinValue(GPIOE, GPIO_Pin_10, vals); break;
		case 43: TM_GPIO_SetPinValue(GPIOE, GPIO_Pin_9, vals); break;
		case 44: TM_GPIO_SetPinValue(GPIOE, GPIO_Pin_8, vals); break;
		case 45: TM_GPIO_SetPinValue(GPIOE, GPIO_Pin_15, vals); break;
		case 46: TM_GPIO_SetPinValue(GPIOE, GPIO_Pin_14, vals); break;
		case 47: TM_GPIO_SetPinValue(GPIOE, GPIO_Pin_13, vals); break;
		case 48: TM_GPIO_SetPinValue(GPIOE, GPIO_Pin_12, vals); break;
		case 49: TM_GPIO_SetPinValue(GPIOE, GPIO_Pin_11, vals); break;
		case 50: TM_GPIO_SetPinValue(GPIOE, GPIO_Pin_10, vals); break;
		default: TM_GPIO_SetPinValue(GPIOE, GPIO_Pin_1, 0); break;
	}
}


void SetFreq1(float FREQ) {
	TM_PWM_TIM_t TIM5_Data;
	float kf=0;
	char chr[30];

	kf=FREQ / 100;
	FREQ_Popravka1 = (kf * 0.001)-0.005;
	
	if(FreqForDispley == 1){
		sprintf(chr, "%.5f", kf);
		TM_ILI9341_Puts(40, 220, chr, &TM_Font_11x18, ILI9341_COLOR_GREEN, FonScreen);		
		FreqSendDisplay (40, 190, FREQ, 0);
	}
	
	/*
	Тактовая частота 168 МГц период 84 Мгц (один тик 11.6 наноСекунды)
	Частота ШИМ = период тактовой/FREQ
	Получаем количество тиков по 11.6 ns заполняющих период
	*/
		TM_PWM_InitTimer(TIM5, &TIM5_Data, FREQ - FREQ_Popravka1);
	
		/* Initialize PWM on TIM4, Channel 4 and PinsPack 1 = PA3 */
		TM_PWM_InitChannel(&TIM5_Data, TM_PWM_Channel_4, TM_PWM_PinsPack_1);
	
		/* Set 50% duty cycle */
		TM_PWM_SetChannel(&TIM5_Data, TM_PWM_Channel_4, TIM5_Data.Period / 2);
}

void SetFreq2(float FREQ) {
	TM_PWM_TIM_t TIM2_Data;
	float kf=0;
	char chr[30];

	kf=FREQ / 100;
	FREQ_Popravka2 = (kf * 0.001)-0.005;

	if(FreqForDispley == 1){
		sprintf(chr, "%.5f", kf);
		TM_ILI9341_Puts(40, 220, chr, &TM_Font_11x18, ILI9341_COLOR_GREEN, FonScreen);
	}

	TM_PWM_InitTimer(TIM2, &TIM2_Data, FREQ - FREQ_Popravka2);
	
		/* Initialize PWM on TIM2, Channel 1 and PinsPack 2 = PA5 */
		TM_PWM_InitChannel(&TIM2_Data, TM_PWM_Channel_1, TM_PWM_PinsPack_2);
	
		/* Set 50% duty cycle */
		TM_PWM_SetChannel(&TIM2_Data, TM_PWM_Channel_1, TIM2_Data.Period / 2);
}

void FreqOFF(void) {
	TM_PWM_TIM_t TIM2_Data,TIM5_Data;
	
	// PWM1 TIM4, Channel 4 and PinsPack 1 = PA3
	TM_PWM_InitTimer(TIM5, &TIM5_Data, 1);
	TM_PWM_InitChannel(&TIM5_Data, TM_PWM_Channel_4, TM_PWM_PinsPack_1);
	TM_PWM_SetChannel(&TIM5_Data, TM_PWM_Channel_4, 100);
	
	// PWM2 TIM2, Channel 1 and PinsPack 2 = PA5
	TM_PWM_InitTimer(TIM2, &TIM2_Data, 1);
	TM_PWM_InitChannel(&TIM2_Data, TM_PWM_Channel_1, TM_PWM_PinsPack_2);
	TM_PWM_SetChannel(&TIM2_Data, TM_PWM_Channel_1, 100);
}

void Volt(void) {
float Volt;
int x;
	
/* Wiev data  ADC*/
/* Read ADC1 Channel 1 */
Volt=DDS_READ_ADC1();
Volt=(20.0/4096)*Volt;

//Volt=23;	
//sprintf(chrs, "%.2f V", Volt);
//Volt=(17.10/4096)*Volt;
//Volt=(100.0/4096)*Volt;
	x=(int)(Volt*10)+54;
	
	
	
//TM_ILI9341_INT_Fill ( x0,  y0,  x1,  y1, uint16_t color);
  TM_ILI9341_INT_Fill ( 54, 225, 255, 230, ILI9341_COLOR_GREEN4 );	
  TM_ILI9341_INT_Fill ( 54, 225,  x , 230, ILI9341_COLOR_YELLOW);	

	
	TM_ILI9341_Puts(52,  203, "0",   &TM_Font_7x10, ILI9341_COLOR_YELLOW, FonScreen);
	TM_ILI9341_Puts(145, 203, "0.5", &TM_Font_7x10, ILI9341_COLOR_YELLOW, FonScreen);
	TM_ILI9341_Puts(245, 203, "1mA", &TM_Font_7x10, ILI9341_COLOR_YELLOW, FonScreen);
	
	TM_ILI9341_DrawLine( 54,  218, 54,  230, ILI9341_COLOR_WHITE);
	TM_ILI9341_DrawLine( 55,  218, 55,  230, ILI9341_COLOR_WHITE);
	
	TM_ILI9341_DrawLine( 65,  220, 65,  230, ILI9341_COLOR_WHITE);
	TM_ILI9341_DrawLine( 75,  220, 75,  230, ILI9341_COLOR_WHITE);
	TM_ILI9341_DrawLine( 85,  220, 85,  230, ILI9341_COLOR_WHITE);
	TM_ILI9341_DrawLine( 95,  220, 95,  230, ILI9341_COLOR_WHITE);
	TM_ILI9341_DrawLine( 105, 220, 105, 230, ILI9341_COLOR_WHITE);
	TM_ILI9341_DrawLine( 115, 220, 115, 230, ILI9341_COLOR_WHITE);
	TM_ILI9341_DrawLine( 125, 220, 125, 230, ILI9341_COLOR_WHITE);
	TM_ILI9341_DrawLine( 135, 220, 135, 230, ILI9341_COLOR_WHITE);
	TM_ILI9341_DrawLine( 145, 220, 145, 230, ILI9341_COLOR_WHITE);
	
	TM_ILI9341_DrawLine( 155, 218, 155, 230, ILI9341_COLOR_WHITE);
	TM_ILI9341_DrawLine( 156, 218, 156, 230, ILI9341_COLOR_WHITE);
	
	TM_ILI9341_DrawLine( 165, 220, 165, 230, ILI9341_COLOR_WHITE);
	TM_ILI9341_DrawLine( 175, 220, 175, 230, ILI9341_COLOR_WHITE);
	TM_ILI9341_DrawLine( 185, 220, 185, 230, ILI9341_COLOR_WHITE);
	TM_ILI9341_DrawLine( 195, 220, 195, 230, ILI9341_COLOR_WHITE);
	TM_ILI9341_DrawLine( 205, 220, 205, 230, ILI9341_COLOR_WHITE);
  TM_ILI9341_DrawLine( 215, 220, 215, 230, ILI9341_COLOR_WHITE);
	TM_ILI9341_DrawLine( 225, 220, 225, 230, ILI9341_COLOR_WHITE);
	TM_ILI9341_DrawLine( 235, 220, 235, 230, ILI9341_COLOR_WHITE);
	TM_ILI9341_DrawLine( 245, 220, 245, 230, ILI9341_COLOR_WHITE);

	TM_ILI9341_DrawLine( 255, 218, 255, 230, ILI9341_COLOR_WHITE);
	TM_ILI9341_DrawLine( 256, 218, 256, 230, ILI9341_COLOR_WHITE);
	
//sprintf(chrs, "%.2f", Volt);
//TM_ILI9341_INT_Fill(159, 180, 319, 230, FonScreen);

//TM_ILI9341_Puts(160, 190, chrs, 
//&TM_Font_16x26, ILI9341_COLOR_CYAN, FonScreen);

//TM_ILI9341_Puts(255, 190, "%", 
//&TM_Font_16x26, ILI9341_COLOR_WHITE, FonScreen);
}

void SetVolt(void){
float Volt;
/*
CHANNEL   ADC1   ADC2   ADC3
0        /PA0    PA0    PA0/
1         PA1    PA1    PA1
2         PA2    PA2    PA2
*/

/* 	========================================================
		== (Перем. Резистор)ADC-PA2 -> (Controller)-> DAC-PA5 ==
		== Для теста перемычка с PA5 на PA1		 								==
		======================================================== */		

Volt=DDS_READ_ADC1(); 						//ADC-PA1 Считывается показания Потенциометра

TM_DAC_SetValue(DDS_DAC, Volt);		// DAC-PA4
}

void SetVoltUP(void){
float VoltUP,VoltBUF;
int i;	

TM_DAC_SetValue(DDS_DAC, 0);	

VoltUP=DDS_READ_ADC1(); 						//ADC-PA1 Считывается показания Потенциометра
VoltUP=VoltUP/100;
VoltBUF=0;
	
	for(i=0;i<100;i++) {
		VoltBUF=VoltBUF+VoltUP;
		Delayms(10);	
		TM_DAC_SetValue(DDS_DAC, VoltBUF);		// DAC-PA4
	}
}


// Вывод на дисплей частоты двух каналов, для тестового режима
void FreqSendDisplay (int x, int y, float freq1, float freq2) {	
char strbuf[30]; 
		
		sprintf(strbuf, "%.2f / %.2f Hz", 
		(float)freq1/100, (float)(freq1+freq2)/100);
		
		TM_ILI9341_Puts(x, y, strbuf, 
		&TM_Font_11x18, ILI9341_COLOR_GREEN, FonScreen);
}

void FreqSendDisplay2 (int x, int y, float freq1, float freq2) {	
char strbuf[30]; 
		
		sprintf(strbuf, "%.3f / %.3f Hz", 
		(float)freq1/1000, (float)freq2/1000);
		
		TM_ILI9341_Puts(x, y, strbuf, 
		&TM_Font_11x18, ILI9341_COLOR_GREEN, FonScreen);
}


// Вывод показания двух таймеров, 1 - таймер пресета, 2 - таймер текущей частоты пресета
void DisplayTIMER () {
	char chrs[10];
	int32_t min1,min2;
	int32_t sec1,sec2;
	

			if (Timer > 59) { min1 = Timer/60; sec1 = Timer - (min1*60);} 
			else { min1=0;	sec1 = Timer;}
	
			if (TimerFull > 59) { min2 = TimerFull/60; sec2 = TimerFull - (min2*60);} 
			else { min2=0;	sec2 = TimerFull;}
	
			/* Format time */
			sprintf(chrs, "%02d:%02d     %02d:%02d", min2, sec2, min1, sec1 );

			TM_ILI9341_Puts(40, 80, "                  ", 
			&TM_Font_16x26, ILI9341_COLOR_YELLOW, FonScreen);
			TM_ILI9341_Puts(40, 80, chrs, 
			&TM_Font_16x26, ILI9341_COLOR_YELLOW, FonScreen);
	return;
}




void LED1_PULS (void){
	LED1_ON();
	Delayms(250);
	LED1_OFF();
}

void LED2_PULS (void){
	LED2_ON(); 
	Delayms(250);
	LED2_OFF();
}

void LED_ALL_PULS (void){
	LED2_ON(); 
	LED1_ON();
	
	Delayms(100);
	
	LED2_OFF(); 
	LED1_OFF();
	Delayms(100);
}

void PageSetTimers (void) {
	TM_ILI9341_Fill(FonScreen);
	

	SendString("УСТАНВОКА", 40, 30, ILI9341_COLOR_YELLOW, FonScreen, FONT_2X);
	SendString("Таймера в сек.", 40, 50, ILI9341_COLOR_YELLOW, FonScreen, FONT_2X);
	
	
	SendString ("Таймер: ",   40 , 85, ILI9341_COLOR_WHITE , FonScreen, FONT_2X);
	
	SendString("<=", 40, 160, ILI9341_COLOR_YELLOW, FonScreen, FONT_2X);
	SendString("-10      +10", 73, 160, ILI9341_COLOR_WHITE, FonScreen, FONT_2X);
	SendString("=>", 236, 160, ILI9341_COLOR_YELLOW, FonScreen, FONT_2X);
}	



// процедура последовательного включения 36 контактов, с віключением предыдущего 
void ForPins (void) {
	int nn, ii=0, i=0;
	
	
	int32_t TimerFull_IN, 
					Timer_IN	=	Timer;
	
	char strbuf[30];
	float FREQ=3575,
				IN_FREQ,IN_FREQ_2;
	
	TimerFull=Timer*Needles;
	TimerFull_IN=TimerFull;
		
		IN_FREQ		= FREQ;
		IN_FREQ_2	= FREQ;
		
		TM_DAC_SetValue(DDS_DAC, 0);		// DAC-PA4				
		
		SetFreq1(FREQ/100); // PWM1
		SetFreq2(FREQ/100); // PWM2	
		SetVoltUP();
	
	TM_ILI9341_Fill(FonScreen);
	
		//Таймер всего:
	SendString("ВСЕЕГО:", 40, 40, ILI9341_COLOR_WHITE, FonScreen, FONT_2X);
	
	//Таймер текущий:
	SendString("ТАЙМЕР::", 200, 40, ILI9341_COLOR_WHITE, FonScreen, FONT_2X);
	
	for (i=1; i<(Needles+1); i++){
		LED1_PULS();
		LED2_PULS();
		
		Set_PinsNUMB_OUT(i, 1);
		
		if (i>1) Set_PinsNUMB_OUT(i-1, 0);
		sprintf(strbuf, "Номер контакта:   %.2d", i);		
		SendString(strbuf, 40, 140, ILI9341_COLOR_WHITE, FonScreen, FONT_2X);
		
		for (Timer=Timer_IN; Timer>0; Timer--) {
			DisplayTIMER();
			//Delayms(1000);
			for (ii=0; ii<4; ii++) {
				Delayms(250);
				Volt();	
			}	
			if(TimerFull>0) TimerFull--;								// Timer * Needles
			if(Timer <= 0) Timer--;			
		}						
	}
	
	for (nn=0; nn<10; nn++) {
		LED_ALL_PULS ();
	}
	FreqOFF();
	TM_DAC_SetValue(DDS_DAC, 0);		// DAC-PA4
	TM_ILI9341_Fill(FonScreen); 
	Page_02();
	
	 TimerFull = TimerFull_IN;
	 Timer		 = Timer_IN;
	
	return;
} 

void ForPins1 (void) {
	int nn,ii=0,i=0, pausa=3;
	
	
	int32_t TimerFull_IN,Timer_IN	=	Timer;
	
	char strbuf[30];
	float FREQ=3575;
	float IN_FREQ,IN_FREQ_2;
	
	TimerFull=(Timer*Needles)+(Needles*pausa);
	TimerFull_IN=TimerFull;
		
		IN_FREQ=FREQ;
		IN_FREQ_2=FREQ;
		
		TM_DAC_SetValue(DDS_DAC, 0);		// DAC-PA4				
		
		SetFreq1(FREQ/100); // PWM1
		SetFreq2(FREQ/100); // PWM2	
		SetVoltUP();
	
	TM_ILI9341_Fill(FonScreen);
	
		//Таймер всего:
	SendString("ВСЕЕГО:", 40, 40, ILI9341_COLOR_WHITE, FonScreen, FONT_2X);
	
	//Таймер текущий:
	SendString("ТАЙМЕР::", 200, 40, ILI9341_COLOR_WHITE, FonScreen, FONT_2X);
	
	for (i=1; i<(Needles+1); i++){
		LED1_PULS();
		LED2_PULS();
		
		Set_PinsNUMB_OUT(1, 1);
		
		
		sprintf(strbuf, "Номер контакта:  \"1\"");		
		SendString(strbuf, 40, 140, ILI9341_COLOR_WHITE, FonScreen, FONT_2X);
		
		for (Timer=Timer_IN; Timer>0; Timer--) {
			DisplayTIMER();
			//Delayms(1000);
			for (ii=0; ii<4; ii++) {
				Delayms(250);
				Volt();	
			}	
			if(TimerFull>0) TimerFull--;								// Timer * Needles
			if(Timer <= 0) Timer--;
			
		}
			
		Set_PinsNUMB_OUT(1, 0);
		
		sprintf(strbuf, "      контакт: \"1\"");
		sprintf(strbuf, "  Пауза 3 сек.      ");		
		SendString(strbuf, 50, 160, ILI9341_COLOR_WHITE, FonScreen, FONT_2X);
		
		Timer=pausa;
		for (nn=0; nn<pausa; nn++) {
			DisplayTIMER();	
			Delayms(1000);							// Пауза 3 Секунды
			
			if(TimerFull>0) TimerFull--;
			Timer--;
		}	
					
	}
	
	for (nn=0; nn<10; nn++) {
		LED_ALL_PULS ();
	}
	FreqOFF();
	TM_DAC_SetValue(DDS_DAC, 0);		// DAC-PA4
	TM_ILI9341_Fill(FonScreen);
	Page_02();
	
	 TimerFull = TimerFull_IN;
	 Timer		 = Timer_IN;
	
	Menu=0;
	return;
} 

int main(void) {
//	char chrs[30];
uint32_t n;

	/* Initialize system */
	SystemInit();	
	
	/* Initialize delay */
	TM_DELAY_Init();
	/* Initialize LCD */
	DDS_Init_ADC1();	
	//DDS_Init_ADC2();
	
	/* Initialize DAC channel 1, pin PA4 */
	TM_DAC_Init(DDS_DAC);
	
	FreqOFF();
	TM_DAC_SetValue(DDS_DAC, 0);		// DAC-PA4
	
	
	TM_ILI9341_Init();
	/* Select orientation */
	TM_ILI9341_Rotate(TM_ILI9341_Orientation_Landscape_2);
	/* Fill LCD with blue color */
	TM_ILI9341_Fill(FonScreen);
	
	/* Init 3 key for STM32_F4VE boards */
	// Init_3_BUTTONS ();	
	GPIO_ALL_OUT_Init ();
	
	LED1_OFF();
	LED2_OFF();
	
	// Стартовая станица 
	// запуска пресета =======   CHACRAS ==============
	Page_00();
	
//	LED_PINs_ini(); LED_OFF1(); LED_OFF2();
	
	n=0;

//	SendString("Вых.Сиигн:", 40, 197, ILI9341_COLOR_WHITE, FonScreen, FONT_2X);
	
	//sprintf(chrs, "N %02d", SystemCoreClock);
	//TM_ILI9341_Puts(40, 195, chrs, 
	//&TM_Font_11x18, ILI9341_COLOR_GREEN, FonScreen);
	
	BUTTON_Init();
	
	while (1) {
		Volt();
		//LED1_PULS();
		//LED2_PULS();
		Delayms(200);
		
		/* Update buttons */
		
		if (ReadBUTT1==0){		
				switch (Namb) // Namb--
				{
					case 0: { Namb = 2; LED1_OFF(); LED2_ON(); Page_02(); } break; //
     
					case 1: { Namb = 0; LED1_OFF(); LED2_ON(); Page_00(); } break; //
     
					case 2: { Namb = 1; LED2_ON();  LED1_ON(); Page_01(); } break; // Стартовая станица
				}
		}	
		
		if (ReadBUTT3==0){
				switch (Namb) // Namb--
				{
					case 0: { Namb = 1; LED1_OFF(); LED2_ON();  Page_01(); } break; // Перебор N Контактов
     
					case 1: { Namb = 2; LED1_ON();  LED2_ON();  Page_02(); } break; // Установка Таймера
     
					case 2: { Namb = 0; LED2_ON();  LED1_OFF(); Page_00(); } break; // Стартовая Станица					
				}
		}
		
		if (ReadBUTT2==1){
				switch (Namb) // начало оператора switch
				{
					case 0: {ForPins(); } break; 	// Page_00();  Стартовая станица
     
					case 1: {SetTimer();} break;  // Установка таймера Page_01(); 
     
					case 2: {ForPins1();} break; 	// Page_02(); 
				}
		}	
	}
}

void SetTimer(void) {
	char strbuf[30];
	PageSetTimers ();
	while (1) {
		
		Delayms(200);
		
		if (ReadBUTT1==0){
				if(Timer<110) Timer=Timer+10; else  Timer=10;
		}	
		
		if (ReadBUTT3==0){
				if(Timer>10) Timer=Timer-10; else  Timer=110;			
		}
		
		if (ReadBUTT2==1){
				Page_01();		
				return;
		}
		
		sprintf(strbuf, "%.2d   ", Timer);		
		SendString(strbuf, 147, 85, ILI9341_COLOR_YELLOW, FonScreen, FONT_2X);
	}	
}	

