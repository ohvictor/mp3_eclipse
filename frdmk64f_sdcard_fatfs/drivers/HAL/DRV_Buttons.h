/***************************************************************************//**
  @file     DRV_Buttons.h
  @brief    Implement buttons driver for FRDM-K64F board with short and long press
  @author   Pablo Gonzalez Leiro
 ******************************************************************************/

#ifndef _DRV_BUTTONS_H_
#define _DRV_BUTTONS_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/



/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define BUTTONS_DEBUG
#define SHORT_PRESS_THRESHOLD 600 //ms

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef enum {
	NOPRESS = 0,
	PRESSING,
	SHORTPRESS,
	LONGPRESS,
} pressstatus_t;

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Initialize buttons driver
 * 
 */
void buttonsInit(void);

/**
 * @brief Get button 1 status
 * 
 * @return pressstatus_t 
 */
pressstatus_t getButton1Status(void);

/**
 * @brief Get the Button2 Status object
 * 
 * @return pressstatus_t 
 */
pressstatus_t getButton2Status(void);

/**
 * @brief Get the Button Encoder Status object
 * 
 * @return pressstatus_t 
 */
pressstatus_t getButtonEncoderStatus(void);

/*******************************************************************************
 ******************************************************************************/

#endif // _DRV_BUTTONS_H_
