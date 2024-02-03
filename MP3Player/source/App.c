/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "fsm_states.h"
#include "evQueue.h"
#include "fsm_motor.h"
#include "controls.h"

#include <stdio.h>
#include <stdint.h>


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

//static void delayLoop(uint32_t veces);

//void updateViewers(char* displayArray, char LEDStatus,char selIndex);

void mp3_play(void);
void mp3_pause(void);
void mp3_stop(void);
void mp3_next(void);
void mp3_prev(void);


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Función que se llama 1 vez, al comienzo del programa */

void App_Init (void)
{
	control_init();
	ctrl_set_play_callback(mp3_play);
	ctrl_set_pause_callback(mp3_pause);
	ctrl_set_stop_callback(mp3_stop);
	ctrl_set_next_callback(mp3_next);
	ctrl_set_prev_callback(mp3_prev);


	FSM_start();


}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
{
	if(evQueueAvailable())
	{
		FSM_setNewState(fsm(FSM_getState(),evQueueGetNext()));

	}

}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void mp3_play(void)
{
	evQueueAdd(EV_PLAY);
}

void mp3_pause(void)
{
	evQueueAdd(EV_PAUSE);
}

void mp3_stop(void)
{
	evQueueAdd(EV_STOP);
}

void mp3_next(void)
{
	evQueueAdd(EV_NEXT);
}

void mp3_prev(void)
{
	evQueueAdd(EV_PREV);
}




/*******************************************************************************
 ******************************************************************************/
