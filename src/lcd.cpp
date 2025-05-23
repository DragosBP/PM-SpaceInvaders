// i2c_lcd.c
#include <util/delay.h>
#include "twi.h"
#include "lcd.h"

// send one 4-bit nibble + control over I²C
static void lcd_write4(uint8_t nibble, uint8_t ctrl)
{
    twi_start();
    twi_write((LCD_ADDR<<1)|0);               // SLA + W
    twi_write(nibble | ctrl | LCD_BL);        // data + RS + backlight
    // strobe E
    twi_write(nibble | ctrl | LCD_BL | LCD_E);
    twi_write(nibble | ctrl | LCD_BL);
    twi_stop();
    _delay_us(50);
}

void lcd_command(uint8_t cmd)
{
    lcd_write4(cmd & 0xF0,       0);
    lcd_write4((cmd<<4) & 0xF0,  0);
    _delay_ms(2);  // most commands need ~1.5 ms
}

void lcd_data(uint8_t dat)
{
    lcd_write4(dat & 0xF0,       LCD_RS);
    lcd_write4((dat<<4) & 0xF0,  LCD_RS);
    _delay_us(50);
}

void lcd_init(void)
{
    twi_init();
    twi_discover();
    _delay_ms(50);


    // classic 4-bit init
    lcd_write4(0x30, 0); _delay_ms(5);
    lcd_write4(0x30, 0); _delay_us(150);
    lcd_write4(0x20, 0); _delay_us(150);

    lcd_command(0x28);  // 4-bit, 2 lines, 5×8 dots
    lcd_command(0x0C);  // display on, cursor off
    lcd_command(0x01);  // clear
    _delay_ms(2);
    lcd_command(0x06);  // entry mode
}

void LCD_print(const char* msg)
{
    while (*msg) {
        lcd_data((uint8_t)*msg++);
    }
}

void LCD_printAt(uint8_t addr, const char* msg)
{
    // Set DDRAM address: 0x80 | addr
    lcd_command(0x80 | (addr & 0x7F));
    LCD_print(msg);
}

void LCD_clear_top_line(void)
{
    // move to start of line 1
    lcd_command(0x80 | LINE1_ADDR);
    // overwrite with spaces
    for (uint8_t i = 0; i < LCD_COLS; i++) {
        lcd_data(' ');
    }
    // return cursor to start
    lcd_command(0x80 | LINE1_ADDR);
}

void LCD_clear_bottom_line(void)
{
    // move to start of line 2
    lcd_command(0x80 | LINE2_ADDR);
    for (uint8_t i = 0; i < LCD_COLS; i++) {
        lcd_data(' ');
    }
    lcd_command(0x80 | LINE2_ADDR);
}