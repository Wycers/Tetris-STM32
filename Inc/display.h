//
// Created by Wycer on 2019/12/12.
//

#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H

#include <cstring>
#include "grid.h"
#include "box.h"

#define MAX_X 22
#define MAX_Y 13

class Display {
    Grid grid;
    int map[MAX_X][MAX_Y], tmp[MAX_X][MAX_Y];
public:
    Display() {
    }

    void init() {
        memset(map, 0xFF, sizeof map);
        memset(tmp, 0xFF, sizeof tmp);

        grid = Grid(MAX_X, MAX_Y, 12, 12);
        grid.set_start_point(10, 10);
        grid.set_background_color(BLACK);
        grid.set_line_color(BLUE);
        grid.display();
    }

    void clear() {
        memset(tmp, 0xFF, sizeof tmp);
        for (int x = 0; x < MAX_X; ++x)
            tmp[x][0] = tmp[x][MAX_Y - 1] = YELLOW;
        for (int y = 0; y < MAX_Y; ++y)
            tmp[MAX_X - 1][y] = YELLOW;
    }

    int clear_line() {
        int x, y;
        int dx, dy;
        bool fullflag;
        int cnt = 0;
        for (y = 1; y < MAX_Y - 1; y++) {
            fullflag = true;
            for (x = 1; x < MAX_X - 1; x++) {
                if (!map[x][y]) {
                    fullflag = 0;
                    break;
                }
            }
            if (fullflag) {
                for (dy = y; dy > 0; dy--)
                    for (dx = 1; dx < MAX_X - 1; dx++)
                        map[dx][dy] = map[dx][dy - 1];
                for (dx = 1; dx < MAX_X - 1; dx++)
                    map[dx][dy] = 0;
                ++cnt;
            }
        }
        return cnt;
    }

    int test(int x, int y, box &box) {
        commit();
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                if (map[x + i][y + j] != 0xFFFFFFFF && box.get(i, j))
                    return 0;
        return 1;
    }

    void put_box(int x, int y, box &box) {
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                if (box.get(i, j))
                    tmp[x + i][y + j] = box.get(i, j);
    }

    void remove_box(int x, int y, box &box) {
        const uint16_t color = box.get_color();
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                if (box.get(i, j))
                    tmp[x + i][y + j] = -1;
    }

    void draw_box(int x, int y, box &box) {
        const uint16_t color = box.get_color();
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                if (box.get(i, j))
                    draw_point(x + i, y + j, color);
    }

    void draw_point(int x, int y, int color) {
        tmp[x][y] = color;
    }

    void commit() {
        for (int i = 0; i < MAX_X; ++i)
            for (int j = 0; j < MAX_Y; ++j) {
                if (tmp[i][j] == 0)
                    continue;
                if (tmp[i][j] == map[i][j])
                    continue;
                grid.draw(i, j, tmp[i][j]);
                map[i][j] = tmp[i][j];
            }
        memset(tmp, 0, sizeof tmp);
    }

    void refresh() {
        commit();
        for (int i = 0; i < MAX_X; i++)
            for (int j = 0; j < MAX_Y; j++) {
                if (map[i][j] != -1)
                    grid.draw(i, j, YELLOW);
                else
                    grid.draw(i, j, BLACK);
            }
    }
};

#endif //LCD_DISPLAY_H
