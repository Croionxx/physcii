#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <unistd.h>
#include <fcntl.h>
#include <cmath>
#include <cstdlib>
#include <ncurses.h>
#include <sys/ioctl.h>
#include <sys/stat.h> 
#include <algorithm> 
#include <thread> 

struct sprite_rectangle {
    int vel_x;
    int vel_y;
    double pos_x;
    double pos_y;
    int size;
    char name;
};

void updatePosition(sprite_rectangle& sprite, int width, int height, double coe) {
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

void handleCommands(std::vector<sprite_rectangle>& sprites, int& gravity, const std::string& fifo_path) {
    int fd = open(fifo_path.c_str(), O_RDONLY);
    char buffer[256];

    while (true) {
        int bytesRead = read(fd, buffer, sizeof(buffer) - 1);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            std::istringstream iss(buffer);
            std::string command;
            iss >> command;

            if (command == "add") {
                std::string name;
                int x, y, vx, vy;
                iss >> name >> x >> y >> vx >> vy;

                sprite_rectangle new_sprite = {vx, vy, static_cast<double>(x), static_cast<double>(y), 2, name[0]};
                sprites.push_back(new_sprite);
            } else if (command == "remove") {
                std::string name;
                iss >> name;

                sprites.erase(std::remove_if(sprites.begin(), sprites.end(),
                              [&name](const sprite_rectangle& sprite) {
                                  return sprite.name == name[0];
                              }),
                              sprites.end());
            } else if (command == "set") {
                std::string param;
                iss >> param;
                if (param == "gravity") {
                    iss >> gravity;
                }
            }
        }
    }
}

int main() {
    std::string fifo_path = "/tmp/physics_fifo";
    mkfifo(fifo_path.c_str(), 0666);

    int gravity = 1;
    int coe = 1;

    initscr();
    cbreak();
    noecho();
    curs_set(0);
    nodelay(stdscr, TRUE);

    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    int width = w.ws_col;
    int height = w.ws_row;
    int delayUs = 50000;

    std::vector<sprite_rectangle> sprites;

    std::thread command_thread(handleCommands, std::ref(sprites), std::ref(gravity), std::ref(fifo_path));

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
            sprite.vel_y += gravity;

            updatePosition(sprite, width, height, coe);

            for (int row = 0; row < sprite.size; row++) {
                for (int col = 0; col < sprite.size; col++) {
                    mvaddch(static_cast<int>(sprite.pos_y) + row, static_cast<int>(sprite.pos_x) + col, sprite.name);
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
    unlink(fifo_path.c_str());
    command_thread.detach();
    return 0;
}
