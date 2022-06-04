/**
 *  Defines for your entire project at one place
 * 
 *	@author 	Igor Korsunskiy	
 *	@email		igkors@ukr.net
 *	@version 	v1.0
 *	@ide			Keil uVision 5
 *	@license	GNU GPL v3
 *	
 * |----------------------------------------------------------------------
 * | Copyright (C) Igor Korsunskiy	, 2022
 */
#ifndef TM_DEFINES_H
#define TM_DEFINES_H


/* Increase clock for SPI5 for LCD on STM32F407VET6 */
/* Set clock to APB2/2 = 180/2/2 = 45MHz */
#define TM_SPI3_PRESCALER	SPI_BaudRatePrescaler_2

#endif
