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
#include "evQueue.h"

#include <stdio.h>

void (*ctrl_play_callback) (void);
void (*ctrl_pause_callback) (void);
void (*ctrl_stop_callback) (void);
void (*ctrl_next_callback) (void);
void (*ctrl_prev_callback) (void);

void control_init()
{
  // Todos los GPIO para botones en pullup, habilitando irq en flanco negativo
  gpioMode(PORTNUM2PIN(PB,2),INPUT_PULLUP);
  if(gpioIRQ(PORTNUM2PIN(PB,2), GPIO_IRQ_MODE_FALLING_EDGE, ctrl_play_callback))
  {
    printf("Play button event registered");
  }
  
  gpioMode(PORTNUM2PIN(PB,3),INPUT_PULLUP);
  if(gpioIRQ(PORTNUM2PIN(PB,3), GPIO_IRQ_MODE_FALLING_EDGE, ctrl_pause_callback))
  {
    printf("Pause button event registered");
  }
  
  gpioMode(PORTNUM2PIN(PB,10),INPUT_PULLUP);
  if(gpioIRQ(PORTNUM2PIN(PB,10), GPIO_IRQ_MODE_FALLING_EDGE, ctrl_stop_callback))
  {
    printf("Stop button event registered");
  }
  
  gpioMode(PORTNUM2PIN(PC,11),INPUT_PULLUP);
  if(gpioIRQ(PORTNUM2PIN(PC,11), GPIO_IRQ_MODE_FALLING_EDGE, ctrl_next_callback))
  {
    printf("Next button event registered");
  }
  
  gpioMode(PORTNUM2PIN(PC,10),INPUT_PULLUP);
  if(gpioIRQ(PORTNUM2PIN(PC,10), GPIO_IRQ_MODE_FALLING_EDGE, ctrl_prev_callback))
  {
    printf("Prev button event registered");
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
