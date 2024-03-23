/***************************************************************************//**
  @file     DRV_Encoder.h
  @brief    Rotary encoder driver
  @author   Grupo 1
 ******************************************************************************/

#ifndef DRV_ENCODER_H_
#define DRV_ENCODER_H_


/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "PDRV_GPIO.h"

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef enum {
	ENC_LEFT,
	ENC_RIGHT,
	ENC_CLICK,
	ENC_NONE
} encoderResult_t;

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Initialize encoder
 */
void initEncoder();

/**
 * @brief Check for new encoder events
 * @return true if there is an encoder event
 */
bool encoderGetStatus();

/**
 * @brief Get event type
 * @return event
 */
encoderResult_t encoderGetEvent();

/*******************************************************************************
 ******************************************************************************/

#endif /* DRV_ENCODER_H_ */
