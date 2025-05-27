#include <stdint.h>

#include "sound.h"

/* Tones (Hz) */
#define c    261
#define d    294
#define e    329
#define f    349
#define g    391
#define gS   415
#define a    440
#define aS   455
#define b    466
#define cH   523
#define cSH  554
#define dH   587
#define dSH  622
#define eH   659
#define fH   698
#define fSH  740
#define gH   784
#define gSH  830
#define aH   880

#define BREAK   0
#define STEP1   65
#define STEP2   98
#define STEP3   131
#define STEP4   196

// Melody
uint16_t melody_notes[] = {STEP1, BREAK, STEP2, BREAK, STEP3, BREAK, STEP4, BREAK};
uint16_t melody_durations[] = {300, 560, 300, 560, 300, 560, 300, 560};
uint8_t melody_num_notes = sizeof(melody_durations) / sizeof(melody_durations[0]);

// Alien Hit
uint16_t alien_hit_notes[] = {gH, fH, eH};
uint16_t alien_hit_durations[] = {250, 250, 250};
uint8_t alien_hit_num_notes = sizeof(alien_hit_durations) / sizeof(alien_hit_durations[0]);

// Death
uint16_t death_notes[] = {a,  f,  cH, BREAK};
uint16_t death_durations[] = {1000, 1000, 1000, 1000};
uint8_t death_num_notes = sizeof(death_durations) / sizeof(death_durations[0]);



