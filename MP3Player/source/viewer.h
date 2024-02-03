
#ifndef __VIEWER_H_
#define __VIEWER_H_

#include <stdbool.h>

typedef enum {
	DISPLAY_ID_MENU,
	DISPLAY_BR_MENU,
	DISPLAY_MENU_NUM
}MENU_t;

typedef enum {
	STATUS_OPEN,
	STATUS_CLOSED,
	STATUS_PROCESSING,
	STATUS_NUM
}VIEWER_STATUS_t;

void __viewer_Init_();

void viewer_status_upd(VIEWER_STATUS_t status);

void viewer_digit_upd(char val, int i);
void viewer_digit_sel(int i);
void viewer_clear();

void viewer_censor_set();
void viewer_censor_clr();

void viewer_refresh();
void viewer_mode(MENU_t mode);
void viewer_num_input(bool state);
void viewer_reset();

#endif
