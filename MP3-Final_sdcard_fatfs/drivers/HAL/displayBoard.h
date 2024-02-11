/***************************************************************************//**
  @file     displayBoard.h
  @brief    Display Board Services
  @author   
 ******************************************************************************/

#ifndef __DISPBOARD_
#define __DISPBOARD_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "gpio.h"
#include "PortConfig.h"
//#include "characters.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define NUM_DIGITS	4



/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef enum {
	status0 = 1,
	status1 = 2,
	status2 = 3
} LEDStat_type;

typedef enum {
	symClr = 0x00,
	sym0 = 0x3F,
	sym1 = 0x06,
	sym2 = 0x5B,
	sym3 = 0x4F,
	sym4 = 0x66,
	sym5 = 0x6D,
	sym6 = 0x7D,
	sym7 = 0x07,
	sym8 = 0x7F,
	sym9 = 0x6F,
	symDash = 0x40,
	symb = 0x7C,
	symr = 0x50,
	symi = 0x04,
	symd = 0x5E,
	symn = 0x54
} sym_type;

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/



/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Configures Output pins to mux and data to 7sD and StatusLEDs. Input pins
 * @param pin the pin whose mode you wish to set (according PORTNUM2PIN)
 * @param mode INPUT, OUTPUT, INPUT_PULLUP or INPUT_PULLDOWN.
 */
void DispBoard_Init();

/*
 * @brief Writes a character on a segment
 * @param character: character to be written from selection (SSChar_type)
 * @param digit: digit position where to write the character
 * @param dp: whether or not to turn on the Decimal Point
 */
void digit_write(sym_type symbol, int digit, bool dp);

/*
 * @brief Refreshes the contents of the display
 */
void display_refresh();

/*
 * @brief Sets the status of the LEDs
 * @param status: status value.
 */
void status_led_write(LEDStat_type status);

/*
 * @brief Refreshes the status LEDs
 */
void status_led_refresh();

/*
 * @brief Sets display brightness level
 */
void brightness_set(uint8_t level);

/*******************************************************************************
 ******************************************************************************/

#endif //__DISPBOARD_
