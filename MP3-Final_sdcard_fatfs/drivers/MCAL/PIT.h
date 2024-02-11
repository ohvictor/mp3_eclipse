/***************************************************************************//**
  @file     PIT.h
  @brief    
  @author   
 ******************************************************************************/

#ifndef PIT_PIT_H_
#define PIT_PIT_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include <stdint.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define PIT_TICK_TIME	20UL	// ns

#define PIT_TICK2NS(x)	((x)*PIT_TICK_TIME)			// ns
#define PIT_TICK2MS(x)	((x)*PIT_TICK_TIME/1000000UL)	// ms

#define PIT_NS2TICK(x)	(((x)/PIT_TICK_TIME)-1)
#define PIT_US2TICK(x)	(((x)*(1000UL/PIT_TICK_TIME))-1UL)
#define PIT_MS2TICK(x)	(((x)*(1000000UL/PIT_TICK_TIME))-1UL)

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef enum {PIT_0, PIT_1, PIT_2, PIT_3, PIT_COUNT} PIT_MODE;

typedef void (*PIT_callback_t) (void);

typedef uint32_t PIT_Tick_t;


/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/
/*
* @brief  PIT channel init
* @param  PIT channel
*/
void PIT_init(PIT_MODE pit, PIT_callback_t callback);

/*
* @brief  sets PIT channel period
* @param  PIT channel
* @param  ticks to load in counter 
*/
void PIT_set_interval(PIT_MODE pit, uint32_t ticks);

/*
* @brief  PIT channel start
* @param  PIT channel
*/
void PIT_start(PIT_MODE pit);

/*
* @brief  PIT channel stop
* @param  PIT channel
*/
void PIT_stop(PIT_MODE chanpitnel);



/*******************************************************************************
 ******************************************************************************/


#endif /* PIT_PIT_H_ */
