/*
=======================================================================================================
The program of the four-channel meander generator with frequency tuning simultaneously on all channels.
Separately for each channel, the duty cycle and phase shift are configured.
A tuning step is set separately for each parameter. (for frequency, for duty, for phase shift)

=======================================================================================================
If it is necessary to use 8 channels with a phase shift of 45 degrees 
(4 direct up to 180 degrees and 4 through inversion after 180 degrees)
=======================================================================================================
 
 ****************************************************************************************
 *	@author			Igor Korsunskiy																													*
 *	@email			igkors@ukr.net																													*
 *	@ide				Keil uVision 5																													*
 *	@packs			STM32F4xx Keil packs version Keil.STM32F4xx_DFP.2.11.0									*
 *	@stdperiph	STM32F4xx Standard peripheral drivers version 1.4.0 or greater required *
 *  @lib				TM - author: Tilen Majerle                                              *
 ****************************************************************************************
 ****************************************************************************************
 *	DDS 4хCHANAL на STM32F407VET6	V-02.01	2022.06.03																		*
 *	TFT ILI9341 display without touchscreen																							*
 ****************************************************************************************
 *********************************************************
		peripheral pins for STM32F407VET6
		_________________________________________________________
		Display TFT ILI9341
		---------------------------------------------------------
		PINS PACK 1 (SPI3)
		
		MOSI   /MISO/  SCK
		PB5    /PB4/   PB3
		
		PB7 	- CS
		PB6 	- WRX PIN for DC (data/command)
		PB8 	- RESET
		+3.3V	- LED, Vcc
		GND		- GND
		_________________________________________________________
		BUTTONS
		---------------------------------------------------------
		PE11 - BUTTON1 	(SWITCH on GND) Left
		PE12 - BUTTON4	(SWITCH on GND) Ok
		PE13 - BUTTON2	(SWITCH on GND) Right
		PE14 - BUTTON4	(SWITCH on GND) UP
		PE15 - BUTTON5	(SWITCH on GND) Down
		__________________________________________________________
		
		In channels No. 2, 3, 4, the phase shift relative to channel No. 1 is adjusted
		----------------------------------------------------------
		TIM1(MASTER CH1 PP1 PA8) Sinchro Timer for four Slave channels
		TIM5(SLAVE0 CH1 PP1 PA0) N 1 Channel - Phase shift 0 degrees
		TIM2(SLAVE1 CH3 PP1 PA2) N 2 Channel - Phase shift 0... 360 degrees
		TIM3(SLAVE2 CH2 PP1 PA7) N 3 Channel - Phase shift 0... 360 degrees
		TIM4(SLAVE3 CH4 PP1 PB9) N 4 Channel - Phase shift 0... 360 degrees

TIMER   |CHANNEL 1            |CHANNEL 2            |CHANNEL 3            |CHANNEL 4
        |PP1    PP2    PP3    |PP1    PP2    PP3    |PP1    PP2    PP3    |PP1    PP2    PP3

TIM 1   |PA8    PE9    -      |PA9    PE10   -      |PA10   PE13   -      |PA11   PE14   -
TIM 2   |PA0    PA5    PA15   |PA1    PB3    -      |PA2    PB10   -      |PA3    PB11   -
TIM 3   |PA6    PB4    PC6    |PA7    PB5    PC7    |PB0    PC8    -      |PB1    PC9    -
TIM 4   |PB6    PD12   -      |PB7    PD13   -      |PB8    PD14   -      |PB9    PD15    -
TIM 5   |PA0    PH10   -      |PA1    PH11   -      |PA2    PH12   -      |PA3    PI0    -
TIM 8   |PC6    PI5    -      |PC7    PI6    -      |PC8    PI7    -      |PC9    PI2    -
TIM 9   |PA2    PE5    -      |PA3    PE6    -      |-      -      -      |-      -      -
TIM 10  |PB8    PF6    -      |-      -      -      |-      -      -      |-      -      -
TIM 11  |PB9    PF7    -      |-      -      -      |-      -      -      |-      -      -
TIM 12  |PB14   PH6    -      |PB15   PH9    -      |-      -      -      |-      -      -
TIM 13  |PA6    PF8    -      |-      -      -      |-      -      -      |-      -      -
TIM 14  |PA7    PF9    -      |-      -      -      |-      -      -      |-      -      -
*******************************************************************************************/
		
/* Include core modules */
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_flash.h"
#include <stdio.h>

/* Include TM libraries here */
#include "defines.h" 
#include "tm_stm32f4_ili9341.h"
#include "tm_stm32f4_delay.h"
#include "tm_stm32f4_pwm.h"
#include "stm32f4xx_flash.h"

#define	FonScreen	ILI9341_COLOR_BLUE

#define ReadBUTT1 GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_11)	// LEFT
#define	ReadBUTT2 GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_12)	// OK
#define	ReadBUTT3 GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_13)	// RIGHT
#define	ReadBUTT4 GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_14)	// UP
#define	ReadBUTT5 GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_15)	// Down

// ++++ menu items   +++++++++++++++++++++++++++++++++++++++++++++++
#define MFREQ					1 // Frequency of 4 channels		/ FREQ
#define MDUTY1				2 // duty cycle CH1-PA0					/ DUTY1
#define MDUTY2				3 // duty cycle CH3-PA2					/ DUTY2
#define MDUTY3				4 // duty cycle CH2-PA7					/ DUTY3
#define MDUTY4				5 // duty cycle CH4-PB9					/ DUTY4

#define MPHAS1				6 // Phase 1 / CH2-PA2  				/ PHASE1
#define MPHAS2				7 // Phase 2 / CH3-PA7					/ PHASE2
#define MPHAS3				8 // Phase 3 / CH3-PB9					/ PHASE3

int16_t STEP  = 10; 			// Adjustment step Freq
int16_t STEPd = 10; 			// Adjustment step DUTE
int16_t STEPf = 10; 			// Adjustment step Phase
int8_t MENU   = MFREQ; 		// Menu item number

// Display Orientation
enum TM_ILI9341_Orientation_Landscape_1;

//  Global variables for Timers
		TIM_OCInitTypeDef TIM_OC;

		TM_PWM_TIM_t TIM1_Data;
		TM_PWM_TIM_t TIM2_Data;
		TM_PWM_TIM_t TIM3_Data;
		TM_PWM_TIM_t TIM4_Data;
		TM_PWM_TIM_t TIM5_Data;

/*
Structure for writing to flash settings. 
While saving settings is not implemented!
*/
typedef struct {
	float 	FREQ;  
	int16_t DUT1; 		
	int16_t	DUT2; 
	int16_t	DUT3; 
	int16_t	DUT4; 
	int16_t PHAS1;
	int16_t PHAS2;//
	int16_t PHAS3;//
	int16_t STEP;
	int16_t STEPd;
	int16_t STEPf;
	int8_t ZeroFlag;
} DataChannals_t;

	
	int32_t DUTY1 	= 0, 		
					DUTY2 	= 0,
					DUTY3 	= 0,
					DUTY4 	= 0;
	
	int16_t DUT1 		= 500, 		// Channel 1 duty cycle 50 %
					DUT2 		= 500,		// Channel 2 duty cycle 50 %
					DUT3 		= 500,		// Channel 3 duty cycle 50 %
					DUT4 		= 500;		// Channel 4 duty cycle 50 %
	
	int32_t POPRAVKA= 68,
					PHASE1	=	0,	
					PHASE2	=	0, 
					PHASE3	=	0;

/*
1) 250kHz C x 4 (20nF) 0, 45, 90 , 135 grd
2) 360kHz C x 4 (10nF) 0, 90, 180, 270 grd
*/

  float 	DUTx		= 0,
					PHASEs	= 0, 
	
					FREQ		= 362000; //362 kHz for capacitors x 4 (20nF)

	int16_t PHAS1		=	900,		// Channel-2 90 degree phase shift
					PHAS2		=	1800,  	// Channel-3 180 degree phase shift
					PHAS3		=	2700;		// Channel-4 270 degree phase shift
					
									
	int8_t 	BUTT1=0, 		BUTT2=0, 		BUTT3=0, BUTT4=0, BUTT5=0;

void StartPage (void);
void FreqSendDisplay (int x, int y, float freq);
void SetCH1(void);
void SetCH2(void);
void SetCH3(void);
void SetCH_ALL(void);
void InitBUTT (void);	
void WriteFlash(void);

void WriteFlash(void)
{	
DataChannals_t ALL_CHANAL;
 FLASH_Unlock();
	
	ALL_CHANAL.FREQ		= FREQ;
	ALL_CHANAL.DUT1		= DUT1;
	ALL_CHANAL.DUT2		= DUT2;
	ALL_CHANAL.DUT3		= DUT3;
	ALL_CHANAL.DUT4		= DUT4;
	ALL_CHANAL.PHAS1	= PHAS1;
	ALL_CHANAL.PHAS2	= PHAS2;
	ALL_CHANAL.PHAS3	= PHAS3;
	ALL_CHANAL.STEP		= STEP;
	ALL_CHANAL.STEPd	=	STEPd;
	ALL_CHANAL.STEPf	= STEPf;
 FLASH_Lock();
}

// TIM1(MASTER CH1 PP1 PA8)
// TIM5(SLAVE0 CH1 PP1 PA0)	
// TIM2(SLAVE1 CH3 PP1 PA2)
// TIM3(SLAVE2 CH2 PP1 PA7)
// TIM4(SLAVE3 CH4 PP1 PB9)

void TIMERS_Init(void) {
	
		// TIM1(MASTER CH1 PP1 PA8)
		TM_PWM_InitTimer	(TIM1, &TIM1_Data, FREQ);

		// TIM5(SLAVE0 CH0 PP1 PA0)
		TM_PWM_InitTimer	(TIM5, &TIM5_Data, FREQ);
		TM_GPIO_InitAlternate(GPIOA, GPIO_PIN_0,  TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High, GPIO_AF_TIM5);
	
		if (DUTx ==0){	
			DUTx		= (TIM1_Data.Period/1000);
			DUTY1		= DUTx * DUT1;
			DUTY2		= DUTx * DUT2;
			DUTY3		= DUTx * DUT3;
			DUTY4		= DUTx * DUT4;
		}
		TM_PWM_SetChannel (&TIM5_Data, TM_PWM_Channel_1, DUTY1-1);	
	
		// TIM2(SLAVE1 CH3 PP1 PA2)
		TM_PWM_InitTimer	(TIM2, &TIM2_Data, FREQ);
		TM_GPIO_InitAlternate(GPIOA, GPIO_PIN_2, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High, GPIO_AF_TIM2);
		TM_PWM_SetChannel (&TIM2_Data, TM_PWM_Channel_3, DUTY2-1);
		
		
		// TIM3(SLAVE2 CH2 PP1 PA7)
		TM_PWM_InitTimer	(TIM3, &TIM3_Data, FREQ);
		TM_GPIO_InitAlternate(GPIOA, GPIO_PIN_7,  TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High, GPIO_AF_TIM3);
		
		if (PHASEs != (TIM3_Data.Period/360)){
			PHASEs 	= (float)(TIM5_Data.Period/360);
			PHASE1	=	PHASEs*PHAS1;		// 90 
			PHASE2	=	PHASEs*PHAS2; 	// 180 
			PHASE3	=	PHASEs*PHAS3; 	// 270
			
		}
		
		TM_PWM_SetChannel (&TIM3_Data, TM_PWM_Channel_2, DUTY3-1);
		
		
		// TIM4(SLAVE3 CH4 PP1 PB9)
		TM_PWM_InitTimer	(TIM4, &TIM4_Data, FREQ);
		TM_GPIO_InitAlternate(GPIOB, GPIO_PIN_9,  TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High, GPIO_AF_TIM4);
		
		TIM_OCStructInit(&TIM_OC);
		TIM_OC.TIM_OCMode 				= TIM_OCMode_PWM1; 				// TIM_OCMode_PWM1 / TIM_OCMode_Toggle
    TIM_OC.TIM_OutputState 		= TIM_OutputState_Enable;
    TIM_OC.TIM_OCPolarity 		= TIM_OCPolarity_High; 		// TIM_OCPolarity_Low / TIM_OCPolarity_High
		TIM_OC.TIM_OutputState 		= TIM_OutputState_Enable;
    TIM_OC.TIM_OCPolarity 		= TIM_OCPolarity_High;
		
		TIM_OC.TIM_Pulse 					= TIM1_Data.Period/2;
		TIM_OC1Init(TIM1, &TIM_OC);
    
		
		TIM_OC.TIM_Pulse 					= DUTY2;
		TIM_OC2Init(TIM2, &TIM_OC);
		
		TIM_OC.TIM_Pulse 					= DUTY3;
		TIM_OC3Init(TIM3, &TIM_OC);
		
		TIM_OC.TIM_Pulse 					= DUTY4;
    TIM_OC4Init(TIM4, &TIM_OC);
		
		// Registr OUT TIM_TRGOSource 
		//Configuring the master 
		TIM_SelectOutputTrigger  	(TIM1, TIM_TRGOSource_Enable);	
		TIM_SelectMasterSlaveMode	(TIM1, TIM_MasterSlaveMode_Enable);
		
		//Configuring the Slsve
		TIM_SelectInputTrigger	 	(TIM3, TIM_TS_ITR0);										//CH2 - TIM_TS_ITR0 for master(TIM1)		
    TIM_SelectSlaveMode				(TIM3, TIM_SlaveMode_Gated);
		TIM_SelectInputTrigger	 	(TIM2, TIM_TS_ITR0);										//CH3 - TIM_TS_ITR0 for master(TIM1)		
    TIM_SelectSlaveMode				(TIM2, TIM_SlaveMode_Gated);
		TIM_SelectInputTrigger	 	(TIM4, TIM_TS_ITR0);										//CH4 - TIM_TS_ITR0 for master(TIM1)		
    TIM_SelectSlaveMode				(TIM4, TIM_SlaveMode_Gated);
		
		TM_PWM_SetChannel 				(&TIM5_Data, TM_PWM_Channel_1, DUTY1);
			
		TIM_SetCounter(TIM2, PHASE1); // PA2 CH4 DELAY 90% / 1679-i / TIM4_Data.Period/4
		TIM_SetCounter(TIM3, PHASE2);	// PA7 CH2 DELAY 180%
		TIM_SetCounter(TIM4, PHASE3); // PB9 CH3 DELAY 270%
		TIM_SetCounter(TIM5, 0);			// PA0 CH3 DELAY  0%
			
		TIM_Cmd(TIM5, ENABLE); // CH5	
		TIM_Cmd(TIM2, ENABLE); // CH3
		TIM_Cmd(TIM3, ENABLE); // CH2
		TIM_Cmd(TIM4, ENABLE); // CH4
			
		TIM_Cmd(TIM1, ENABLE); // CH1
		
}

void SetCH_ALL(void) {
	
		TM_PWM_InitTimer	(TIM1, &TIM1_Data, FREQ);
		TM_PWM_InitTimer	(TIM5, &TIM5_Data, FREQ);
		TM_PWM_InitTimer	(TIM2, &TIM2_Data, FREQ);
		TM_PWM_InitTimer	(TIM3, &TIM3_Data, FREQ);
		TM_PWM_InitTimer	(TIM4, &TIM4_Data, FREQ);
	
		PHASEs 	= (float)(TIM1_Data.Period/3600);
		PHASE1	= PHASEs*PHAS1; // 
		PHASE2	=	PHASEs*PHAS2; //
		PHASE3	=	PHASEs*PHAS3;
	
		DUTx		= (TIM1_Data.Period/1000);
		DUTY1		= DUTx * DUT1*10;
		DUTY2		= DUTx * DUT2;
		DUTY3		= DUTx * DUT3;
		DUTY4		= DUTx * DUT4;
	
		TM_PWM_SetChannel (&TIM5_Data, TM_PWM_Channel_1, DUTY1);
		TM_PWM_SetChannel (&TIM2_Data, TM_PWM_Channel_3, DUTY2);
		TM_PWM_SetChannel (&TIM3_Data, TM_PWM_Channel_2, DUTY3);
		TM_PWM_SetChannel (&TIM4_Data, TM_PWM_Channel_4, DUTY4);
		
		// TIM_SetCounter - TIM_SetCounter - Write the number of ticks to the delay register for the specified timer. 
		// In the line of stm32f4xx controllers, 
		// there are three timers on board with such a register. 
		//This allows the three PWM outputs to be re-arranged for phase offset relative to the input flip-flop.
		TIM_SetCounter(TIM2, PHASE1); // PA2 CH4 DELAY 25% / 1679-i / TIM4_Data.Period/4
		TIM_SetCounter(TIM3, PHASE2);	// PA7 CH2 DELAY 50%
		TIM_SetCounter(TIM4, PHASE3); // PB9 CH3 DELAY 75%
		TIM_SetCounter(TIM5, 14);			// PA0 CH3 DELAY  0%
		
		
		TIM_Cmd(TIM5, ENABLE); // CH5
		TIM_Cmd(TIM2, ENABLE); // CH3
		TIM_Cmd(TIM3, ENABLE); // CH2
		TIM_Cmd(TIM4, ENABLE); // CH4
		
			
		TIM_Cmd(TIM1, ENABLE); // CH1
}

void StartPage (void){
	int32_t color;
	char strbuf[30];
	// Стартовая страница
	TM_ILI9341_Fill(FonScreen);
	
	if (MENU==MFREQ)
	sprintf(strbuf, "STEP:  %d", STEP);
	
	if ((MENU==MDUTY1) || (MENU==MDUTY2) || (MENU==MDUTY3) || (MENU==MDUTY4))
	sprintf(strbuf, "STEP:  %d", STEPd);
	
	if ((MENU==MPHAS1) || (MENU==MPHAS2) || (MENU==MPHAS3))
	sprintf(strbuf, "STEP:  %d", STEPf);
	
	TM_ILI9341_Puts(40, 10, strbuf, 
	&TM_Font_11x18, ILI9341_COLOR_WHITE, FonScreen);
	
	
	//FreqSendDisplay (40, 50, FREQ);
	
	sprintf(strbuf, "FREQ:  %.3f KHz", 
		(float)FREQ/1000);
		if (MENU!=1) color=ILI9341_COLOR_WHITE; else color=ILI9341_COLOR_YELLOW;
		TM_ILI9341_Puts(40, 30, strbuf, 
		&TM_Font_11x18, color, FonScreen);
		
	sprintf(strbuf, "Duty CH1:  %.1f %%", 
		(float)DUT1/10);
	if (MENU!=2) color=ILI9341_COLOR_WHITE; else color=ILI9341_COLOR_YELLOW;	
	TM_ILI9341_Puts(40, 55, strbuf, 
	&TM_Font_11x18, color, FonScreen);
	
	sprintf(strbuf, "Duty CH2:  %.1f %%", 
		(float)DUT2/10);
	if (MENU!=3) color=ILI9341_COLOR_WHITE; else color=ILI9341_COLOR_YELLOW;	
	TM_ILI9341_Puts(40, 75, strbuf, 
	&TM_Font_11x18, color, FonScreen);
	
	sprintf(strbuf, "Duty CH3:  %.1f %%", 
		(float)DUT3/10);
	if (MENU!=4) color=ILI9341_COLOR_WHITE; else color=ILI9341_COLOR_YELLOW;	
	TM_ILI9341_Puts(40, 95, strbuf, 
	&TM_Font_11x18, color, FonScreen);
	
	sprintf(strbuf, "Duty CH4:  %.1f %%", 
		(float)DUT4/10);
	if (MENU!=5) color=ILI9341_COLOR_WHITE; else color=ILI9341_COLOR_YELLOW;	
	TM_ILI9341_Puts(40, 115, strbuf, 
	&TM_Font_11x18, color, FonScreen);
	
	sprintf(strbuf, "Phase 2:  %.1f", 
		(float)PHAS1/10);
	if (MENU!=6) color=ILI9341_COLOR_WHITE; else color=ILI9341_COLOR_YELLOW;
	TM_ILI9341_Puts(40, 140, strbuf, 
	&TM_Font_11x18, color, FonScreen);
	
	sprintf(strbuf, "Phase 3:  %.1f", 
		(float)PHAS2/10);
	if (MENU!=7) color=ILI9341_COLOR_WHITE; else color=ILI9341_COLOR_YELLOW;
	TM_ILI9341_Puts(40, 160, strbuf, 
	&TM_Font_11x18, color, FonScreen);
	
	sprintf(strbuf, "Phase 4:  %.1f", 
		(float)PHAS3/10);
	if (MENU!=8) color=ILI9341_COLOR_WHITE; else color=ILI9341_COLOR_YELLOW;
	TM_ILI9341_Puts(40, 180, strbuf, 
	&TM_Font_11x18, color, FonScreen);
	
	
	TM_ILI9341_Puts(40, 207, "1-PA0 2-PA2 3-PA7 4-PB9", 
	&TM_Font_11x18, ILI9341_COLOR_WHITE, FonScreen); // ILI9341_COLOR_YELLOW
}

// Displaying the frequency of two channels, for test mode
void FreqSendDisplay (int x, int y, float freq) {	
char strbuf[30]; 
	
	sprintf(strbuf, "FREQ:  %.2f Hz", 
		(float)freq);
		
		TM_ILI9341_Puts(x, y, strbuf, 
		&TM_Font_11x18, ILI9341_COLOR_WHITE, FonScreen);
}

void InitBUTT (void){
	GPIO_InitTypeDef BUTT_Struct;             	// Structure Reference
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	
  BUTT_Struct.GPIO_Pin  	= GPIO_Pin_11 | GPIO_Pin_14 | GPIO_Pin_13 | GPIO_Pin_15 | GPIO_Pin_14;
	BUTT_Struct.GPIO_Mode 	= GPIO_Mode_IN; 	  // we want it to be an input
	BUTT_Struct.GPIO_Speed 	= GPIO_Speed_2MHz;	//this sets the GPIO modules clock speed
	BUTT_Struct.GPIO_OType 	= GPIO_OType_PP;   	// this sets the pin type to push / pull (as opposed to open drain)
	BUTT_Struct.GPIO_PuPd 	= GPIO_PuPd_UP; 		// this enables the pulldown resistor --> we want to detect a high level
	GPIO_Init(GPIOE, &BUTT_Struct);
}

int main(void) {
	/* Initialize system */
	SystemInit();	
	/* Initialize delay */
	TM_DELAY_Init();
		
	/* Initialize LCD */	
	TM_ILI9341_Init();
	/* Select orientation */
	TM_ILI9341_Rotate(TM_ILI9341_Orientation_Landscape_2);
	/* Fill LCD with blue color */
	TM_ILI9341_Fill(FonScreen);
	
	TIMERS_Init();
	SetCH_ALL();
	
	// MENU - Start Page
	StartPage();
	
	InitBUTT();
	
	while (1) {	
		// Reading the status of buttons and working with the menu
		if (ReadBUTT2==0){
			Delayms(250);
			if (MENU < 9) MENU++; if (MENU == 9) MENU=1;
			StartPage();
		}
		
		if (ReadBUTT4==0){
			Delayms(250);
			if (MENU > 0) MENU--; if (MENU == 0) MENU=8;
			StartPage();
		}
		
		if (ReadBUTT5==0){
			Delayms(250);
			if (MENU==MFREQ)
			switch (STEP) {
				case 1: 		STEP=10; 		break;
				case 10: 		STEP=100;		break;
				case 100: 	STEP=1;			break;
				default:		STEP=1;			break;
			}
			
			if ((MENU==MDUTY1) || (MENU==MDUTY2) || (MENU==MDUTY3) || (MENU==MDUTY4))
			switch (STEPd) {
				case 1: 		STEPd=10; 		break;
				case 10: 		STEPd=100;		break;
				case 100: 	STEPd=1000;		break;
				default:		STEPd=1;			break;
			}
			
			if ((MENU==MPHAS1) || (MENU==MPHAS2) || (MENU==MPHAS3))
			switch (STEPf) {
				case 1: 		STEPf=10; 		break;
				case 10: 		STEPf=100;		break;
				case 100: 	STEPf=1;			break;
				default:		STEPf=1;			break;
			}
			
			
			StartPage();
		}
		
		if (ReadBUTT1==0){
				Delayms(250);
				switch (MENU)	{
					case MFREQ: {
						if(FREQ>STEP) FREQ=FREQ-STEP;
						StartPage();
						SetCH_ALL();
					}break;
					
					case MDUTY1: {
						if(DUT1-STEPd > 0) DUT1=DUT1-STEPd;
						else DUT1 = 1;
						StartPage();
						SetCH_ALL();
					}break;
					
					case MDUTY2: {
						if(DUT2-STEPd > 0) DUT2=DUT2-STEPd;
						else DUT2 = 1;
						StartPage();
						SetCH_ALL();
					}break;
					
					case MDUTY3: {
						if(DUT3-STEPd > 0) DUT3=DUT3-STEPd;
						else DUT3 = 1;
						StartPage();
						SetCH_ALL();
					}break;
					
					case MDUTY4: {
						if(DUT4-STEPd > 0) DUT4=DUT4-STEPd;
						else DUT1 = 1;
						StartPage();
						SetCH_ALL();
					}break;
					
					case MPHAS1: {
						if ((PHAS1-STEPf)>0) PHAS1=PHAS1-STEPf;
						else PHAS1=1;
						StartPage();
						SetCH_ALL();
					} break;
					
					case MPHAS2: {
						if ((PHAS2-STEPf)>0) PHAS2=PHAS2-STEPf;
						else PHAS2=1;
						StartPage();
						SetCH_ALL();
					} break;
					
					case MPHAS3: {
						if ((PHAS3-STEPf)>0) PHAS3=PHAS3-STEPf;
						else PHAS3=1;
						StartPage();
						SetCH_ALL();
					} break;
				}
		}
		
		if (ReadBUTT3==0){
				Delayms(250);
				switch (MENU)	{	
					case MFREQ: {
						FREQ=FREQ+STEP;
						StartPage();
						SetCH_ALL();
					}break;
					
					case MDUTY1: {
						if(DUT1+STEPd < 1000) DUT1=DUT1+STEPd;
						else DUT1 = 999;
						StartPage();
						SetCH_ALL();
					}break;
					
					case MDUTY2: {
						if(DUT2+STEPd < 1000) DUT2=DUT2+STEPd;
						else DUT2 = 999;
						StartPage();
						SetCH_ALL();
					}break;
					
					case MDUTY3: {
						if(DUT3+STEPd < 1000) DUT3=DUT3+STEPd;
						else DUT3 = 999;
						StartPage();
						SetCH_ALL();
					}break;
					
					case MDUTY4: {
						if(DUT4+STEPd < 1000) DUT4=DUT4+STEPd;
						else DUT4 = 999;
						StartPage();
						SetCH_ALL();
					}break;
					
					case MPHAS1: { 
						if ((PHAS1+STEPf)<3600) PHAS1=PHAS1+STEPf;
						else PHAS1=3599;
						StartPage();
						SetCH_ALL();
					} break;
					
					case MPHAS2: { 
						if ((PHAS2+STEPf)<3600) PHAS2=PHAS2+STEPf;
						else PHAS2=3599;
						StartPage();
						SetCH_ALL();
					} break;

					case MPHAS3: {
						if ((PHAS3+STEPf)<3600) PHAS3=PHAS3+STEPf;
						else PHAS3=3599;
						StartPage();
						SetCH_ALL();
					} break;
				}	
		}
	}
}
