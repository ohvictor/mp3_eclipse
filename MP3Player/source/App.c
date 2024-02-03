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

#include "fileSystem.h"

#include <stdio.h>
#include <stdint.h>


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

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
	FSM_start();
	bool r = init_filesys();
	loadSDWrapper();
}

void loadSDWrapper(){
    if (init_filesys()){
        if(statrt_mapping()){
            sd_state = true;    
        }
        else{
            sd_state = false;
        }
    }
    else{
        sd_state = false;
    }
    
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






/*******************************************************************************
 ******************************************************************************/
