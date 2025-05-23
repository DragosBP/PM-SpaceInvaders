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
    // DDRD |= (1 << PD5);
    // DDRD |= (1 << PD6);

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

}


int main(void) {
    // Initing everything
    init();

    // Some variables declaration
    // To stop it from singing (AAAAAHHHH)
    int continue_singing = 1;

    // For song
    uint32_t last_note = 0;
    uint16_t note_idx = 0;

    // For the game
    int game_state = MENU; 

    // For menu
    int show_select = 1;
    uint32_t last_show = 0;
    uint32_t show_time = 400;
    char start[] = "Start Game";
    
    // For game
    uint32_t last_frame = 0;
    uint32_t frame_time = 1000 / 30;

    // For tank
    uint16_t tank_position = 120 - (TANK_WIDTH / 2) * SCALE;

    // For aliens
    int columns[NR_ALIENS];
    for (int i = 0; i < NR_ALIENS; i++) {
        columns[i] = NR_ROWS;
    }
    int row_nr = NR_ROWS - 1;
    int aliens_pos = 16;
    int aliens_direction = 1;
    int alien_frame_nr = 0;
    int alien_frame_time = 24; // When there are 7 => 5
    int aliens_animation_frame = 0;

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
                int speed = x_input / 150;
                
                // Calculate new position, taking into consideration margins
                uint16_t old_position = tank_position;
                tank_position = calcluate_tank_position(tank_position, speed);
                
                if (tank_position != old_position) {
                    // Draw the new one
                    draw_tank_bitmap(tft, tank_position, PLAY_TANK_Y, ST77XX_GREEN);
                }
                
                // Alien Logic
                if (alien_frame_nr++ == alien_frame_time) {
                    alien_frame_nr = 0;
                    
                    // Calculate the y based on what row it draws
                    int position_y = 5 + row_nr * SCALE * (BUFFER_HEIGHT + 4);
                    
                    // Draw the row
                    draw_alien_row(tft, columns, aliens_pos, position_y, row_nr, aliens_animation_frame, ST77XX_WHITE);

                    // Update the row and check if it needs a reset
                    row_nr--;
                    if (row_nr < 0) {
                        // Update the new position
                        row_nr = NR_ROWS - 1;
                        aliens_pos += aliens_direction * ALIEN_JUMP;

                        // Change the animation frame
                        aliens_animation_frame = aliens_animation_frame == 0 ? 1 : 0;

                        // Change direction at limit
                        if (aliens_pos >= ALIEN_RIGHT) {
                            aliens_direction = -1;
                        }
                        if (aliens_pos <= ALIEN_LEFT) {
                            aliens_direction = 1;
                        }
                    }
                }

                // Debugging
                if (show != 0 && nr++ == show) {
                    nr = 0;
                    printf("Speed: %d\nX_Input: %d\nTank position: %u\n\n", speed, x_input, tank_position);
                }
            }
        }

        int y = -(myAnalogRead(PC0) - 512);
        int x = myAnalogRead(PC1) - 512;
        
        // Button Pressed and Held
        if ((PIND & (1 << PD2))) {
            printf("X: %d\nY: %d\n\n", x, y);
            OCR2A = 120;
        } else {
            OCR2A = 0;
        }
        
        // Just the initial press
        if (button_pressed) {
            printf("Yey, intrerupere\n");
            button_pressed = 0;
            
            if (game_state == MENU) {
                // Sets the game state    
                game_state = GAME;

                // Init the LCD Game
                LCD_clear_top_line();
                LCD_clear_bottom_line();
    
                LCD_printAt(LINE1_ADDR, "Score: ");
                LCD_printAt(LINE2_ADDR, "Lives: 3");
        
                // Init the Screen Game
                display_start_game(tft);

                // Init the Tank
                draw_tank_bitmap(tft, tank_position, PLAY_TANK_Y, ST77XX_GREEN);

                // Init the Aliens
                for (int i = 0; i < NR_ROWS; i++) {
                    int position_y = 5 + i * SCALE * (BUFFER_HEIGHT + 4);
                    
                    draw_alien_row(tft, columns, aliens_pos, position_y, i, aliens_animation_frame, ST77XX_WHITE);
                }

                // Temporary for the song
                continue_singing = 0;
                OCR0A = 0;
            } else {
                // Shooting

            }
        }

        // Sing the song
        if (continue_singing &&  SYSTICKS_PASSED(last_note, durations[note_idx])) {
            last_note = systicks;
            
            note_idx = (note_idx + 1) % num_notes;
            OCR0A = (F_CPU / 256) / surprise_notes[note_idx];
        }
    }

    return 0;
}