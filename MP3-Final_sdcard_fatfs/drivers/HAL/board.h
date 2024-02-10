/***************************************************************************//**
  @file     board.h
  @brief    Board management
  @author   Nicolás Magliola
 ******************************************************************************/

#ifndef _BOARD_H_
#define _BOARD_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "gpio.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/***** BOARD defines **********************************************************/

// On Board User LEDs
#define PIN_LED_RED     		PORTNUM2PIN(PB,22)// PTB22
#define PIN_LED_GREEN   		PORTNUM2PIN(PE,26)// PTE26
#define PIN_LED_BLUE    		PORTNUM2PIN(PB,21)// PTB21
#define PIN_LED_YELLOW_EXT    	PORTNUM2PIN(PB,23)// PTB23

#define LED_ACTIVE      LOW


// On Board User Switches
#define PIN_SW2         	PORTNUM2PIN(PC,6)// PTC6
#define PIN_SW3         	PORTNUM2PIN(PA,4)// PTA4
#define PIN_SW_EXT         	PORTNUM2PIN(PC,9)// PTC9

#define SW_ACTIVE		LOW       // ???
#define SW_INPUT_TYPE	INPUT_PULLUP   // ???

//Encoder
#define ENCODER_PIN_A	PORTNUM2PIN(PB,22)//
#define ENCODER_PIN_B	PORTNUM2PIN(PB,22)//
#define ENCODER_BUTTON	PORTNUM2PIN(PB,22)//


/* lcd mapping */

#define LCD_D7_PIN  PORTNUM2PIN(PC,3) //PTC3
#define LCD_D6_PIN  PORTNUM2PIN(PC,2) //PTC2
#define LCD_D5_PIN  PORTNUM2PIN(PA,2) //PTA2
#define LCD_D4_PIN  PORTNUM2PIN(PB,23) //PTB23
#define LCD_ENABLE_PIN  PORTNUM2PIN(PA,1) //PTA1
#define LCD_RS_PIN      PORTNUM2PIN(PB,9) //PTB9

#define LCD_ENABLE_EN (gpioMode(LCD_ENABLE_PIN,OUTPUT)) /*!< Enable target LCD Enable */
#define LCD_RS_EN (gpioMode(LCD_RS_PIN,OUTPUT)) /*!< Enable target LCD RS */
#define LCD_D7_EN (gpioMode(LCD_D7_PIN,OUTPUT)) /*!< Enable target LCD D7*/
#define LCD_D6_EN (gpioMode(LCD_D6_PIN,OUTPUT)) /*!< Enable target LCD D6*/
#define LCD_D5_EN (gpioMode(LCD_D5_PIN,OUTPUT)) /*!< Enable target LCD D5*/
#define LCD_D4_EN (gpioMode(LCD_D4_PIN,OUTPUT)) /*!< Enable target LCD D4*/
#define LCD_ENABLE_OFF (gpioWrite(LCD_ENABLE_PIN, false)) /*!< Turn off target LCD Enable */
#define LCD_RS_OFF (gpioWrite(LCD_RS_PIN, false)) /*!< Turn off target LCD RS */
#define LCD_D7_OFF (gpioWrite(LCD_D7_PIN, false)) /*!< Turn off target LCD D7*/
#define LCD_D6_OFF (gpioWrite(LCD_D6_PIN, false)) /*!< Turn off target LCD D6*/
#define LCD_D5_OFF (gpioWrite(LCD_D5_PIN, false)) /*!< Turn off target LCD D5*/
#define LCD_D4_OFF (gpioWrite(LCD_D4_PIN, false)) /*!< Turn off target LCD D4*/
#define LCD_ENABLE_ON (gpioWrite(LCD_ENABLE_PIN, true)) /*!< Turn on target LCD Enable */
#define LCD_RS_ON (gpioWrite(LCD_RS_PIN, true)) /*!< Turn on target LCD RS */
#define LCD_D7_ON (gpioWrite(LCD_D7_PIN, true)) /*!< Turn on target LCD D7 */
#define LCD_D6_ON (gpioWrite(LCD_D6_PIN, true)) /*!< Turn on target LCD D6 */
#define LCD_D5_ON (gpioWrite(LCD_D5_PIN, true)) /*!< Turn on target LCD D5 */
#define LCD_D4_ON (gpioWrite(LCD_D4_PIN, true)) /*!< Turn on target LCD D4 */

// Test point
#define TP				PORTNUM2PIN(PC,5) // PTC4
#define TP2				PORTNUM2PIN(PC,7)
/*******************************************************************************
 ******************************************************************************/


#endif // _BOARD_H_
