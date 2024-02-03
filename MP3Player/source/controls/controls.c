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

pinIrqFun_t irq_ev_gen_play(void);
pinIrqFun_t irq_ev_gen_pause(void);
pinIrqFun_t irq_ev_gen_stop(void);
pinIrqFun_t irq_ev_gen_next(void);
pinIrqFun_t irq_ev_gen_prev(void);

void control_init()
{
  // Todos los GPIO para botones en pullup, habilitando irq en flanco negativo
  gpioMode(PORTNUM2PIN(PB,2),INPUT_PULLUP);
  if(gpioIRQ(PORTNUM2PIN(PB,2), GPIO_IRQ_MODE_FALLING_EDGE, irq_ev_gen_play))
  {
    printf("Play button event registered");
  }
  
  gpioMode(PORTNUM2PIN(PB,3),INPUT_PULLUP);
  if(gpioIRQ(PORTNUM2PIN(PB,3), GPIO_IRQ_MODE_FALLING_EDGE, irq_ev_gen_pause))
  {
    printf("Pause button event registered");
  }
  
  gpioMode(PORTNUM2PIN(PB,10),INPUT_PULLUP);
  if(gpioIRQ(PORTNUM2PIN(PB,10), GPIO_IRQ_MODE_FALLING_EDGE, irq_ev_gen_stop))
  {
    printf("Stop button event registered");
  }
  
  gpioMode(PORTNUM2PIN(PC,11),INPUT_PULLUP);
  if(gpioIRQ(PORTNUM2PIN(PC,11), GPIO_IRQ_MODE_FALLING_EDGE, irq_ev_gen_next))
  {
    printf("Next button event registered");
  }
  
  gpioMode(PORTNUM2PIN(PC,10),INPUT_PULLUP);
  if(gpioIRQ(PORTNUM2PIN(PC,10), GPIO_IRQ_MODE_FALLING_EDGE, irq_ev_gen_prev))
  {
    printf("Prev button event registered");
  }

}

pinIrqFun_t irq_ev_gen_play(void)
{
  evQueueAdd(EV_PLAY);
}

pinIrqFun_t irq_ev_gen_pause(void)
{
  evQueueAdd(EV_PAUSE);
}

pinIrqFun_t irq_ev_gen_stop(void)
{
  evQueueAdd(EV_STOP);
}

pinIrqFun_t irq_ev_gen_next(void)
{
  evQueueAdd(EV_NEXT);
}

pinIrqFun_t irq_ev_gen_prev(void)
{
  evQueueAdd(EV_PREV);
}
