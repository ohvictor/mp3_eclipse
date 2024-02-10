/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "fsm_states.h"
#include "evQueue.h"
#include "fsm_motor.h"
#include "gpio.h"
#include "board.h"
#include "systick.h"
#include "time.h"
#include "controls.h"
#include "filesystem.h"
#include "LCD_GDM1602A.h"

#include <stdio.h>
#include <stdint.h>


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
void mp3_play(void);
void mp3_pause(void);
void mp3_stop(void);
void mp3_next(void);
void mp3_prev(void);
/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

//void updateViewers(char* displayArray, char LEDStatus,char selIndex);
//SD
static bool sd_state = false;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Función que se llama 1 vez, al comienzo del programa */

void App_Init (void)
{

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
