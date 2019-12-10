//
// Created by Wycer on 2019/12/10.
//

#ifndef LCD_GRID_H
#define LCD_GRID_H

#include <stdint-gcc.h>
#include "lcd.h"

typedef struct {
    uint16_t sx, sy, ex, ey;
    uint16_t row, row_height;
    uint16_t col, col_width;
    uint16_t line_color, background_color;
} Grid;

void Grid_set_start_point(Grid * const this, uint16_t sx, uint16_t sy);
void Grid_set_end_point(Grid * const this, uint16_t ex, uint16_t ey);

void Grid_set_row(Grid * const this, uint16_t row, uint16_t row_height);
void Grid_set_col(Grid * const this, uint16_t col, uint16_t col_width);

void Grid_set_line_color(Grid * const this, uint16_t color);
void Grid_set_background_color(Grid * const this, uint16_t color);
void Grid_draw(Grid * const this);
#endif //LCD_GRID_H
