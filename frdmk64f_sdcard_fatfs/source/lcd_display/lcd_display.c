// high_level_lcd.c

#include "lcd_display.h"
#include "DRV_LCD.h"

static uint8_t print_counter = 0;
static bool update = false;
static bool clear = false;
static bool return_home = false;
static const char * buff;

void lcd_display_clear_loop();

void lcd_display_init() {
    lcd_init();
}

void lcd_display_config(){
	lcd_config_t lcd_config = {FOURBITS, TWOLINES, FIVEEIGHTDOTS, DISPLAYON, CURSOROFF, BLINKOFF, LCD_RIGHT, SHIFTOFF};
	lcd_configure(lcd_config);
}

void lcd_display_clear_loop() {
	if(clear && !lcd_is_busy()){
		lcd_write_ins(CLEAR_DISPLAY, false);
		clear = false;
		return_home = true;
	}
	if(return_home && !lcd_is_busy()){
		lcd_write_ins(RETURN_HOME, false);
		return_home = false;
	}
    // You may want to add a delay here to ensure the clear display command is completed
}

void lcd_display_set_string(const char *str){
	buff = str;
	update = true;
}

void lcd_display_clear(){
	clear = true;
}

void lcd_display_loop() {
	lcd_display_clear_loop();
	if(update && !lcd_is_busy()){
		if (buff[print_counter] != '\0') {
			if(print_counter == 15){
				lcd_display_set_cursor(1, 0);
			}
			if(buff[print_counter] == '/' || buff[print_counter] == 'O' || buff[print_counter] == 'o' || buff[print_counter] == '?' || buff[print_counter] == '_'){
				lcd_write_data(' ', false);
			}
			else{
				lcd_write_data(buff[print_counter], false);
			}
			print_counter++;
		}
		else{
			update = false;
			print_counter = 0;
			lcd_display_set_cursor(0, 0);
		}
	}
}

void lcd_display_set_cursor(uint8_t row, uint8_t col) {
    uint8_t addr = col + (row == 1 ? 0x40 : 0);
    lcd_write_ins(SET_DDRAM_ADDRESS(addr), false);
    while(lcd_is_busy()){};
    // You may want to add a delay here to ensure the set cursor command is completed
}
