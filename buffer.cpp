#include <ncurses.h>
#include <vector>
#include <unistd.h> 
#include <sys/ioctl.h>
#include <cmath>
#include <cstdlib>

struct sprite_rectangle {
    int vel_x;
    int vel_y;
    double pos_x;
    double pos_y;
    int size;
    char name;
};

void updatePosition(sprite_rectangle& sprite, int width, int height, double coe, double gravity) {
    sprite.vel_y += gravity;

    sprite.pos_x += sprite.vel_x;
    sprite.pos_y += sprite.vel_y;

    if (sprite.pos_x <= 1 || sprite.pos_x + sprite.size >= width - 1) {
        sprite.vel_x = -coe * sprite.vel_x; 
        sprite.pos_x = fmax(1, fmin(sprite.pos_x, width - sprite.size - 1));
    }
    if (sprite.pos_y <= 1 || sprite.pos_y + sprite.size >= height - 1) {
        sprite.vel_y = -coe * sprite.vel_y; 
        sprite.pos_y = fmax(1, fmin(sprite.pos_y, height - sprite.size - 1));
    }
}

int main() {
    double coe = 1;       // coefficient of restitution
    double gravity = 1.0;   // gravity

    initscr();
    cbreak();
    noecho();
    curs_set(0);
    nodelay(stdscr, TRUE);

    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    int width  = w.ws_col;
    int height = w.ws_row;
    int delayUs = 50000; 

    std::vector<sprite_rectangle> sprites;

    // Create sprites
    for (int i = 0; i < 100; i++) {
        sprite_rectangle sprite;
        sprite.pos_x = rand() % (width - 6) + 1;
        sprite.pos_y = rand() % (height - 6) + 1;
        sprite.vel_x = (rand() % 3 + 1) * (rand() % 2 == 0 ? 1 : -1);
        sprite.vel_y = (rand() % 3 + 1) * (rand() % 2 == 0 ? 1 : -1);
        sprite.size = 2;
        sprite.name = '#';
        sprites.push_back(sprite);
    }

    while (true) {
        clear();

        for (int x = 0; x < width; x++) {
            mvaddch(0, x, '-');
            mvaddch(height - 1, x, '-');
        }
        for (int y = 0; y < height; y++) {
            mvaddch(y, 0, '|');
            mvaddch(y, width - 1, '|');
        }

        for (auto& sprite : sprites) {
            updatePosition(sprite, width, height, coe, gravity);

            for (int row = 0; row < sprite.size; row++) {
                for (int col = 0; col < sprite.size; col++) {
                    mvaddch(int(sprite.pos_y) + row, int(sprite.pos_x) + col, sprite.name);
                }
            }
        }

        refresh();
        usleep(delayUs);

        int ch = getch();
        if (ch == 'q' || ch == 'Q') {
            break;
        }
    }

    endwin();
    return 0;
}
