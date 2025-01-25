#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>

typedef struct sprite_rectangle{
    int vel_x;
    int vel_y;
    int pos_x;
    int pos_y;
    char name;
}sprite_rectangle;

void create_rectangle( sprite_rectangle *rectangle, int pos_x, int pos_y = 0, char name = 'bob', int vel_x = 0, int vel_y = 0){
    rectangle->vel_x = vel_x;
    rectangle->vel_y = vel_y;
    rectangle->name = name;
    rectangle->pos_x = pos_x;
    rectangle->pos_y = pos_y;
}

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

