//
// Created by Wycer on 2019/12/11.
//

#include "box.h"

bool box::get(int x, int y) {
    return mat[x][y];
}

uint16_t box::get_color() {
    return color;
}

box::box(box *b) {
    for (int x = 0; x < 4; x++)
        for (int y = 3; y >= 0; y--)
            mat[x][y] = b->get(y, 3 - x);
    color = b->get_color();
}
