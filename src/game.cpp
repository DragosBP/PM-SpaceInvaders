#include "game.h"

// Menu
void display_start_screen(Adafruit_ST7789 &tft, char *text) {
    int len = strlen(text);
    
    int text_x = (WIDTH - TEXT_WIDTH * TEXT_SIZE * len)  / 2;
    int text_y = HEIGHT / 2 - TEXT_HEIGHT * TEXT_SIZE;

    // Fill the screen with black
    tft.fillScreen(ST77XX_BLACK);

    // Show the "Start game" text
    tft.setCursor(text_x, text_y);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(TEXT_SIZE);
    tft.setTextWrap(true);
    tft.print(text);

}

void draw_select(Adafruit_ST7789 &tft, char *text, uint16_t color) {
    int len = strlen(text);

    int text_x = (WIDTH - TEXT_WIDTH * TEXT_SIZE * len)  / 2;
    int text_y = HEIGHT / 2 - TEXT_HEIGHT * TEXT_SIZE;

    int line_x = text_x;
    int line_y = text_y + TEXT_HEIGHT + 20;
    int line_length = TEXT_WIDTH * TEXT_SIZE * len;

    for (int y = line_y; y < line_y + 5; y++) {
        tft.drawLine(line_x, y, line_x + line_length, y, color);
    }
}

void display_start_game(Adafruit_ST7789 &tft) {
    tft.fillScreen(ST77XX_BLACK);
}

// Game display
void clear_zone(Adafruit_ST7789 &tft, int position_x, int position_y, int width, int height) {
    tft.fillRect(position_x, position_y, width, height, ST77XX_BLACK);
}

static uint16_t buffer[BUFFER_WIDTH * BUFFER_HEIGHT * SCALE * SCALE];

// Tank
void draw_tank_bitmap(Adafruit_ST7789 &tft, int position_x, int position_y, uint16_t color) {
    clear_zone(tft, 0, position_y, position_x, BUFFER_HEIGHT * SCALE);
    clear_zone(tft, position_x + BUFFER_WIDTH * SCALE, position_y, WIDTH - (position_x + BUFFER_WIDTH * SCALE), BUFFER_HEIGHT * SCALE);

    const int sw = BUFFER_WIDTH  * SCALE;
    const int sh = BUFFER_HEIGHT * SCALE;
    uint16_t *buf = buffer;

    for (int row = 0; row < BUFFER_HEIGHT; row++) {
        for (int col = 0; col < BUFFER_WIDTH; col++) {
            bool bit = pgm_read_byte(&tank_sprite[row][col]);
            for (int dy = 0; dy < SCALE; dy++) {
                    for (int dx = 0; dx < SCALE; dx++) {
                    int idx = (row * SCALE + dy) * sw + (col * SCALE + dx);
                    buf[idx] = bit ? color : ST77XX_BLACK;
                }
            }
        }
    }

    tft.drawRGBBitmap(position_x, position_y, buf, sw, sh);
}

int16_t calcluate_tank_position(int16_t old_position, int speed) {
    int16_t tank_position = old_position + speed;
    
    if (tank_position < 1) {
        tank_position = 1;
    }

    if (tank_position > WIDTH - BUFFER_WIDTH * SCALE) {
        tank_position = WIDTH - BUFFER_WIDTH * SCALE;
    }

    return tank_position;
}

void draw_tank_laser(Adafruit_ST7789 &tft, int position_x, int position_y, uint16_t color) {
    const int len = BUFFER_HEIGHT * SCALE / 2;
    
    clear_zone(tft, position_x, position_y + len, SCALE, TANK_LASER_SPEED);
    
    tft.drawRect(position_x, position_y, SCALE, len, color);
}

int16_t calcluate_tank_laser_position(int16_t old_position) {
    int16_t laser_position = old_position - TANK_LASER_SPEED;

    return laser_position;
}

// Aliens

// In your draw call:
void draw_alien_bitmap(Adafruit_ST7789 &tft,
                       int x0, int y0,
                       const uint8_t sprite[BUFFER_HEIGHT][BUFFER_WIDTH],
                       uint16_t fg_color) {
    const int sw = BUFFER_WIDTH  * SCALE;
    const int sh = BUFFER_HEIGHT * SCALE;
    uint16_t *buf = buffer;

    for (int row = 0; row < BUFFER_HEIGHT; row++) {
        for (int col = 0; col < BUFFER_WIDTH; col++) {
            bool bit = pgm_read_byte(&sprite[row][col]);
            for (int dy = 0; dy < SCALE; dy++) {
                    for (int dx = 0; dx < SCALE; dx++) {
                    int idx = (row * SCALE + dy) * sw + (col * SCALE + dx);
                    buf[idx] = bit ? fg_color : ST77XX_BLACK;
                }
            }
        }
    }

    tft.drawRGBBitmap(x0, y0, buf, sw, sh);
}


void draw_alien_row(Adafruit_ST7789 &tft,
                    int columns[NR_ALIENS],
                    uint16_t start_x,
                    uint16_t position_y,
                    int row_nr,
                    int frame,
                    uint16_t color) {
    clear_zone(tft, 0, position_y, start_x, BUFFER_HEIGHT * SCALE);


    const uint8_t (*alien)[BUFFER_WIDTH];
    switch (row_nr) {
        case 0: alien = (frame == 0) ? alien3_frame0 : alien3_frame1; break;
        case 1:
        case 2: alien = (frame == 0) ? alien2_frame0 : alien2_frame1; break;
        default:alien = (frame == 0) ? alien1_frame0 : alien1_frame1; break;
    }

    for (int col = 0; col < NR_ALIENS; col++) {
        if (row_nr < columns[col]) {
            draw_alien_bitmap(tft, start_x, position_y, alien, color);
        } else {
            clear_zone(tft, start_x, position_y, BUFFER_WIDTH * SCALE, BUFFER_HEIGHT * SCALE);
        }
        start_x += BUFFER_WIDTH * SCALE;
    }
}

void draw_alien_explosion(Adafruit_ST7789 &tft,
                          int row,
                          int col,
                          uint16_t aliens_x,
                          uint16_t start_y,
                          uint16_t color)
{
    const uint16_t sprite_w    = ALIEN_WIDTH * SCALE;
    const uint16_t row_spacing = (BUFFER_HEIGHT + 4) * SCALE;

    // Calculate top-left corner of the alien explosion
    uint16_t x = aliens_x + col * sprite_w;
    uint16_t y = start_y + row * row_spacing;

    draw_alien_bitmap(tft, x, y, alien_explosion, color);
}

// Lasers 
void draw_laser_miss(Adafruit_ST7789 &tft, int x0, int y0, uint16_t color) {
    const int sw = BUFFER_WIDTH * SCALE;
    const int sh = BUFFER_HEIGHT * SCALE;
    uint16_t *buf = buffer;  // Reuse the global buffer

    for (int row = 0; row < BUFFER_HEIGHT; row++) {
        for (int col = 0; col < BUFFER_WIDTH; col++) {
            bool bit = pgm_read_byte(&laser_miss[row][col]);
            for (int dy = 0; dy < SCALE; dy++) {
                for (int dx = 0; dx < SCALE; dx++) {
                    int idx = (row * SCALE + dy) * sw + (col * SCALE + dx);
                    buf[idx] = bit ? color : ST77XX_BLACK;
                }
            }
        }
    }

    tft.drawRGBBitmap(x0, y0, buf, sw, sh);
}

int laser_hits_alien(int16_t laser_x,
                      int16_t laser_y,
                      int16_t aliens_x,
                      int16_t start_y,
                      int columns[NR_ALIENS]
                    )
{
    const int16_t aw          = ALIEN_WIDTH * SCALE;
    const int16_t sprite_h    = BUFFER_HEIGHT * SCALE;
    const int16_t row_spacing = (BUFFER_HEIGHT + 4) * SCALE;
    const int16_t lx0         = laser_x;
    const int16_t lx1         = laser_x + SCALE - 1;
    const int16_t ly          = laser_y;

    // trim off these many pixels (pre-scale) on left/right per row
    static const uint8_t left_error[NR_ROWS]  = { 3, 2, 2, 1, 1 };
    static const uint8_t right_error[NR_ROWS] = { 4, 2, 2, 1, 1 };

    for (int row = 0; row < NR_ROWS; row++)
    {
        // full-height vertical bounds for this row
        int16_t ay0 = start_y + row * row_spacing;
        int16_t ay1 = ay0 + sprite_h - 1;
        if (ly < ay0 || ly > ay1) continue;

        for (int col = 0; col < NR_ALIENS; col++)
        {
            if (columns[col] <= row) continue;

            // compute trimmed hit-box in X:
            // start at aliens_x + col*aw + left_error*scale
            int32_t ax0 = (int32_t)aliens_x
                        + col * aw
                        + left_error[row] * SCALE;
            // end at ax0 + full width - 1 - right_error*scale
            int32_t ax1 = ax0
                        + aw - 1
                        - right_error[row] * SCALE;

            if (!(lx1 < ax0 || lx0 > ax1))
                return col;
        }
    }

    return -1;
}
