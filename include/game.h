#ifndef GAME_H_
#define GAME_H_

#include <Adafruit_ST7789.h>

#define HEIGHT 240
#define WIDTH 320

#define MENU 0
#define GAME 1

#define TEXT_SIZE 3
#define TEXT_HEIGHT 8
#define TEXT_WIDTH 6

// Menu functions
void display_start_screen(Adafruit_ST7789 tft, char *text);
void draw_select(Adafruit_ST7789 tft, char *text, uint16_t color);

void display_start_game(Adafruit_ST7789 tft);
void draw_tank(Adafruit_ST7789 tft, int position_x, uint16_t colr);

#endif