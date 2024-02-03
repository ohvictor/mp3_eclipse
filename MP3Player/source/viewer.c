
#include "viewer.h"
#include "displayBoard.h"

#include <stdint.h>
#include <stdbool.h>

typedef struct {
	char digits[NUM_DIGITS];
	MENU_t mode;
	bool num_input;
	bool censor;
	uint8_t dig_sel;
	VIEWER_STATUS_t output;
}VIEWER_t;

static VIEWER_t viewer;

void __viewer_Init_()
{
	viewer.dig_sel = 5;
	viewer.mode = DISPLAY_ID_MENU;
	viewer.num_input = false;
	viewer.censor = false;

	viewer.output = STATUS_CLOSED;

	viewer.digits[0] = 0;
	viewer.digits[1] = 0;
	viewer.digits[2] = 0;
	viewer.digits[3] = 0;
}

void viewer_reset()
{
	__viewer_Init_();
}

void viewer_censor_set()
{
	viewer.censor = true;
}

void viewer_censor_clr()
{
	viewer.censor = false;
}

void viewer_digit_upd(char val, int i)
{
	viewer.digits[i] = val;
}

void viewer_digit_sel(int i)
{
	viewer.dig_sel = i;
}

void viewer_status_upd(VIEWER_STATUS_t status)
{
	viewer.output = status;
}

void viewer_mode(MENU_t mode)
{
	viewer.mode = mode;
}

void viewer_num_input(bool state)
{
	viewer.num_input = state;
}

void viewer_clear()
{
	int i;
	for(i=0; i<NUM_DIGITS; i++)
	{
		viewer.digits[i] = 0;
	}
}

void viewer_refresh()
{
	if(!viewer.num_input)
	{
		switch(viewer.mode)
		{
		case DISPLAY_ID_MENU:
			digit_write(symi, 0, false);
			digit_write(symd, 1, false);
			digit_write(symi, 2, false);
			digit_write(symn, 3, false);
			break;
		case DISPLAY_BR_MENU:
			digit_write(symb, 0, false);
			digit_write(symr, 1, false);
			digit_write(symi, 2, false);
			digit_write(symClr, 3, false);
			break;
		default:
			break;
		}
	}
	else
	{
		int i;
		bool dp;
		sym_type data;
		for(i=0; i<NUM_DIGITS; i++)
		{
			dp = false;
			switch (viewer.digits[i])
			{
			case '0':data = sym0; break;
			case '1':data = sym1; break;
			case '2':data = sym2; break;
			case '3':data = sym3; break;
			case '4':data = sym4; break;
			case '5':data = sym5; break;
			case '6':data = sym6; break;
			case '7':data = sym7; break;
			case '8':data = sym8; break;
			case '9':data = sym9; break;
			default: data = symClr; break;
			}

			if(viewer.dig_sel == i)
			{
				dp = true;
			}

			if((viewer.censor) && (i != viewer.dig_sel))
			{
				data = symDash;
			}

			digit_write(data, i, dp);
		}
	}

	switch(viewer.output)
	{
	case STATUS_OPEN: status_led_write(status0); break;
	case STATUS_CLOSED: status_led_write(status2); break;
	case STATUS_PROCESSING: status_led_write(status1); break;
	default: break;
	}
}
