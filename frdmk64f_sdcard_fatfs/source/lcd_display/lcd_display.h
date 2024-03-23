// lcd_display.h

#ifndef HIGH_LEVEL_LCD_H
#define HIGH_LEVEL_LCD_H

#include <stdint.h>
#include <stdbool.h>

void lcd_display_init();
void lcd_display_config();
void lcd_display_clear();
void lcd_display_loop();
void lcd_display_set_string(const char *str);
void lcd_display_set_cursor(uint8_t row, uint8_t col);

#endif // HIGH_LEVEL_LCD_H
