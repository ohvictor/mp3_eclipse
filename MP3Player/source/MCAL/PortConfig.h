/***************************************************************************//**
  @file     PortConfig.h
  @brief    Port Configuration File
  @author   uLab 2021 G3
 ******************************************************************************/

#ifndef __PTCONFIG_
#define __PTCONFIG_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "gpio.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define PIN_SEG_A	PORTNUM2PIN(PC,3)	//PTC3
#define PIN_SEG_B	PORTNUM2PIN(PC,2)	//PTC2
#define PIN_SEG_C	PORTNUM2PIN(PA,2)	//PTA2
#define PIN_SEG_D	PORTNUM2PIN(PB,23)	//PTB23
#define PIN_SEG_E	PORTNUM2PIN(PA,1)	//PTA1
#define PIN_SEG_F	PORTNUM2PIN(PB,9)	//PTB9
#define PIN_SEG_G	PORTNUM2PIN(PC,17)	//PTC17
#define PIN_SEG_DP	PORTNUM2PIN(PC,16)	//PTC16

#define PIN_SEL_0	PORTNUM2PIN(PC,5)	//PTC5
#define PIN_SEL_1	PORTNUM2PIN(PC,7)	//PTC7

#define PIN_STA_0	PORTNUM2PIN(PB,19)	//PTB19
#define PIN_STA_1	PORTNUM2PIN(PB,18)	//PTB18

#define PIN_RCHA	PORTNUM2PIN(PC,0)	//PTC0
#define PIN_RCHB	PORTNUM2PIN(PC,9)	//PTC9
#define PIN_RSW		PORTNUM2PIN(PC,8)	//PTC8

#define PIN_MAG_DAT	PORTNUM2PIN(PB,2)	//PTB2
#define PIN_MAG_E	PORTNUM2PIN(PB,3)	//PTB3
#define PIN_MAG_CLK	PORTNUM2PIN(PB,10)	//PTB10


#endif // _PTCONFIG_
