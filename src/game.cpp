#include "game.h"

void display_start_screen(Adafruit_ST7789 tft, char *text) {
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

void draw_select(Adafruit_ST7789 tft, char *text, uint16_t color) {
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

void display_start_game(Adafruit_ST7789 tft) {
    // Fill the screen with black
    tft.fillScreen(ST77XX_BLACK);
}

void draw_tank(Adafruit_ST7789 tft, int position_x, uint16_t colr) {
    
}