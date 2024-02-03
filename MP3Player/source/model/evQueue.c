/***************************************************************************//**
  @file     evQueue.c
  @brief    Display Board Services
  @author   uLab 2021 G1
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "evQueue.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/


/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static event_t evQueue[EV_QUEUE_SIZE];
static int evQueueWriteIndex = 0;
static int evQueueReadIndex = 0;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/


void evQueueAdd(event_t event)
{
    evQueue[evQueueWriteIndex] = event;
    evQueueWriteIndex = (evQueueWriteIndex+1)%EV_QUEUE_SIZE;
    // Could check against overflow
}


bool evQueueAvailable()
{
    return evQueue[evQueueReadIndex] != NO_EV;
}


event_t evQueueGetNext()
{
    event_t event;
    event = evQueue[evQueueReadIndex];
    evQueue[evQueueReadIndex] = NO_EV;

    evQueueReadIndex = (evQueueReadIndex + 1) % EV_QUEUE_SIZE;

    return event;
}


void evQueueClear()
{
    for (int i = 0; i < EV_QUEUE_SIZE; i++)
    {
        evQueue[i] = NO_EV;
    }
    evQueueWriteIndex = 0;
    evQueueReadIndex = 0;

}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/