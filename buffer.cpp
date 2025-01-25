#include <ncurses.h>
#include <vector>
#include <utility>
#include <unistd.h> 
#include <sys/ioctl.h>
#include <cmath>

struct Vector2D {
    double x;
    double y;
};

Vector2D calculateNewPosition(const Vector2D& initialPosition, 
                              const Vector2D& initialVelocity, 
                              const Vector2D& acceleration, 
                              double time) {
    Vector2D newPosition;
    newPosition.x = initialPosition.x + initialVelocity.x * time + 
                    0.5 * acceleration.x * time * time;
    newPosition.y = initialPosition.y + initialVelocity.y * time + 
                    0.5 * acceleration.y * time * time;
    return newPosition;
}

int main() {
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    nodelay(stdscr, TRUE);  

    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    int width  = w.ws_col;
    int height = w.ws_row;

    int squareSize = 1;
    int delayUs = 10000;  // Reduced delay for smoother animation

    Vector2D position = {10.0, 10.0};
    Vector2D velocity = {5.0, 3.0};
    Vector2D acceleration = {0.1, -0.1};
    double timeStep = 0.1;

    while (true) {
        clear();

        // Draw border
        for (int x = 0; x < width; x++) {
            mvaddch(0, x, '-');
            mvaddch(height - 1, x, '-');
        }
        for (int y = 0; y < height; y++) {
            mvaddch(y, 0, '|');
            mvaddch(y, width - 1, '|');
        }

        // Calculate new position
        position = calculateNewPosition(position, velocity, acceleration, timeStep);

        // Ensure the square stays within the screen bounds
        position.x = fmax(1, fmin(position.x, width - squareSize - 1));
        position.y = fmax(1, fmin(position.y, height - squareSize - 1));

        // Draw square
        for (int row = 0; row < squareSize; row++) {
            for (int col = 0; col < squareSize; col++) {
                mvaddch(int(position.y) + row, int(position.x) + col, '#');
            }
        }

        refresh();
        usleep(delayUs);

        // Check for 'q' key press
        int ch = getch();
        if (ch == 'q' || ch == 'Q') {
            break;
        }
    }

    endwin();
    return 0;
}
