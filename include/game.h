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
#define DEATH 2
#define OVER 3

// Text sizes
#define TEXT_SIZE 3
#define TEXT_HEIGHT 8
#define TEXT_WIDTH 6

// Whole sprites size
#define SCALE 2

// Tank constants
#define PLAY_TANK_Y 280
#define TANK_HEIGHT 9
#define TANK_WIDTH 15
#define LIVES_TOTAL 3

// Alien constants
// Hitbox sizes
#define ALIEN_HEIGHT 8
#define ALIEN_WIDTH  15

// Matrix
#define NR_ALIENS 7
#define NR_ROWS 5

// Movement
#define ALIEN_DROP 5
#define ALIEN_JUMP 2
#define ALIEN_RIGHT 30
#define ALIEN_LEFT 4

// Laser constants
#define LASER_SPEED 10
#define LASER_MISS_LAST 15

// Alien lasers constant
#define NR_ALIEN_LASERS 3

// Death constants

// Menu functions
void display_start_screen(Adafruit_ST7789 &tft, char *text);
void draw_select(Adafruit_ST7789 &tft, char *text, uint16_t color);

// Game display
void display_start_game(Adafruit_ST7789 &tft);
void clear_zone(Adafruit_ST7789 &tft, int position_x, int position_y, int width, int height);

// Tank
void draw_tank_bitmap(Adafruit_ST7789 &tft, int position_x, int position_y, uint16_t color);
int16_t calcluate_tank_position(int16_t old_position, int speed);
void draw_tank_death(Adafruit_ST7789 &tft, int position_x, int position_y, const uint8_t sprite[BUFFER_HEIGHT][BUFFER_WIDTH], uint16_t color);

// Tank laser
void draw_tank_laser(Adafruit_ST7789 &tft, int position_x, int position_y, uint16_t color);
int16_t calcluate_tank_laser_position(int16_t old_position);

// Aliens
void draw_alien_bitmap(Adafruit_ST7789 &tft, int x0, int y0, const uint8_t sprite[BUFFER_HEIGHT][BUFFER_WIDTH], uint16_t color);
void draw_alien_row(Adafruit_ST7789 &tft, int columns[NR_ALIENS], uint16_t start_x,  uint16_t position_y, int row_nr, int frame, uint16_t color);
void draw_alien_explosion(Adafruit_ST7789 &tft,int row,int col,uint16_t aliens_x,uint16_t start_y,uint16_t color);

// Alien laser
int alien_choose_shooting_column(int columns[NR_ALIENS]);
int16_t calcluate_alien_laser_position(int16_t old_position);
void draw_alien_laser(Adafruit_ST7789 &tft, int position_x, int position_y, uint16_t color);

// Lasers
void draw_laser_miss(Adafruit_ST7789 &tft, int x0, int y0, uint16_t color);
int laser_hits_alien(int16_t laser_x, int16_t laser_y, int16_t aliens_x, int16_t aliens_y, int columns[NR_ALIENS]);
bool laser_hits_tank_laser(int16_t alien_laser_x, int16_t alien_laser_y, int16_t tank_laser_x, int16_t tank_laser_y);
bool alien_laser_hits_tank(int16_t laser_x,int16_t laser_y,int16_t tank_x,int16_t tank_y);

#endif