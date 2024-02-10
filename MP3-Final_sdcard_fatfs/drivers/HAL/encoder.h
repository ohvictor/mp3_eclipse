/***************************************************************************//**
  @file     encoder.h
  @brief
  @author   Facundo Molina
 ******************************************************************************/

#ifndef _ENCODER_H_
#define _ENCODER_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "PortConfig.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define CW					1
#define CCW					-1
#define NOTHING 			0

#define BUTTON_PRESSED		LOW
#define BUTTON_RELEASED		HIGH

#define ENCODER_ISR_PERIOD			5	// ticks
#define BUTTON_ISR_PERIOD			10	// ticks
#define BUTTON_LONG_ISR_COUNT		100	// times


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief
 * @param
 * @param
 */
void rotaryEncoder_init(void);

/**
 * @brief
 * @param
 * @param
 */
void rotary_Init(void);

/**
 * @brief
 * @param
 * @param
 */
void button_init(void);

/**
 * @brief
 * @param
 * @param
 */
int encoder_read(void);


/**
 * @brief
 * @param
 * @param
 */
void set_encoderCW_callback(void (*funcallback)(void));

/**
 * @brief
 * @param
 * @param
 */
void set_encoderCCW_callback(void (*funcallback)(void));

/**
 * @brief
 * @param
 * @param
 */
void set_button_long_callback(void (*funcallback)(void));


/**
 * @brief
 * @param
 * @param
 */
void set_button_short_callback(void (*funcallback)(void));



/*******************************************************************************
 ******************************************************************************/

#endif // _ENCODER_H_
