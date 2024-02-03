/***************************************************************************//**
  @file     encoder.c
  @brief
  @author
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "encoder.h"
#include "timer.h"
#include "board.h"
#include "gpio.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/




/******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/


/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/



/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

void encoder_isr(void);
void button_isr(void);

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

// Esta función externa puede recibir parámetros
void (*encoderCW_callback) (void);
void (*encoderCCW_callback) (void);
void (*button_long_callback) (void);
void (*button_short_callback) (void);

static int encoderTimerID;
static int buttonTimerID;

static int ALastState;
static int ACurrentState;

static int buttonLastState;
static int buttonCurrentState;

static int rotation;
static int buttonCounter;



/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/


/**
 * @brief
 * @param
 * @param
 */
void rotaryEncoder_init(void)
{
	static bool isInit = false;
	if(isInit)
	{
		return;
	}

	timerInit();

	encoderCW_callback = NULL;
	encoderCCW_callback = NULL;
	button_long_callback = NULL;
	button_short_callback = NULL;

	rotary_Init();
	button_init();

	isInit = true;
}


/**
 * @brief
 * @param
 * @param
 */
void rotary_Init(void)
{
	static bool yaInit = false;
	if(yaInit)
		return;

	gpioMode(PIN_RCHA, INPUT);
	gpioMode(PIN_RCHB, INPUT);

	encoderTimerID = timerGetId();
	timerStart(encoderTimerID, ENCODER_ISR_PERIOD, TIM_MODE_PERIODIC, encoder_isr);

	ALastState = gpioRead(PIN_RCHA);
	rotation = 0;

	yaInit = true;
}


/**
 * @brief
 * @param
 * @param
 */
void button_init(void)
{
	static bool yaInit = false;
	if(yaInit)
		return;

	gpioMode(PIN_RSW, INPUT);

	buttonLastState = gpioRead(PIN_RSW);

	buttonTimerID = timerGetId();
	timerStart(buttonTimerID, BUTTON_ISR_PERIOD, TIM_MODE_PERIODIC, button_isr);

	buttonLastState = BUTTON_RELEASED;

	buttonCounter = 0;

	yaInit = true;
}


/**
 * @brief
 * @param
 * @param
 */
int encoder_read()
{
	return rotation;
}

/**
 * @brief
 * @param
 * @param
 */
void encoder_isr(void)
{
	ACurrentState = gpioRead(PIN_RCHA);
	int BCurrentState = gpioRead(PIN_RCHB);

	if(ACurrentState != ALastState && ACurrentState == HIGH)
	{
		if(BCurrentState != ACurrentState)
		{
			rotation = CW;
			encoderCW_callback();
		}
		else
		{
			rotation = CCW;
			encoderCCW_callback();
		}
	}
	rotation = NOTHING;
	ALastState = ACurrentState;
}


/**
 * @brief
 * @param
 * @param
 */
void button_isr(void)
{
    static int longPressed = false;

    buttonCurrentState = gpioRead(PIN_RSW);

    if(buttonCurrentState == LOW)
    {
        buttonCounter++;
        if(buttonCounter >= BUTTON_LONG_ISR_COUNT)
        {
            button_long_callback();
            buttonCounter = 0;
            longPressed = true;
        }
    }
    //If the button was released, callbacks are called.
    else
    {
        if(buttonLastState == LOW && buttonCounter < BUTTON_LONG_ISR_COUNT && !longPressed)
        {
            button_short_callback();
            buttonCounter = 0;
        }
        longPressed = false;
        buttonCounter = 0;
    }
    buttonLastState = buttonCurrentState;
}

/**
 * @brief
 * @param
 * @param
 */
void set_encoderCW_callback(void (*funcallback)(void))
{
	encoderCW_callback = funcallback;
}

/**
 * @brief
 * @param
 * @param
 */
void set_encoderCCW_callback(void (*funcallback)(void))
{
	encoderCCW_callback = funcallback;
}


/**
 * @brief
 * @param
 * @param
 */
void set_button_long_callback(void (*funcallback)(void))
{
	button_long_callback = funcallback;
}


/**
 * @brief
 * @param
 * @param
 */
void set_button_short_callback(void (*funcallback)(void))
{
	button_short_callback = funcallback;
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/




/******************************************************************************/
