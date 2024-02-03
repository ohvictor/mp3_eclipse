/***************************************************************************//**
  @file     displayBoard.h
  @brief    Display Board Services
  @author   uLab 2021 G1
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "displayBoard.h"
#include "timer.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
enum {SEGA,SEGB,SEGC,SEGD,SEGE,SEGF,SEGG,SEGDP};
enum {SEL0,SEL1};

#define EVAL_BIT(val,x) (((1<<x) & val) && (1<<x))

typedef uint8_t SEG_type;
typedef struct {
	SEG_type segments[NUM_DIGITS];
	LEDStat_type status;
} Display_type;

//#define REFRESH_FREQ	2000	// Hz
//#define REFRESH_PERIOD	(1/REFRESH_FREQ*1000) // ms
#define REFRESH_PERIOD 100


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
/*
 * @brief Writes the symbol data into the GPIO ports ("bus")
 * @param seg the segment data to be written
 */
void digit_set(SEG_type seg);

void digit_sel(int i);

void digit_refresh(SEG_type seg, int i);

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
static Display_type display;

static tim_id_t displayTimerID;

//static uint8_t brightnessLevel;

static uint8_t br_level;

void DispBoard_Init()
{
	display.segments[0] = symDash;
	display.segments[1] = symDash;
	display.segments[2] = symDash;
	display.segments[3] = symDash;
	// Inicializar en modo OUTPUT los pines conectados a segmentos
	gpioMode(PIN_SEG_A,OUTPUT);
	gpioMode(PIN_SEG_B,OUTPUT);
	gpioMode(PIN_SEG_C,OUTPUT);
	gpioMode(PIN_SEG_D,OUTPUT);
	gpioMode(PIN_SEG_E,OUTPUT);
	gpioMode(PIN_SEG_F,OUTPUT);
	gpioMode(PIN_SEG_G,OUTPUT);
	gpioMode(PIN_SEG_DP,OUTPUT);

	// Inicializar en modo OUTPUT los pines conectados a SEL
	gpioMode(PIN_SEL_0,OUTPUT);
	gpioMode(PIN_SEL_1,OUTPUT);

	// Inicializar en modo OUTPUT los pines de STATUS
	gpioMode(PIN_STA_0,OUTPUT);
	gpioMode(PIN_STA_1,OUTPUT);

	// Inicializar en modo INPUT los pines del Rotary Encoder
	// Considerar el setup de las interrupciones
	gpioMode(PIN_RCHA,INPUT);
	gpioMode(PIN_RCHB,INPUT);
	gpioMode(PIN_RSW ,INPUT);

	// Timer for Refresh
	br_level = 1;
	displayTimerID = timerGetId();
	timerStart(displayTimerID, br_level, TIM_MODE_PERIODIC, display_refresh);

}

void digit_write(sym_type symbol, int digit, bool dp)
{
	SEG_type write_data = 0x00;
	write_data |= symbol;

	if(dp)
	{
		write_data |= 0x80;
	}

	display.segments[digit] = write_data;
}

void display_refresh()
{
	int i;
	for (i=0;i<NUM_DIGITS;i++)
	{
		digit_refresh(display.segments[i],i);
	}
	gpioWrite(PIN_STA_0,EVAL_BIT(display.status,0));
	gpioWrite(PIN_STA_1,EVAL_BIT(display.status,1));
}

void status_led_write(LEDStat_type status)
{
	display.status = status;
}

void status_led_refresh()
{
	gpioWrite(PIN_STA_0, EVAL_BIT(display.status,0));
	gpioWrite(PIN_STA_1, EVAL_BIT(display.status,1));
}

void brightness_set(uint8_t level)
{
	br_level = REFRESH_PERIOD / (level+1)/10;
	timerStart(displayTimerID, br_level, TIM_MODE_PERIODIC, display_refresh);
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void digit_refresh(SEG_type seg, int i)
{
	digit_set(symClr);
	digit_sel(i);
	digit_set(display.segments[i]);
	digit_set(symClr);
}

void digit_sel(int i)
{
	gpioWrite(PIN_SEL_0, EVAL_BIT(i, SEL0));
	gpioWrite(PIN_SEL_1, EVAL_BIT(i, SEL1));
}

void digit_set(SEG_type seg)
{
	gpioWrite(PIN_SEG_A, EVAL_BIT(seg,SEGA));
	gpioWrite(PIN_SEG_B, EVAL_BIT(seg,SEGB));
	gpioWrite(PIN_SEG_C, EVAL_BIT(seg,SEGC));
	gpioWrite(PIN_SEG_D, EVAL_BIT(seg,SEGD));
	gpioWrite(PIN_SEG_E, EVAL_BIT(seg,SEGE));
	gpioWrite(PIN_SEG_F, EVAL_BIT(seg,SEGF));
	gpioWrite(PIN_SEG_G, EVAL_BIT(seg,SEGG));
	gpioWrite(PIN_SEG_DP,EVAL_BIT(seg,SEGDP));
}

/*******************************************************************************
 ******************************************************************************/
