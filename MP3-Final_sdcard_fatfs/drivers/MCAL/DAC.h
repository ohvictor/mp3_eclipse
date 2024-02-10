/***************************************************************************//**
  @file     DAC.h
  @brief    
  @author   
  @date		
 ******************************************************************************/

#ifndef DAC_DAC_H_
#define DAC_DAC_H_

#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
typedef enum {DAC_0, DAC_1} DAC_n;

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/
typedef uint16_t DACdata_t;

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

void DAC_init (DAC_n dac_n);

/*******************************************************************************
 ******************************************************************************/

#endif // _DAC_DAC_H_