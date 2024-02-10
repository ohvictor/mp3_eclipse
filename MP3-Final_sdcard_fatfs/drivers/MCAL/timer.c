/***************************************************************************//**
  @file     timer.c
  @brief    Timer driver. Advance implementation
  @author   Facundo Molina
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "systick.h"
#include "timer.h"



/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#if TIMER_TICK_MS != (1000U/SYSTICK_ISR_FREQUENCY_HZ)
#error Las frecuencias no coinciden!!
#endif // TIMER_TICK_MS != (1000U/SYSTICK_ISR_FREQUENCY_HZ)

#define TIMER_DEVELOPMENT_MODE    1

#define TIMER_ID_INTERNAL   0


/******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef struct {
	ttick_t             period;
	ttick_t             cnt;
    tim_callback_t      callback;
    uint8_t             mode        : 1;
    uint8_t             running     : 1;
    uint8_t             expired     : 1;
    uint8_t             unused      : 5;
} myTimer_t;


/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

/**
 * @brief Periodic service
 */
static void timer_isr(void);
int getArrayEffectiveLength();


/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static myTimer_t timers[TIMERS_MAX_CANT];
static tim_id_t timers_cant = TIMER_ID_INTERNAL+1;


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void timerInit(void)
{
    static bool isInit = false;
    if (isInit)
    {
        return;
    }

    SysTick_Init(timer_isr); // init peripheral

    timers[TIMER_ID_INTERNAL].period = TIMER_INTERNAL_PERIOD;
    timers[TIMER_ID_INTERNAL].cnt = TIMER_INTERNAL_PERIOD;
    timers[TIMER_ID_INTERNAL].callback = false;
    timers[TIMER_ID_INTERNAL].running = true;
    timers[TIMER_ID_INTERNAL].mode = TIM_MODE_PERIODIC;
    timers[TIMER_ID_INTERNAL].expired = 0;

    isInit = true;
}


tim_id_t timerGetId(void)
{
#ifdef TIMER_DEVELOPMENT_MODE
    if (timers_cant >= TIMERS_MAX_CANT)
    {
        return TIMER_INVALID_ID;
    }
    else
#endif // TIMER_DEVELOPMENT_MODE
    {
        return timers_cant++;
    }
}


void timerStart(tim_id_t id, ttick_t ticks, uint8_t mode, tim_callback_t callback)
{
#ifdef TIMER_DEVELOPMENT_MODE
    if ((id < timers_cant) && (mode < CANT_TIM_MODES))
#endif // TIMER_DEVELOPMENT_MODE
    {
        // disable timer
    	timers[id].running = 0;
        // configure timer

    	timers[id].period = ticks;
    	timers[id].cnt = ticks;
    	timers[id].callback = callback;
    	timers[id].mode = mode;
    	timers[id].expired = 0;

        // enable timer
    	timers[id].running = 1;
    }
}


void timerStop(tim_id_t id)
{
    // ****** COMPLETAR ******
    // Apago el timer
    // y bajo el flag
	timers[id].running = 0;
}


bool timerExpired(tim_id_t id)
{
    // ****** COMPLETAR ******
    // Verifico si expirÃ³ el timer
    // y bajo el flag
	if(timers[id].cnt == 0){
		timers[id].expired = 1;
		return true;
	}
	return false;
}


void timerDelay(ttick_t ticks)
{
    timerStart(TIMER_ID_INTERNAL, ticks, TIM_MODE_SINGLESHOT, NULL);
    while (!timerExpired(TIMER_ID_INTERNAL))
    {
    	timers[TIMER_ID_INTERNAL].cnt--;
    }
    timers[TIMER_ID_INTERNAL].cnt = timers[TIMER_ID_INTERNAL].period;
}


bool timerGetStatus(int id)
{
	bool idFound = false;												 //Flag
	int i = 0;															 //Index
	int arrayEffectiveLength = getArrayEffectiveLength(); //Amount of not NULL elements in sysTickElements.
	/*Searches for the id in the array*/
	while ((idFound == false) && (i < arrayEffectiveLength))
	{
		if (timers[i].callback == id) //ID found.
		{
			return (timers[i].cnt == 0); //Resumes the calling of the callback.
		}
		i++;
	}
	return false;
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void timer_isr(void)
{
    // ****** COMPLETAR ******
    // decremento los timers activos
    // si hubo timeout!
    // 1) execute action: callback or set flag
    // 2) update state
	for(int i = TIMER_ID_INTERNAL + 1; i < timers_cant; i++){
		if(timers[i].running)
		{
			--timers[i].cnt;
			if(timerExpired(i))
			{
				if(timers[i].callback != NULL)
				{
					timers[i].callback();
				}
				if(timers[i].mode == TIM_MODE_SINGLESHOT){
					timerStop(i);
				}
				else if(timers[i].mode == TIM_MODE_PERIODIC){
					timers[i].expired = 0;
					timers[i].cnt = timers[i].period;
				}
			}
		}
	}
}


int getArrayEffectiveLength()
{
	int i = 0;				//Index
	int j = 0;
	bool foundLast = false; //Flag
	while (i < timers_cant)
	{
		// check if timer is active
		if (timers[i].callback != NULL && timers[i].running == 1 && timers[i].expired == 0)
			j++;
		i++;
	}
	return j;
}


/******************************************************************************/
