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

#include <stdio.h>
#include <stdint.h>

#include "vumeter.h"
#include "timer.h"

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

void display_callback(void);
/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

tim_id_t display_timer_id;
static uint8_t powers[8];

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
	ctrl_set_play_callback(mp3_play);
	ctrl_set_pause_callback(mp3_pause);
	ctrl_set_stop_callback(mp3_stop);
	ctrl_set_next_callback(mp3_next);
	ctrl_set_prev_callback(mp3_prev);
	control_init();


	gpioMode(PIN_LED_GREEN, OUTPUT);
	gpioMode(PIN_LED_BLUE, OUTPUT);
	gpioMode(PIN_LED_RED, OUTPUT);


	FSM_start();

	vu_init();
	int i;
	for(i=0; i<8; i++)
	{
		powers[i] = 0xff-i*32;
	}
	vu_set_power(powers);
	vu_update();

	timerInit();
	display_timer_id = timerGetId();
	timerStart(display_timer_id, 500, TIM_MODE_PERIODIC, display_callback);
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

void display_callback(void)
{
	/*
	int i;
	for(i=0; i<8; i++)
	{
		powers[i] += 0x20;
	}
	*/
	//vu_update(powers);
	vu_update();
}


/*******************************************************************************
 ******************************************************************************/
