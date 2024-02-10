#ifndef SOURCES_FSMSTATES_H_
#define SOURCES_FSMSTATES_H_

#include <stddef.h>
#include "fsm_motor.h"

/**********************
 * EVENTS DECLARATION *
 * ********************/

typedef enum{
    NO_EV,
    EV_PLAY,
    EV_STOP,
    EV_PAUSE,
    EV_NEXT,
    EV_PREV,
    EV_ERROR,
    EV_VOL_UP,
    EV_VOL_DN,
    EV_TOTAL
} event_t;

/*********************/
/* STATES DECLARATION*/
/*********************/

extern STATE stopped[];
extern STATE playing[];
extern STATE paused[];


/*FSM INTERFACE FUNCTIONS DECLARATION */

STATE* FSM_getState(void);
void FSM_start(void);
void FSM_setNewState(STATE *pNewState);

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

void music_play();
void music_stop();
void music_pause();
void music_next();
void music_prev();

void volume_increase();
void volume_decrease();

void no_action();


#endif /* SOURCES_FSMSTATES_H_ */
