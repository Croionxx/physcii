# physcii
2D Physics simulation in terminal

### To Run
```sh
g++ physcii.cpp -o phsycii -lncurses -pthread
```

### Commands to control
```
echo "add <shapeType (C/S)> <Name> <Size> <X_position> <Y_poistion> <X_velocity> <Y_velocity>" > /tmp/physics_fifo
```
