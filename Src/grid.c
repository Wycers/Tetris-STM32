//
// Created by Wycer on 2019/12/10.
//

#include "grid.h"

void Grid_set_line_color(Grid *const this, uint16_t color) {
    this->line_color = color;
}

void Grid_set_background_color(Grid *const this, uint16_t color) {
    this->background_color = color;
}

void Grid_set_start_point(Grid *const this, uint16_t sx, uint16_t sy) {
    this->sx = sx;
    this->sy = sy;
}

void Grid_set_end_point(Grid *const this, uint16_t ex, uint16_t ey) {
    this->ex = ex;
    this->ey = ey;
}

void Grid_draw(Grid *const this) {
    LCD_Fill(this->sx, this->sy, this->ex, this->ey, this->background_color);
    POINT_COLOR = this->line_color;

    for (int i = 0, t = 0; i <= this->col; i++) {
        LCD_DrawLine(this->sx + t, this->sy, this->sx + t, this->ey);
        t = t + 12;
    }
    for (int i = 0, t = 0; i <= this->row; i++) {
        LCD_DrawLine(this->sx, this->sy + t, this->ex, this->sy + t);
        t = t + 12;
    }
}

void Grid_set_col(Grid *const this, uint16_t col, uint16_t col_width) {
    this->col = col;
    this->col_width = col_width;
    this->ex = this->sx + col * col_width;
}

void Grid_set_row(Grid *const this, uint16_t row, uint16_t row_height) {
    this->row = row;
    this->row_height = row_height;
    this->ey = this->sy + row * row_height;
}

