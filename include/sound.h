#ifndef SOUND_H_
#define SOUND_H_

#include <stdint.h>


// Laser
#define LASER_TONE      784

// Melody
extern uint16_t melody_notes[]; // The nodes in our melody
extern uint16_t melody_durations[]; // The durations (in ms) of every note in the melody above
extern uint8_t melody_num_notes; // Total number of notes in melody

// Alien hit
extern uint16_t alien_hit_notes[]; // The notes when you hit an alien
extern uint16_t alien_hit_durations[]; // The durations of every nothe in the sounds above
extern uint8_t alien_hit_num_notes; // Total number of notes in melody

// Death
extern uint16_t death_notes[];
extern uint16_t death_durations[];
extern uint8_t death_num_notes;
#endif
