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

// Function to create a grid and check for collisions
bool checkCollision(const std::vector<sprite_rectangle>& sprites, int grid_size, int grid_width, int grid_height) {
    std::vector<std::vector<std::vector<int> > > grid(grid_height, std::vector<std::vector<int> >(grid_width));

    for (std::size_t i = 0; i < sprites.size(); ++i) {
        int grid_x = sprites[i].pos_x / grid_size;
        int grid_y = sprites[i].pos_y / grid_size;

        for (int row = 0; row < sprites[i].size; row++) {
            for (int col = 0; col < sprites[i].size; col++) {
                int grid_cell_x = (sprites[i].pos_x + col) / grid_size;
                int grid_cell_y = (sprites[i].pos_y + row) / grid_size;

                if (grid_cell_x >= 0 && grid_cell_x < grid_width && grid_cell_y >= 0 && grid_cell_y < grid_height) {
                    if (!grid[grid_cell_y][grid_cell_x].empty()) {
                        return true;
                    }
                    grid[grid_cell_y][grid_cell_x].push_back(i);
                }
            }
        }
    }
    return false; 
}


void calculateCollision(sprite_rectangle& sprite1, sprite_rectangle& sprite2) {
    int temp_vel_x = sprite1.vel_x;
    int temp_vel_y = sprite1.vel_y;

    sprite1.vel_x = sprite2.vel_x;
    sprite1.vel_y = sprite2.vel_y;

    sprite2.vel_x = temp_vel_x;
    sprite2.vel_y = temp_vel_y;
}

void updatePosition(sprite_rectangle& sprite, std::vector<sprite_rectangle>& sprites, int width, int height, double coe, double gravity) {
    sprite.vel_y += gravity;

    sprite.pos_x += sprite.vel_x;
    sprite.pos_y += sprite.vel_y;

    // Handle collisions with screen boundaries
    if (sprite.pos_x <= 1 || sprite.pos_x + sprite.size >= width - 1) {
        sprite.vel_x = -coe * sprite.vel_x; 
        sprite.pos_x = fmax(1, fmin(sprite.pos_x, width - sprite.size - 1));
    }
    if (sprite.pos_y <= 1 || sprite.pos_y + sprite.size >= height - 1) {
        sprite.vel_y = -coe * sprite.vel_y; 
        sprite.pos_y = fmax(1, fmin(sprite.pos_y, height - sprite.size - 1));
    }

    // Check for collisions with other sprites
    for (std::size_t i = 0; i < sprites.size(); ++i) {
        sprite_rectangle& other = sprites[i];
        if (&sprite != &other) { // Don't check collision with itself
            bool isOverlappingX = (sprite.pos_x < other.pos_x + other.size && sprite.pos_x + sprite.size > other.pos_x);
            bool isOverlappingY = (sprite.pos_y < other.pos_y + other.size && sprite.pos_y + sprite.size > other.pos_y);

            if (isOverlappingX && isOverlappingY) {
                calculateCollision(sprite, other);
            }
        }
    }
}

int main() {
    double coe = 1;       // Coefficient of restitution
    double gravity = 0;   // Gravity factor

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
    for (int i = 0; i < 10; i++) {
        sprite_rectangle sprite;
        sprite.pos_x = rand() % (width - 6) + 1;
        sprite.pos_y = rand() % (height - 6) + 1;
        sprite.vel_x = (rand() % 3 + 1) * (rand() % 2 == 0 ? 1 : -1);
        sprite.vel_y = (rand() % 3 + 1) * (rand() % 2 == 0 ? 1 : -1);
        sprite.size = 3;
        sprite.name = '#';
        sprites.push_back(sprite);
    }

    int grid_size = 2; // Size of the grid equal to smallest sprite size
    int grid_width = width / grid_size;
    int grid_height = height / grid_size;

    while (true) {
        clear();

        // Draw screen boundaries
        for (int x = 0; x < width; x++) {
            mvaddch(0, x, '-');
            mvaddch(height - 1, x, '-');
        }
        for (int y = 0; y < height; y++) {
            mvaddch(y, 0, '|');
            mvaddch(y, width - 1, '|');
        }

        // Update and draw all sprites
        for (std::size_t i = 0; i < sprites.size(); ++i) {
            sprite_rectangle& sprite = sprites[i];
            updatePosition(sprite, sprites, width, height, coe, gravity);

            for (int row = 0; row < sprite.size; row++) {
                for (int col = 0; col < sprite.size; col++) {
                    mvaddch(int(sprite.pos_y) + row, int(sprite.pos_x) + col, sprite.name);
                }
            }
        }

        // Check for collisions using grid
        if (checkCollision(sprites, grid_size, grid_width, grid_height)) {
            mvprintw(0, 0, "Collision detected!");
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
