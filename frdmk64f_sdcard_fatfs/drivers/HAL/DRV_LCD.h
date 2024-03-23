/***************************************************************************//**
  @file     +Nombre del archivo (ej: template.h)+
  @brief    +Descripcion del archivo+
  @author   +Nombre del autor (ej: Salvador Allende)+
 ******************************************************************************/

#ifndef _DRV_LCD_H_
#define _DRV_LCD_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "Util.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define CLEAR_DISPLAY				 0b00000001
#define RETURN_HOME					 0b00000010
#define ENTRY_MODE_SET(ID,S)		(0b00000100 | ID<<1 | S)
#define DISPLAY_CONTROL(D,C,B)		(0b00001000 | D<<2 | C<<1 | B)
#define CURSOR_DISPLAY_SHIFT(SC,RL)	(0b00010000 | SC<<3 | RL<<2)
#define FUNCTION_SET(DL,N,F)		(0b00100000 | DL<<4 | N<<3 | F<<2)
#define SET_CGRAM_ADDRESS(addr)		(0b01000000 | addr)
#define SET_DDRAM_ADDRESS(addr)		(0b10000000 | addr)

#define NUM_BITS                    4

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef enum { FOURBITS, EIGHTBITS } lcd_interface_t;   // Only 4 bits is supported by HW
typedef enum { ONELINE, TWOLINES } lcd_lines_t;
typedef enum { FIVEEIGHTDOTS, FIVETENDOTS } lcd_font_t;
typedef enum { DISPLAYOFF, DISPLAYON} lcd_display_t;
typedef enum { CURSOROFF, CURSORON } lcd_cursor_t;
typedef enum { BLINKOFF, BLINKON} lcd_blink_t;
typedef enum { LCD_LEFT, LCD_RIGHT } lcd_shift_dir_t;
typedef enum { SHIFTOFF, SHIFTON } lcd_shift_t;

typedef struct {
    lcd_interface_t interface;
    lcd_lines_t lines;
    lcd_font_t font;
    lcd_display_t display;
    lcd_cursor_t cursor;
    lcd_blink_t blink;
    lcd_shift_dir_t shift_dir;
    lcd_shift_t shift;
} lcd_config_t;

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

void lcd_init();
void lcd_configure(lcd_config_t lcd_config);
bool lcd_is_busy();
void lcd_write_ins(uint8_t value, bool nibble);
void lcd_write_data(uint8_t value, bool nibble);

/*******************************************************************************
 ******************************************************************************/

#endif // _DRV_LCD_H_
