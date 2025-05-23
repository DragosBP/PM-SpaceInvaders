#ifndef SPRITES_H_
#define SPRITES_H_

#include <stdint.h>
#include <avr/pgmspace.h>

#define BUFFER_HEIGHT 9
#define BUFFER_WIDTH  15

// Aliens (1 = pixel, 0 = transparent)
extern const uint8_t alien1_frame0[BUFFER_HEIGHT][BUFFER_WIDTH] PROGMEM;
extern const uint8_t alien1_frame1[BUFFER_HEIGHT][BUFFER_WIDTH] PROGMEM;
extern const uint8_t alien2_frame0[BUFFER_HEIGHT][BUFFER_WIDTH] PROGMEM;
extern const uint8_t alien2_frame1[BUFFER_HEIGHT][BUFFER_WIDTH] PROGMEM;
extern const uint8_t alien3_frame0[BUFFER_HEIGHT][BUFFER_WIDTH] PROGMEM;
extern const uint8_t alien3_frame1[BUFFER_HEIGHT][BUFFER_WIDTH] PROGMEM;

// Tank
extern const uint8_t tank_sprite[BUFFER_HEIGHT][BUFFER_WIDTH] PROGMEM;

#endif // SPRITES_H_
