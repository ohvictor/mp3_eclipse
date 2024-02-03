#include "fsm_states.h"
#include "evQueue.h"

#include <stdbool.h>
#include <stdint.h>


/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE FOR FSM
 ******************************************************************************/
#define VOL_MAX 30
#define VOL_MIN 0

static uint8_t volume_level = 15;

/********************
 * STATES DEFINITION
 *******************/

STATE stopped[]=
{
  {NO_EV, stopped, no_action},
  {EV_PLAY, playing, music_play},
  {EV_NEXT, playing, music_next},
  {EV_PREV, playing, music_prev},
  {EV_VOL_UP, stopped, volume_increase},
  {EV_VOL_DN, stopped, volume_decrease},
  {TABLE_END, stopped, no_action}
};

STATE playing[]=
{
  {EV_STOP, stopped, music_stop},
  {EV_PAUSE, paused, music_pause},
  {EV_NEXT, playing, music_next},
  {EV_PREV, playing, music_prev},
  {EV_ERROR, stopped, music_stop},
  {EV_VOL_UP, playing, volume_increase},
  {EV_VOL_DN, playing, volume_decrease},
  {TABLE_END, stopped, no_action}
};

STATE paused[]=
{
  {NO_EV, paused, no_action},
  {EV_PLAY, playing, music_play},
  {EV_STOP, stopped, music_stop},
  {EV_NEXT, playing, music_next},
  {EV_PREV, playing, music_prev},
  {EV_ERROR, stopped, music_stop},
  {EV_VOL_UP, paused, volume_increase},
  {EV_VOL_DN, paused, volume_decrease},
  {TABLE_END, stopped, no_action}
};


/***************************
 * FSM INTERFACE DEFINITION
 **************************/

void FSM_start(void)
{
    pState=stopped;
}


STATE* FSM_getState(void)
{
	return (pState);
}


void FSM_setNewState(STATE *pNewState)
{
  pState=pNewState;
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FSM TRANSITION FUNCTIONS
 *******************************************************************************
 ******************************************************************************/

void music_play()
{
  // Play from current spot in track

}

void music_stop()
{
  // Cease playing, and return to beginning of current track

}

void music_pause()
{
  // Cease playing, but remain in current position of current track

}

void music_next()
{
  // Start from the beginning of the following track

  music_play();
}

void music_prev()
{
  // Start from the beginning of the earlier track

  music_play();
}

void volume_increase()
{
  if (volume_level < VOL_MAX)
  {
    volume_level++;

  }

}

void volume_decrease()
{
  if (volume_level > VOL_MIN)
  {
    volume_level--;

  }

}

void no_action(void)
{

}


/******************************************************************************/

