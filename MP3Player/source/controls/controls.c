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

	gpioMode(PIN_PLAY, INPUT);
	gpioIRQ(PIN_PLAY, PORT_eInterruptFalling, ctrl_play_callback);

	gpioMode(PIN_PAUSE, INPUT);
	gpioIRQ(PIN_PAUSE, PORT_eInterruptFalling, ctrl_pause_callback);

	gpioMode(PIN_STOP, INPUT);
	gpioIRQ(PIN_STOP, PORT_eInterruptFalling, ctrl_stop_callback);

	gpioMode(PIN_NEXT, INPUT);
	gpioIRQ(PIN_NEXT, PORT_eInterruptFalling, ctrl_next_callback);

	gpioMode(PIN_PREV, INPUT);
	gpioIRQ(PIN_PREV, GPIO_IRQ_MODE_FALLING_EDGE, ctrl_prev_callback);
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
