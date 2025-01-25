#include <ncurses.h>
#include <vector>
#include <unistd.h>
#include <sys/ioctl.h>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <fcntl.h>
#include <sys/stat.h>
#include <algorithm>
#include <thread>
#include <memory>

enum ShapeType {
    SQUARE,
    CIRCLE
};

struct Sprite {
    int vel_x;
    int vel_y;
    double pos_x;
    double pos_y;
    int size;
    char name;
    ShapeType type;
};

bool checkCollision(const Sprite& sprite1, const Sprite& sprite2) {
    bool isOverlappingX = sprite1.pos_x < sprite2.pos_x + sprite2.size &&
                          sprite1.pos_x + sprite1.size > sprite2.pos_x;
    bool isOverlappingY = sprite1.pos_y < sprite2.pos_y + sprite2.size &&
                          sprite1.pos_y + sprite1.size > sprite2.pos_y;
    return isOverlappingX && isOverlappingY;
}

void resolveCollision(Sprite& sprite1, Sprite& sprite2) {
    std::swap(sprite1.vel_x, sprite2.vel_x);
    std::swap(sprite1.vel_y, sprite2.vel_y);
}

void updatePosition(Sprite& sprite, std::vector<Sprite>& sprites, int width, int height, double coe, double gravity) {
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

    for (auto& other : sprites) {
        if (&sprite != &other && checkCollision(sprite, other)) {
            resolveCollision(sprite, other);
        }
    }
}

void handleCommands(std::vector<Sprite>& sprites, int& gravity, std::shared_ptr<double> coe, const std::string& fifo_path) {
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
                int x, y, vx, vy, size;
                char shapeType;
                iss >> shapeType >> name >> size >> x >> y >> vx >> vy; 

                Sprite new_sprite = {vx, vy, static_cast<double>(x), static_cast<double>(y), size, name[0], (shapeType == 'C' ? CIRCLE : (shapeType == 'S' ? SQUARE : throw std::invalid_argument("Invalid shape type")))};
                sprites.push_back(new_sprite);
            } else if (command == "remove") {
                std::string name;
                iss >> name;

                sprites.erase(std::remove_if(sprites.begin(), sprites.end(),
                              [&name](const Sprite& sprite) {
                                  return sprite.name == name[0];
                              }),
                              sprites.end());
            } else if (command == "set") {
                std::string param;
                iss >> param;
                if (param == "gravity") {
                    iss >> gravity;
                } else if (param == "coe") {
                    double new_coe;
                    iss >> new_coe;
                    *coe = new_coe;
                }
            }
        }
    }
}

void drawShape(const Sprite& sprite) {
    if (sprite.type == SQUARE) {
        for (int row = 0; row < sprite.size; row++) {
            for (int col = 0; col < sprite.size; col++) {
                mvaddch(static_cast<int>(sprite.pos_y) + row, static_cast<int>(sprite.pos_x) + col, sprite.name);
            }
        }
    } else if (sprite.type == CIRCLE) {
        int radius = sprite.size / 2;
        int center_x = sprite.pos_x + radius;
        int center_y = sprite.pos_y + radius;

        for (int y = -radius; y <= radius; y++) {
            for (int x = -radius; x <= radius; x++) {
                if (x * x + y * y <= radius * radius) {
                    mvaddch(center_y + y, center_x + x, sprite.name);
                }
            }
        }
    }
}

int main() {
    std::string fifo_path = "/tmp/physics_fifo";
    mkfifo(fifo_path.c_str(), 0666);

    int gravity = 1;
    auto coe = std::make_shared<double>(1.0);

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

    std::vector<Sprite> sprites;

    std::thread command_thread(handleCommands, std::ref(sprites), std::ref(gravity), coe, fifo_path);

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
            updatePosition(sprite, sprites, width, height, *coe, gravity);
            drawShape(sprite);
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
