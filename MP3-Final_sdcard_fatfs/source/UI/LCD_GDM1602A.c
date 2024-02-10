 /***************************************************************************
  @file     LCD_GDM1602A.c
  @brief    LCD controller
  @author   Grupo 5
 ******************************************************************************/

/*************************************************************
 * 		INCLUDES
 *************************************************************/
#include <stdbool.h>
#include <string.h>

#include "SPI_wrapper.h"
#include "timer.h"
#include "LCD_GDM1602A.h"
#include "fsl_device_registers.h"

/**************************************************************
 * 		CONSTANTS AND MACROS
 **************************************************************/

#define BLANCK_SPACE			(0x20										)
#define CLEAR					(1											)
#define RET_HOME				(1<<1										)
#define ENTRY_MODE(i_d, sh)		(1<<2 | (i_d)<< 1 	   | (sh)				)
#define DISPLAY(d,c,b)			(1<<3 | (d)  << 2	   | (c)  <<1 | (b)		)
#define SHIFT(s_c, r_l)			(1<<4 | (s_c)<< 3 	   | (r_l)<<2			)
#define FUNCTIONS(dl, n, f)		(1<<5 | (dl) << 4 	   | (n)  <<3 | (f)<<2	)
#define SET_DDRAM(add)			(1<<7 | ((add) & 0x7F)						)

#define ENABLE(x)  				(((x) & 1)<<1								)
#define RS(x)					( (x) & 1									)
#define VAL(x)					(((x) & 0xF)<<2								)
// RS -> Qa, E -> Qb, Data -> Qc a Qf
#define BLOCK(e, rs, v) 		(ENABLE(e) | RS(rs) | VAL(v)				)

#define INIT_INSTRUCTIONS		10

#define SHIFTING_BUFFER_LEN		(25*DISPLAY_COLUMNS)

#define CURSOR_VISIBLE			0
#define CURSOR_BLINK			0

#define FAST_SPEED				400

/*********************************************************
 * 		LOCAL STRUCTS AND ENUMS
 ********************************************************/

typedef enum{
	NOTHING,
	SHIFTING,
	BOUNCING
}lcd_line_state_t;

typedef struct{
	char buffer[SHIFTING_BUFFER_LEN];
	uint8_t pointer;
	uint8_t timer_id;
	uint8_t length;
	uint8_t begin;
	bool direction;
	lcd_line_state_t state;
	lcd_shift_speed_t speed;
}lcd_line_t;

/****************************************************************
 * 		FUNCTION DECLARATION WITH FILE SCOPE
 ******************************************************************/

static void sendBlock(uint8_t byte, uint8_t rs);
static void initRoutineTimerCallback(void);
static void initRoutineSPICallback(void);

static void LCD_writeInstruction(uint8_t instruct);
static void shifttingCallback(void);

/*****************************************************************
 * 		VARIABLES WITH FILE LEVEL SCOPE
 *****************************************************************/

static uint8_t 	init_status = 0;
static uint8_t 	timer_id;
static uint8_t 	init_fase;

static uint8_t init_instructions[INIT_INSTRUCTIONS] = {FUNCTIONS(1,0,0),FUNCTIONS(1,0,0),FUNCTIONS(1,0,0),
									  	  	  	  	   FUNCTIONS(0,0,0),FUNCTIONS(0,1,0),DISPLAY(0,0,0),
													   CLEAR, ENTRY_MODE(1, 0),DISPLAY(1,CURSOR_VISIBLE,CURSOR_BLINK),
													   RET_HOME};
static uint8_t init_delays[INIT_INSTRUCTIONS] = {5,1,1,1,1,1,2,1,1,2};

static bool send_status = true; // Free to send something

static lcd_line_t lcdLines[DISPLAY_ROWS];

/**********************************************************************
 * 		FUNCTION WITH GOBAL SCOPE
 **********************************************************************/

void LCD_Init(void)
{
	if(init_status == 0)
	{
		SPI_Init(SPI_0_ID, SPI_SLAVE_0, 300000U);
		//SysTick_Init();
		init_fase = 0;

		timer_id = timerGetId();

		timerStart(timer_id, 15, TIM_MODE_PERIODIC, initRoutineTimerCallback);

		for(int i = 0; i < DISPLAY_ROWS; i++)
		{
			lcdLines[i].length = 0;
			lcdLines[i].pointer = 0;
			lcdLines[i].state = NOTHING;
			lcdLines[i].timer_id = timerGetId();
			timerStart(lcdLines[i].timer_id, FAST_SPEED, TIM_MODE_PERIODIC, shifttingCallback);
			timerStop(lcdLines[i].timer_id);
		}
		init_status = 1;

	}
}

void LCD_UpdateClock(void)
{
	SPI_Config(SPI_0_ID, SPI_SLAVE_0, 300000U);
}

bool LCD_isInit(void)
{
	return (init_status == 2);
}

void LCD_writeData(uint8_t data)
{
	sendBlock(data, 1);
}

bool LCD_setCursor(uint8_t row, uint8_t column)
{
	if(row < DISPLAY_ROWS && column < DISPLAY_COLUMNS)
	{
		uint8_t address = column + row * 0x40;
		LCD_writeInstruction(SET_DDRAM(address));
		return true;
	}
	return false;
}

void LCD_writeDataInPos(uint8_t data, uint8_t row, uint8_t column)
{

	if(LCD_setCursor(row, column))
	{
		LCD_writeData(data);
	}
}

void LCD_writeStrInPos(char * str, uint8_t len, uint8_t row, uint8_t column)
{
	uint16_t i, max = ((column + len ) < DISPLAY_COLUMNS)? column + len : DISPLAY_COLUMNS ;
	if(LCD_setCursor(row, column))
	{
		for(i = 0; i<max; i++)
		{
			LCD_writeData(str[i]);
		}
	}
}

void LCD_clearRow(uint8_t row)
{
	uint8_t i;
	memset(lcdLines[row].buffer, 0x20, SHIFTING_BUFFER_LEN);
	if(LCD_setCursor(row, 0))
	{
		for(i = 0; i<DISPLAY_COLUMNS; i++)
		{
			LCD_writeData(BLANCK_SPACE);
		}
		LCD_setCursor(row, 0); // back to the beginning of the row
		lcdLines[row].state = NOTHING;
	}
}

void LCD_stopMove(uint8_t row)
{
	lcdLines[row].state = NOTHING;
}

void LCD_clearDisplay(void)
{
	// Not using the display build in function to have better timing control
	// Slightly slower
	uint8_t i;
	memset(lcdLines[0].buffer, 0x20, SHIFTING_BUFFER_LEN);
	memset(lcdLines[1].buffer, 0x20, SHIFTING_BUFFER_LEN);
	LCD_setCursor(0, 0);
	for(i = 0; i<DISPLAY_COLUMNS; i++)
	{
		LCD_writeData(BLANCK_SPACE);
	}
	lcdLines[0].state = NOTHING;
	LCD_setCursor(1, 0);
	for(i = 0; i<DISPLAY_COLUMNS; i++)
	{
		LCD_writeData(BLANCK_SPACE);
	}
	LCD_setCursor(0, 0); // back to the beginning of the display
	lcdLines[1].state = NOTHING;
}

void LCD_writeShiftingStr(char * str, uint8_t len, uint8_t row, lcd_shift_speed_t speed)
{
	uint8_t i;
	if(row < DISPLAY_ROWS)
	{
		memset(lcdLines[row].buffer, 0x20, SHIFTING_BUFFER_LEN);
		for(i = 0; i < len; i++)
		{
			lcdLines[row].buffer[i] = str[i];
		}

		timerStart(lcdLines[row].timer_id, (3-speed)*FAST_SPEED, TIM_MODE_PERIODIC, shifttingCallback);
		timerStop(lcdLines[row].timer_id);
		lcdLines[row].speed = speed;

		if(lcdLines[row].state == NOTHING)
		{
			timerStart(lcdLines[row].timer_id, (3-speed)*FAST_SPEED, TIM_MODE_PERIODIC, shifttingCallback);
		}

		lcdLines[row].state = SHIFTING;
		lcdLines[row].length = len;
		lcdLines[row].pointer = 0;
		lcdLines[row].begin = 0;
	}
}

/****************************************************************
 * 		FUNCTIONS WITH FILE LEVEL SCOPE
 ***************************************************************/

static void LCD_writeInstruction(uint8_t instruct)
{
	sendBlock(instruct, 0);
}

static void sendBlock(uint8_t byte, uint8_t rs)
{
	char msg[6] = {0U};

	// MSB first
	msg[0] = BLOCK(0, rs, (byte & 0xF0) >> 4);
	msg[1] = BLOCK(1, rs, (byte & 0xF0) >> 4);
	msg[2] = BLOCK(0, rs, (byte & 0xF0) >> 4);

	msg[3] = BLOCK(0, rs, byte & 0xF);
	msg[4] = BLOCK(1, rs, byte & 0xF);
	msg[5] = BLOCK(0, rs, byte & 0xF);

	SPI_Send(SPI_0_ID, SPI_SLAVE_0, msg, 6, (init_status == 2)? NULL:initRoutineSPICallback);
	send_status = false;
}

static void sendNybble(uint8_t nybble, uint8_t rs)
{
	char msg[6] = {0U};

	// MSB first
	msg[0] = BLOCK(0, rs, (nybble & 0xF0)>>4);
	msg[1] = BLOCK(1, rs, (nybble & 0xF0)>>4);
	msg[2] = BLOCK(0, rs, (nybble & 0xF0)>>4);

	SPI_Send(SPI_0_ID, SPI_SLAVE_0, msg, 3, (init_status == 2)? NULL:initRoutineSPICallback);
	send_status = false;
}

static void initRoutineTimerCallback(void)
{
	timerStop(timer_id);

	if (init_fase < 4)
	{
		timerStart(timer_id, init_delays[init_fase], TIM_MODE_PERIODIC, initRoutineTimerCallback);
		sendNybble(init_instructions[init_fase], 0);
	}
	else if(init_fase < INIT_INSTRUCTIONS)
	{
		timerStart(timer_id, init_delays[init_fase], TIM_MODE_PERIODIC, initRoutineTimerCallback);
		LCD_writeInstruction(init_instructions[init_fase]);
	}
	else
	{
		timerStop(timer_id);
		init_status = 2;
	}
}

static void initRoutineSPICallback(void)
{
	send_status = true;
	timerStart(timer_id, 15, TIM_MODE_PERIODIC, initRoutineTimerCallback);
	init_fase++;
}


static void shifttingCallback(void)
{

	char buffer[DISPLAY_COLUMNS];
	lcd_line_state_t curr;
	for(int i = 0; i < DISPLAY_ROWS; i++)
	{
		if(!timerGetStatus(lcdLines[i].timer_id))
		{
			continue;
		}
		curr = lcdLines[i].state;
		if(curr == NOTHING)
		{
			timerStop(lcdLines[i].timer_id);
		}
		else
		{
			uint8_t start = lcdLines[i].begin;
			for(int j = 0; j<(DISPLAY_COLUMNS-start); j++)
			{
				buffer[j] = lcdLines[i].buffer[(lcdLines[i].pointer + j)%lcdLines[i].length];
			}

			lcdLines[i].pointer = (lcdLines[i].pointer + 1)%SHIFTING_BUFFER_LEN;

			LCD_writeStrInPos(buffer, DISPLAY_COLUMNS-start, i, start);
		}
	}
}
