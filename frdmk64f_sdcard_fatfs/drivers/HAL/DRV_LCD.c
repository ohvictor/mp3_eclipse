/***************************************************************************//**
  @file     +Nombre del archivo (ej: template.c)+
  @brief    +Descripcion del archivo+
  @author   +Nombre del autor (ej: Salvador Allende)+
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "DRV_LCD.h"
#include "PDRV_GPIO.h"
#include "DRV_Timers.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define DELAY_UNIT  2

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

enum { RS_IR, RS_DR };    // Instruction Register, Data Register
enum { DEC, INC };
enum { CURSORMOVE, DISPLAYSHIFT };
enum {     // TBD
    D4_PIN = PORTNUM2PIN(PC, 9),
    D5_PIN = PORTNUM2PIN(PC, 0),
    D6_PIN = PORTNUM2PIN(PC, 7),
    D7_PIN = PORTNUM2PIN(PC, 5),
    RS_PIN = PORTNUM2PIN(PB, 19),
    ENABLE_PIN = PORTNUM2PIN(PC, 8),
    RW_PIN = PORTNUM2PIN(PC, 1)
};

/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void lcd_write(uint8_t value, uint8_t rs, bool nibble);
static void lcd_write_fsm_callback();
static void lcd_timer_delay(ttick_t ticks);

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static const int GPIO_PINS[] = {D4_PIN, D5_PIN, D6_PIN, D7_PIN};

static bool lcd_config_busy = false;
static bool lcd_write_busy = false;
static uint8_t lcd_write_fsm_state = 0;
static tim_id_t lcd_config_tim_id;
static tim_id_t lcd_write_tim_id;
static tim_id_t lcd_timer_delay_id;
static lcd_config_t lcd_config;

static uint8_t fsm_value;
static bool fsm_rs;
static bool fsm_nibble;

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void lcd_init(){
    static bool already_init = false;
    if(already_init) return;

    already_init = true;
    lcd_config_busy = false;
    lcd_write_busy = false;

    initTimers();
    lcd_config_tim_id = timerGetId();
    lcd_write_tim_id = timerGetId();
	lcd_timer_delay_id = timerGetId();

    for(char i = 0; i < NUM_BITS; i++){
        gpioMode(GPIO_PINS[i], OUTPUT);
    }
    gpioMode(RS_PIN, OUTPUT);
    gpioMode(ENABLE_PIN, OUTPUT);
    gpioMode(RW_PIN, OUTPUT);
}

bool lcd_is_busy(){
    return lcd_write_busy;
}

void lcd_configure(lcd_config_t config){
    lcd_config_busy = true;
    lcd_config = config;

    gpioWrite(RS_PIN, 0);
    gpioWrite(ENABLE_PIN, 0);
    gpioWrite(RW_PIN, 0);

    lcd_write_ins(0b0011, true);
    lcd_timer_delay(6*DELAY_UNIT);
    lcd_write_ins(0b0011, true);
    lcd_timer_delay(2*DELAY_UNIT);
    lcd_write_ins(0b0011, true);
    lcd_timer_delay(2*DELAY_UNIT);
    lcd_write_ins(0b0010, true);  // Selects 4-bit interface
    lcd_timer_delay(2*DELAY_UNIT);
    lcd_write_ins(FUNCTION_SET(lcd_config.interface, lcd_config.lines, lcd_config.font), false);
    lcd_timer_delay(4*DELAY_UNIT);
    lcd_write_ins(DISPLAY_CONTROL(DISPLAYOFF, lcd_config.cursor, lcd_config.blink), false);
    lcd_timer_delay(4*DELAY_UNIT);
    lcd_write_ins(CLEAR_DISPLAY, false);
    lcd_timer_delay(4*DELAY_UNIT);
    lcd_write_ins(ENTRY_MODE_SET(lcd_config.shift_dir, lcd_config.shift), false);
    lcd_timer_delay(4*DELAY_UNIT);
    lcd_write_ins(DISPLAY_CONTROL(lcd_config.display, lcd_config.cursor, lcd_config.shift), false);
    lcd_timer_delay(4*DELAY_UNIT);
    lcd_write_ins(CLEAR_DISPLAY, false);
    lcd_timer_delay(4*DELAY_UNIT);
    lcd_write_ins(RETURN_HOME, false);
    lcd_timer_delay(4*DELAY_UNIT);

    lcd_config_busy = false;
}

void lcd_write_ins(uint8_t value, bool nibble){
	if(!nibble){
        value = (value & 0x0F) << 4 | (value & 0xF0) >> 4; // Swap nibbles
	}
    lcd_write(value, RS_IR, nibble);
}

void lcd_write_data(uint8_t value, bool nibble){
	if(!nibble){
        value = (value & 0x0F) << 4 | (value & 0xF0) >> 4; // Swap nibbles
	}
    lcd_write(value, RS_DR, nibble);
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void lcd_write_gpio(uint8_t bits){
    for(uint8_t i = 0; i < NUM_BITS; i++){
        gpioWrite(GPIO_PINS[i], (bits >> i) & 0x01);
    }
}

static void lcd_write(uint8_t value, uint8_t rs, bool nibble){
    lcd_write_busy = true;
    lcd_write_fsm_state = 0;

    fsm_value = value;
    fsm_rs = rs;
    fsm_nibble = nibble;
    
    lcd_write_fsm_callback();
}

static void lcd_write_fsm_callback(){
    switch(lcd_write_fsm_state){
        case 0:
            //lcd_write_gpio(0);
            gpioWrite(ENABLE_PIN, 0);
            lcd_write_gpio(fsm_value & 0x0F);
            gpioWrite(RS_PIN, fsm_rs);
            gpioWrite(ENABLE_PIN, 1);
            lcd_write_fsm_state = 1;
            timerStart(lcd_write_tim_id, DELAY_UNIT, TIM_MODE_SINGLESHOT, lcd_write_fsm_callback);
            break;
        case 1:
            gpioWrite(ENABLE_PIN, 0);
            if(fsm_nibble){
                lcd_write_fsm_state = 2;
                timerStart(lcd_write_tim_id, DELAY_UNIT, TIM_MODE_SINGLESHOT, lcd_write_fsm_callback);
            } else {
                fsm_value >>= 4;
                fsm_nibble = true;
                lcd_write_fsm_state = 0;
                timerStart(lcd_write_tim_id, DELAY_UNIT, TIM_MODE_SINGLESHOT, lcd_write_fsm_callback);
            }
            break;
        case 2:
            lcd_write_busy = false;
            break;
    }
}

void lcd_timer_delay(ttick_t ticks){
	timerStart(lcd_timer_delay_id, ticks, TIM_MODE_SINGLESHOT, NULL);
	while ( !timerExpired(lcd_timer_delay_id) ){
		//wait
	}
	timerStop(lcd_timer_delay_id);
}
