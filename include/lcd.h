#ifndef LCD_H_
#define LCD_H_

#include <stdint.h>

// adjust to your module’s address
#define LCD_ADDR   0x27  

// PCF8574 pin assignments
#define LCD_RS     0x01  // P0
#define LCD_RW     0x02  // P1  (always 0)
#define LCD_E      0x04  // P2
#define LCD_BL     0x08  // P3

#define LCD_COLS   16
#define LINE1_ADDR 0x00
#define LINE2_ADDR 0x40

void lcd_init(void);
void lcd_command(uint8_t cmd);
void lcd_data   (uint8_t dat);

// new helpers:
void LCD_print(const char* msg);
void LCD_printAt(uint8_t addr, const char* msg);
void LCD_clear_top_line(void);
void LCD_clear_bottom_line(void);

#endif