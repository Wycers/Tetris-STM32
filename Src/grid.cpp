//
// Created by Wycer on 2019/12/10.
//

#include "grid.h"

void Grid::set_line_color(uint16_t color) {
    this->line_color = color;
}

void Grid::set_background_color(uint16_t color) {
    this->background_color = color;
}

void Grid::set_start_point(uint16_t sx, uint16_t sy) {
    this->sx = sx;
    this->sy = sy;
    this->ex = this->sx + col * col_width;
    this->ey = this->sy + row * row_height;
}

void Grid::set_end_point(uint16_t ex, uint16_t ey) {
    this->ex = ex;
    this->ey = ey;
}

void Grid::display() {
    LCD_Fill(this->sx, this->sy, this->ex, this->ey, this->background_color);
    POINT_COLOR = this->line_color;

    for (int i = 0, t = 0; i <= this->col; i++, t += this->col_width) {
        LCD_DrawLine(this->sx + t, this->sy, this->sx + t, this->ey);
    }
    for (int i = 0, t = 0; i <= this->row; i++, t += this->row_height) {
        LCD_DrawLine(this->sx, this->sy + t, this->ex, this->sy + t);
    }
}

void Grid::set_col(uint16_t col, uint16_t col_width) {
    this->col = col;
    this->col_width = col_width;
    this->ex = this->sx + col * col_width;
}

void Grid::set_row(uint16_t row, uint16_t row_height) {
    this->row = row;
    this->row_height = row_height;
    this->ey = this->sy + row * row_height;
}

void Grid::draw(uint16_t x, uint16_t y, uint16_t color) {
    LCD_Fill(this->sx + y * col_width + 1, this->sy + x * this->row_height + 1,
             this->sx + (y + 1) * col_width - 1, this->sy + (x + 1) * row_height - 1, color);
}

