//
// Created by Wycer on 2019/12/10.
//

#ifndef LCD_GRID_H
#define LCD_GRID_H

#include <stdint-gcc.h>
#include "lcd.h"

class Grid {
private:
    uint16_t sx, sy, ex, ey;
    uint16_t row, row_height;
    uint16_t col, col_width;
    uint16_t line_color, background_color;
public:
    void set_start_point(uint16_t sx, uint16_t sy);

    void set_end_point(uint16_t ex, uint16_t ey);

    void set_row(uint16_t row, uint16_t row_height);

    void set_col(uint16_t col, uint16_t col_width);

    void draw(uint16_t x, uint16_t y, uint16_t color);

    void set_line_color(uint16_t color);

    void set_background_color(uint16_t color);

    void display();
};

#endif //LCD_GRID_H
