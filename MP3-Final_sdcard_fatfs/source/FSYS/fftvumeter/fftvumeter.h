/***************************************************************************//**
  @file     fftvumeter.h
  @brief
  @author
  @date
 ******************************************************************************/

#ifndef FFTVUMETER_H_
#define FFTVUMETER_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/


#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include "arm_math.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define FFT_SIZE	1024
#define NUMBER_OF_BANDS 8


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*
 * @brief
 */
void fftvumeter_init();

/*
 * @brief
 */
bool fftvumeter_calculate_power(float * signal, float sampleRate, int lowerFreqBand, int higherFreqBand);

/*
 * @brief
 */
uint8_t* fftvumeter_get_power();



/*******************************************************************************
 ******************************************************************************/

#endif // FFTVUMETER_H_