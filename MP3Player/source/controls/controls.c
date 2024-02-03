/*******************************************************************************
  @file     controls.c
  @brief    Configuración de controles.
  @author   Grupo 3
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "controls.h"
#include "gpio.h"
#include "PortConfig.h"

#include <stdio.h>

void (*ctrl_play_callback) (void);
void (*ctrl_pause_callback) (void);
void (*ctrl_stop_callback) (void);
void (*ctrl_next_callback) (void);
void (*ctrl_prev_callback) (void);

void control_init()
{
	ctrl_play_callback = NULL;
	ctrl_pause_callback = NULL;
	ctrl_stop_callback = NULL;
	ctrl_next_callback = NULL;
	ctrl_prev_callback = NULL;
	// Todos los GPIO para botones en pullup, habilitando irq en flanco negativo
	gpioMode(PIN_PLAY,INPUT_PULLUP);
	if(gpioIRQ(PIN_PLAY, GPIO_IRQ_MODE_FALLING_EDGE, ctrl_play_callback))
	{
		//printf("Play button event registered");
	}

	gpioMode(PIN_PAUSE,INPUT_PULLUP);
	if(gpioIRQ(PIN_PAUSE, GPIO_IRQ_MODE_FALLING_EDGE, ctrl_pause_callback))
	{
		//printf("Pause button event registered");
	}

	gpioMode(PIN_STOP,INPUT_PULLUP);
	if(gpioIRQ(PIN_STOP, GPIO_IRQ_MODE_FALLING_EDGE, ctrl_stop_callback))
	{
		//printf("Stop button event registered");
	}

	gpioMode(PIN_NEXT,INPUT_PULLUP);
	if(gpioIRQ(PIN_NEXT, GPIO_IRQ_MODE_FALLING_EDGE, ctrl_next_callback))
	{
		//printf("Next button event registered");
	}

	gpioMode(PIN_PREV,INPUT_PULLUP);
	if(gpioIRQ(PIN_PREV, GPIO_IRQ_MODE_FALLING_EDGE, ctrl_prev_callback))
	{
		//printf("Prev button event registered");
	}

}


void ctrl_set_play_callback(void (*func_callback)(void))
{
	ctrl_play_callback = func_callback;
}

void ctrl_set_pause_callback(void (*func_callback)(void))
{
	ctrl_pause_callback = func_callback;
}

void ctrl_set_stop_callback(void (*func_callback)(void))
{
	ctrl_stop_callback = func_callback;
}

void ctrl_set_next_callback(void (*func_callback)(void))
{
	ctrl_next_callback = func_callback;
}

void ctrl_set_prev_callback(void (*func_callback)(void))
{
	ctrl_prev_callback = func_callback;
}
