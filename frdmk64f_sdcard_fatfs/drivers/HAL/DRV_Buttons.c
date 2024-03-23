/***************************************************************************//**
  @file     DRV_Buttons.c
  @brief    Implement buttons driver for FRDM-K64F board with short and long press
  @author   Pablo Gonzalez Leiro
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "DRV_Buttons.h"
#include "DRV_Timers.h"
#include "PDRV_GPIO.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#ifdef BUTTONS_DEBUG
#define LED_ON false
#define LED_OFF true
#endif

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef struct {
    pin_t pin;
    bool button_pressed;
    tim_id_t timerId;
    pinIrqFun_t handler;
    uint16_t shortPressThreshold;
    pressstatus_t status;
    bool* flagOmitAction;
    bool event;
    #ifdef BUTTONS_DEBUG
    pin_t pinLed;
    #endif
} button_t;


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static pressstatus_t getButtonStatus(button_t* button);

static void button1Handler(void);
static void button2Handler(void);
static void buttonEncoderHandler(void);
static void genericButtonHandler(button_t* button);

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

#ifdef BUTTONS_DEBUG
#define pinLedRed   PORTNUM2PIN(PB, 22)
#define pinLedGreen PORTNUM2PIN(PE, 26)
#define pinLedBlue  PORTNUM2PIN(PB, 21)
#endif

bool flagOmitAction = false;

static button_t button1 = {
    .pin = PORTNUM2PIN(PC, 16),
    .button_pressed = false,
    .timerId = TIMER_INVALID_ID,
    .handler = button1Handler,
    .shortPressThreshold = SHORT_PRESS_THRESHOLD,
    .status = NOPRESS,
    .flagOmitAction = &flagOmitAction,
    .event = false,
    #ifdef BUTTONS_DEBUG
    .pinLed = pinLedRed
    #endif
};

static button_t button2 = {
    .pin = PORTNUM2PIN(PC, 17),
    .button_pressed = false,
    .timerId = TIMER_INVALID_ID,
    .handler = button2Handler,
    .shortPressThreshold = SHORT_PRESS_THRESHOLD,
    .status = NOPRESS,
    .flagOmitAction = &flagOmitAction,
    .event = false,
    #ifdef BUTTONS_DEBUG
    .pinLed = pinLedGreen
    #endif
};

static button_t buttonEncoder = {
    .pin = PORTNUM2PIN(PA, 2),
    .button_pressed = false,
    .timerId = TIMER_INVALID_ID,
    .handler = buttonEncoderHandler,
    .shortPressThreshold = SHORT_PRESS_THRESHOLD,
    .status = NOPRESS,
    .flagOmitAction = &flagOmitAction,
    .event = false,
    #ifdef BUTTONS_DEBUG
    .pinLed = pinLedBlue
    #endif
};


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void buttonsInit(void) {
    button1.timerId = timerGetId();
	button2.timerId = timerGetId();
    buttonEncoder.timerId = timerGetId();

	// push buttons
	gpioMode( button1.pin, INPUT );
	gpioMode( button2.pin, INPUT );
    gpioMode( buttonEncoder.pin, INPUT );

	gpioIRQ( button1.pin, GPIO_IRQ_MODE_BOTH_EDGES, button1Handler );
	gpioIRQ( button2.pin, GPIO_IRQ_MODE_BOTH_EDGES, button2Handler );
    gpioIRQ( buttonEncoder.pin, GPIO_IRQ_MODE_BOTH_EDGES, buttonEncoderHandler );

    #ifdef BUTTONS_DEBUG
	gpioMode( button1.pinLed, OUTPUT );
	gpioMode( button2.pinLed, OUTPUT );
	gpioMode( buttonEncoder.pinLed, OUTPUT );

	gpioWrite( button1.pinLed, LED_OFF );
	gpioWrite( button2.pinLed, LED_OFF );
	gpioWrite( buttonEncoder.pinLed, LED_OFF );
    #endif
}

pressstatus_t getButton1Status(void) {
    return getButtonStatus(&button1);
}

pressstatus_t getButton2Status(void) {
    return getButtonStatus(&button2);
}

pressstatus_t getButtonEncoderStatus(void) {
    return getButtonStatus(&buttonEncoder);
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static pressstatus_t getButtonStatus(button_t* button) {
    if( button->event ) {
        button->event = false;
        return button->status;
    }
    else {
        return NOPRESS;
    }
}

static void button1Handler(void) {
    genericButtonHandler(&button1);
}

static void button2Handler(void) {
    genericButtonHandler(&button2);
}

static void buttonEncoderHandler(void) {
    genericButtonHandler(&buttonEncoder);
}

static void genericButtonHandler(button_t* button) {

    // PRESSED
    if( gpioRead(button->pin) == button->button_pressed ) {
        if( *button->flagOmitAction ) {
            return;
        }
        else {
            *button->flagOmitAction = true;
        }
        #ifdef BUTTONS_DEBUG
        gpioToggle( button->pinLed);
        #endif
        button->status = PRESSING;
        //Seteo el timer para que llame periodicamente a encoder_callback con SHORT_PRESS_THRESHOLD
        timerStart(button->timerId, TIMER_MS2TICKS(button->shortPressThreshold), TIM_MODE_SINGLESHOT, NULL);
    }
    // RELEASED
    else if( button->status == PRESSING ) {
        #ifdef BUTTONS_DEBUG
        gpioToggle( button->pinLed );
        #endif
        if( timerExpired(button->timerId) ) {
            button->status = LONGPRESS;
        }
        else {
            timerStop(button->timerId);
            button->status = SHORTPRESS;
        }
        button->event = true;
        *button->flagOmitAction = false;
    }
 }
