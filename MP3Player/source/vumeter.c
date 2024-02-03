#include "vumeter.h"
#include "DRVLEDDISPLAY.h"

static MATRIX_BOARD_t led_board;

void vu_init()
{
    LED_DISPLAY_init();
    
}