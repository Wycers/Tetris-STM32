//
// Created by Wycer on 2019/12/11.
//

#ifndef LCD_BOX_H
#define LCD_BOX_H

#include <initializer_list>
#include <cstdint>

class box {
    bool mat[4][4];
    uint16_t color;

public:
    box() {}

    box(box *b);

    box(std::initializer_list<std::initializer_list<bool>> lists, uint16_t color) {
        int x = 0;
        for (std::initializer_list<bool> list : lists) {
            for (const bool &i : list) {
                mat[x / 4][x % 4] = i;
                ++x;
            }
        }
        this->color = color;
    }

    bool get(int x, int y);
    uint16_t get_color();
};

#endif //LCD_BOX_H
