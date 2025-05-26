#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

#include "usart.h"
#include "adc.h"
#include "timers.h"
#include "sound.h"
#include "twi.h"
#include "lcd.h"

#include "game.h"

#define PM_BAUD 9600

// Hardware SPI pins on UNO
#define TFT_CS   10
#define TFT_DC    9

// Reset tied to 3.3 V, so we pass -1. 
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, -1);

volatile int button_pressed = 0;

ISR (INT0_vect) {
    button_pressed = 1;
}

void init() {
    // Iniit intreruperi pentru buton
    cli();
    
    EICRA |= (1 << ISC00) | (1 << ISC01);    
    EIMSK |= (1 << INT0);
    
    sei();
    
    // Init GPIO
    // Set PB5 (Led) as output
    DDRB |= (1 << PB5);
    
    // Set PD2 (Button) as input 
    DDRD &= ~(1 << PD2);   // Set PD2 as input
    
    // Set PD5 (Buzzer) as output
    DDRD |= (1 << PD5);
    DDRD |= (1 << PD6);

    // Init Usart
    USART0_init(CALC_USART_UBRR(PM_BAUD));
    USART0_use_stdio();
    printf("Initialised USART!\n");
    
    // Init Timers
    Timer0_init_ctc();
    Timer1_init_systicks();
    Timer2_init_ctc();
    printf("Initialised Timers!\n");
    
    // Init the ADC
    adc_init();
    printf("Initialised ADC!\n");
    
    // Init SPI
    SPI.begin();
    SPI.setClockDivider(SPI_CLOCK_DIV16);
    printf("Initialised SPI!\n");
    
    // Init display
    tft.init(240, 320);
    tft.invertDisplay(false);
    printf("Initialised Display!\n");
    
    // Initialize I2C
    lcd_init();
    printf("Initialised LCD!\n");

    // Init random for the alines
    srand(ADC);
}


int main(void) {
    // Initing everything
    init();

    // Some variables declaration
    // To stop it from singing (AAAAAHHHH)
    int continue_singing = 0;

    // For song
    uint32_t last_note = 0;
    uint16_t note_idx = 0;
    uint16_t melody_break_speed = 0;

    // For the game
    int game_state = MENU; 
    int shot_first_bullet = false; 

    // For menu
    int show_select = 1;
    uint32_t last_show = 0;
    uint32_t show_time = 800;
    char start[] = "Start Game";
    
    // For game
    uint32_t last_frame = 0;
    uint32_t frame_time = 1000 / 30;
    uint32_t score = 0;
    char score_buffer[20];

    // Tank
    // Body
    uint16_t tank_position = 120 - (TANK_WIDTH / 2) * SCALE;
    
    // Aliens
    // Data to store about the,
    int nr_aliens = NR_ALIENS * NR_ROWS;
    int rows_deleted = 1;
    int columns[NR_ALIENS];
    for (int i = 0; i < NR_ALIENS; i++) {
        columns[i] = NR_ROWS;
    }
    int row_nr = NR_ROWS - rows_deleted;

    // Their y position
    int start_y = 40;
    int cleaned_y = 0;
    
    // Their x position
    int alien_right = ALIEN_RIGHT;
    int alien_left = ALIEN_LEFT;
    int aliens_pos = 16;
    int aliens_direction = 1;
    
    // Movement speed
    int alien_frame_nr = 0;
    int alien_frame_time = 24; // 24 on start, decrement 2 on every 3 aliens dead
    
    // Animation frame
    int aliens_animation_frame = 0;
    
    // Lasers
    // Their speed
    int laser_frame_time = 1;

    // Tank Laser 
    bool present_tank_laser = false;
    uint16_t tank_laser_x = 0;
    uint16_t tank_laser_y = 0;
    int tank_laser_frame_nr = 0;
    
    // Tank Laser Miss
    int remove_tank_laser_miss = 0;
    uint16_t tank_laser_miss_x = 0;
    uint16_t tank_laser_miss_y = 0;

    // Tank Laser Hit
    int column_hit;
    int alien_hit_x;
    int alien_hit_y;
    bool continue_alien_hit_sing;
    uint32_t alien_hit_last_note = 0;
    uint16_t alien_hit_note_idx = 0;

    // Alien Lasers
    bool alien_lasers_present[NR_LASERS] = {false, false, false};
    
    // ALien Lasers Generating
    int alien_lasers_generate_current = 0;
    int alien_lasers_generate_time = 30;
    int alien_lasers_generate_nr = 0;

    // Alien Lasers Position
    uint16_t alien_laser_x[NR_LASERS] = {0, 0, 0};
    uint16_t alien_laser_y[NR_LASERS] = {0, 0, 0};
    int alien_laser_frame_nr = 0;

    // Alien Lasers Miss
    int remove_alien_lasers_miss[NR_LASERS] = {0, 0, 0};
    uint16_t alien_lasers_miss_x[NR_LASERS] = {0, 0, 0};
    uint16_t alien_laser_miss_y[NR_LASERS] = {0, 0, 0};

    // Alien Lasers Hit
    bool alien_lasers_hit = false;

    // LCD Sreen
    LCD_printAt(LINE1_ADDR, "Press the Button");
    LCD_printAt(LINE2_ADDR, "Start the Game!");
    

    printf("Started program!\n");

    // Debug
    int nr = 0;
    int show = 0;

    display_start_screen(tft, start);
    while (1) {
        // For when it overflows
        if (systicks < last_show) {
            last_show = systicks;
            last_frame = systicks;
            last_note = systicks;
            alien_hit_last_note = systicks;
        }
        
        if (game_state == MENU) {
            if (SYSTICKS_PASSED(last_show, show_time)) {
                last_show = systicks;
                
                uint16_t color = show_select ? ST77XX_WHITE : ST77XX_BLACK;
                draw_select(tft, start, color);
                
                show_select = show_select ? 0 : 1;
            }
        } else {
            if (SYSTICKS_PASSED(last_frame, frame_time)) {
                last_frame = systicks;
            
                // Tank Logic
                int x_input = myAnalogRead(PC1) - 512;
                int speed = x_input / 180;
                
                // Calculate new position, taking into consideration margins
                uint16_t old_position = tank_position;
                tank_position = calcluate_tank_position(tank_position, speed);
                
                if (tank_position != old_position) {
                    // Draw the new one
                    draw_tank_bitmap(tft, tank_position, PLAY_TANK_Y, ST77XX_GREEN);
                }
                
                // Alien Logic
                if (alien_frame_nr++ >= alien_frame_time && !continue_alien_hit_sing) {
                    alien_frame_nr = 0;
                    
                    // Calculate the y based on what row it draws
                    int position_y = start_y + row_nr * SCALE * (BUFFER_HEIGHT + 4);
                    
                    // Draw the row
                    draw_alien_row(tft, columns, aliens_pos, position_y, row_nr, aliens_animation_frame, ST77XX_WHITE);
                    if (cleaned_y) {
                        cleaned_y--;
                        clear_zone(tft, 0, position_y - ALIEN_DROP, WIDTH, ALIEN_DROP);
                    }

                    // Update the row and check if it needs a reset
                    row_nr--;
                    if (row_nr < 0) {
                        // Change direction at limit
                        if (aliens_pos >= alien_right) {
                            start_y += ALIEN_DROP;
                            aliens_direction = -1;
                            cleaned_y = NR_ROWS;

                        }
                        if (aliens_pos <= alien_left) {
                            start_y += ALIEN_DROP;
                            aliens_direction = 1;
                            cleaned_y = NR_ROWS;
                        }
                        
                        // Update the new position
                        aliens_pos += aliens_direction * ALIEN_JUMP;
                        row_nr = NR_ROWS - rows_deleted;

                        // Change the animation frame
                        aliens_animation_frame = aliens_animation_frame == 0 ? 1 : 0;
                    }
                }

                // Laser Logic
                // Tank
                if (present_tank_laser && tank_laser_frame_nr++ >= laser_frame_time) {
                    // Reset frame counter
                    tank_laser_frame_nr = 0;

                    // Calculate new position
                    tank_laser_y = calcluate_tank_laser_position(tank_laser_y);

                    // printf("Laser_position: %d\n", tank_laser_y);
                    column_hit = laser_hits_alien(tank_laser_x, tank_laser_y, aliens_pos, start_y, columns);
                    if (column_hit != -1) {
                        // Stop laser sound and start alien hit sound
                        OCR2A = 0;
                        continue_alien_hit_sing = true;

                        // Update the score
                        int add_score;
                        switch (columns[column_hit])
                        {
                        case 1:
                            add_score = 30;
                            break;
                        case 2:
                        case 3:
                            add_score = 20;
                            break;
                        case 4:
                        case 5:
                        default:
                            add_score = 10;
                            break;
                        }
                        score += add_score;

                        // Show the new sore
                        LCD_clear_top_line();
                        sprintf(score_buffer, "Score: %lu", score);
                        LCD_printAt(LINE1_ADDR, score_buffer);

                        // Put explosion
                        draw_alien_explosion(tft, columns[column_hit] - 1, column_hit, aliens_pos, start_y, ST77XX_RED);

                        // Save the position of the explosion
                        alien_hit_x = aliens_pos + column_hit * BUFFER_WIDTH * SCALE;
                        alien_hit_y = start_y + (columns[column_hit] - 1) * (BUFFER_HEIGHT + 4) * SCALE;

                        // Eliminnate the alien
                        columns[column_hit]--;
                        nr_aliens--;
                        
                        // Check nr of rows remaining
                        int max = -1;
                        int found = 0;
                        int left_free = 0;
                        int right_free;
                        for (int i = 0; i < NR_ALIENS; i++) {
                            if (columns[i] > max)
                                max = columns[i];
                            
                            if (columns[i] != 0) {
                                found = 1;
                                right_free = 0;
                            } else {
                                right_free++;
                            }

                            if (!found) {
                                left_free++;
                            }
                        }
                        rows_deleted = 6 - max;
                        alien_left = ALIEN_LEFT - (left_free * BUFFER_WIDTH * SCALE);
                        alien_right = ALIEN_RIGHT + (right_free * BUFFER_WIDTH * SCALE);

                        // Change the speed
                        if ((nr_aliens + 1) % 3 == 0) {
                            alien_frame_time -= 2;
                            melody_break_speed += 50;
                        }

                        // Reset the laser
                        present_tank_laser = false;
                        draw_tank_laser(tft, tank_laser_x, tank_laser_y, ST77XX_BLACK);
                    } else {
                        if (tank_laser_y < 10) {
                            // Hit the ceiling - delete the laser
                            present_tank_laser = false;
                            
                            // Tank Miss
                            remove_tank_laser_miss = LASER_MISS_LAST;
                            tank_laser_miss_x = tank_laser_x - BUFFER_WIDTH / 2 * SCALE;
                            tank_laser_miss_y = tank_laser_y;
    
                            // Draw the laser miss + delete the old one
                            draw_tank_laser(tft, tank_laser_x, tank_laser_y, ST77XX_BLACK);
                            draw_laser_miss(tft, tank_laser_miss_x, tank_laser_miss_y, ST77XX_RED);
                        } else {
                            draw_tank_laser(tft, tank_laser_x, tank_laser_y, ST77XX_GREEN);
                        }
                    }

                }

                // Tank miss
                if (remove_tank_laser_miss-- == 1) {
                    OCR2A = 0;
                    clear_zone(tft, tank_laser_miss_x, tank_laser_miss_y, BUFFER_WIDTH * SCALE, BUFFER_HEIGHT * SCALE);
                }

                // Debugging
                if (show != 0 && nr++ == show) {
                    nr = 0;
                    printf("Speed: %d\nX_Input: %d\nTank position: %u\n\n", speed, x_input, tank_position);
                }
            }
        }

        // int y = -(myAnalogRead(PC0) - 512);
        // int x = myAnalogRead(PC1) - 512;
        
        // Just the initial press
        if (button_pressed) {
            button_pressed = 0;
            
            if (game_state == MENU) {
                // Sets the game state    
                game_state = GAME;

                // Init the LCD Game
                LCD_clear_top_line();
                LCD_clear_bottom_line();
    
                sprintf(score_buffer, "Score: %lu", score);

                LCD_printAt(LINE1_ADDR, score_buffer);
                LCD_printAt(LINE2_ADDR, "Lives: 3");
        
                // Init the Screen Game
                display_start_game(tft);

                // Init the Tank
                draw_tank_bitmap(tft, tank_position, PLAY_TANK_Y, ST77XX_GREEN);

                // Init the Aliens
                for (int i = 0; i < NR_ROWS; i++) {
                    int position_y = start_y + i * SCALE * (BUFFER_HEIGHT + 4);
                    
                    draw_alien_row(tft, columns, aliens_pos, position_y, i, aliens_animation_frame, ST77XX_WHITE);
                }

                // Temporary for the song
                continue_singing = 1;
            } else {
                // Because it shoots a bullet in advance
                if (!shot_first_bullet) {
                    shot_first_bullet = true;
                    continue;
                }
                
                // Shoot only if there is no other laser from the tank and there is no explosion
                if (!present_tank_laser && !continue_alien_hit_sing) {
                    // Shooting
                    present_tank_laser = true;
    
                    OCR2A = (F_CPU / 256) / LASER_TONE;

                    // Start cordonates
                    tank_laser_x = tank_position + BUFFER_WIDTH / 2 * SCALE;
                    tank_laser_y = PLAY_TANK_Y - BUFFER_HEIGHT * SCALE / 2;
                    
                    tank_laser_frame_nr = laser_frame_time; // To start moving from the beggining
                }
            }
        }

        // Sing the song
        uint16_t melody_time = melody_durations[note_idx] - (melody_notes[note_idx] == 0 ? melody_break_speed : 0);
        if (continue_singing && SYSTICKS_PASSED(last_note, melody_time)) {
            last_note = systicks;
            
            note_idx = (note_idx + 1) % melody_num_notes;
            if (melody_notes[note_idx] == 0) {
                OCR0A = 0;
            } else {
                OCR0A = (F_CPU / 256) / melody_notes[note_idx];
            }
        }

        // Alien hit sound
        if (continue_singing && continue_alien_hit_sing && SYSTICKS_PASSED(alien_hit_last_note, alien_hit_durations[alien_hit_note_idx])) {
            alien_hit_last_note = systicks;

            if (alien_hit_note_idx++ < alien_hit_num_notes) {
                OCR2A = (F_CPU / 256) / alien_hit_notes[alien_hit_note_idx];
            } else {
                OCR2A = present_tank_laser ? LASER_TONE : 0; // Continue any othe sound
                continue_alien_hit_sing = false; // Deactivate the singing
                alien_hit_note_idx = 0; // Reset the notes
                clear_zone(tft, alien_hit_x, alien_hit_y, BUFFER_WIDTH * SCALE, BUFFER_HEIGHT * SCALE); // Clear the zone (best part of the code yo do this)
            }
        }
    }

    return 0;
}