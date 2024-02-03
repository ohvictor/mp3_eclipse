/*******************************************************************************
  @file     evQueue.h
  @brief    event queue services
  @author   uLab 2021 G1
 ******************************************************************************/

#ifndef __EVQUEUE_
#define __EVQUEUE_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include <stdbool.h>

#include "fsm_states.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define EV_QUEUE_SIZE 10

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
 * @brief Insert event to event queue
 * @param event incoming event
 */
void evQueueAdd(event_t event);

/**
 * @brief Reports if there's an event waiting in queue
 * @return BOOL
 */
bool evQueueAvailable();

/**
 * @brief Gets the next event in queue
 * @return next event
 */
event_t evQueueGetNext();

/**
 * @brief Clears all events in queue
 */
void evQueueClear();

/*******************************************************************************
 ******************************************************************************/

#endif //__EVQUEUE_
