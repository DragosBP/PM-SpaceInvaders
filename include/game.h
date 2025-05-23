#ifndef GAME_H_
#define GAME_H_

#include <Adafruit_ST7789.h>

#include "sprites.h"

// Screen sizes
#define HEIGHT 320
#define WIDTH 240

// Screen displays options
#define MENU 0
#define GAME 1

// Text sizes
#define TEXT_SIZE 3
#define TEXT_HEIGHT 8
#define TEXT_WIDTH 6

// Whole sprites size
#define SCALE 2

// Tank constants
#define PLAY_TANK_Y 290
#define TANK_HEIGHT 9
#define TANK_WIDTH 15

// Alien constants
// Hitbox sizes
#define ALIEN_HEIGHT 8
#define ALIEN_WIDTH  15

// Matrix
#define NR_ALIENS 7
#define NR_ROWS 5

// Movement
#define ALIEN_JUMP 2
#define ALIEN_RIGHT 30
#define ALIEN_LEFT 4

// Menu functions
void display_start_screen(Adafruit_ST7789 &tft, char *text);
void draw_select(Adafruit_ST7789 &tft, char *text, uint16_t color);

// Game display
void display_start_game(Adafruit_ST7789 &tft);
void clear_zone(Adafruit_ST7789 &tft, int position_x, int position_y, int width, int height);

// Tank
void draw_tank_bitmap(Adafruit_ST7789 &tft, int position_x, int position_y, uint16_t color);
int16_t calcluate_tank_position(int16_t old_position, int speed);

// Aliens
void draw_alien_bitmap(Adafruit_ST7789 &tft, int x0, int y0, const uint8_t sprite[BUFFER_HEIGHT][BUFFER_WIDTH], uint16_t color);
void draw_alien_row(Adafruit_ST7789 &tft, int columns[NR_ALIENS], uint16_t start_x,  uint16_t position_y, int row_nr, int frame, uint16_t color);

#endif