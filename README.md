Keil / STM32F407_CHx4
Four-channel meander generator.
1. Setting the phase shift between channels with hard synchronization.
2. Setting the duty cycle separately for each channel.
3. Frequency tuning to hot simultaneously on all channels. When changing the frequency, the duty cycle and phase shift parameters are saved.
4. A tuning step is set separately for each parameter. (for frequency, for duty, for phase shift)

TIM1 is initialized by the master.
Slave TIM5 PA0 - N 1 Channel, TIM2 PA2 - N 2 Channel,
      TIM3 PA7 - N 3 Channel, TIM4 PB9 - N 4 Channel 

Display TFT ILI9341
---------------------------------------------------------
PB5-MOSI / PB4-MISO / PB3-SCK
  		
PB7 	- CS
PB6 	- WRX PIN for DC (data/command)
PB8 	- RESET
+3.3V	- LED, Vcc
GND	- GND
_________________________________________________________
BUTTONS (SWITCH on GND)
---------------------------------------------------------
PE11 BUTTON-1 Left | PE12 BUTTON-2 Ok | PE13 BUTTON-3 Right | 
PE14 BUTTON-4 UP   | PE15 BUTTON-5 Down
