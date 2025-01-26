# Sprite Physics Simulation

## Overview

This project simulates physics interactions between sprites (squares and circles) on a terminal interface using `ncurses`. Sprites can be dynamically added, removed, and manipulated using commands sent via a named FIFO pipe.

The simulation supports:

- Collision detection and response.
- Gravity and restitution.
- Random sprite generation.
- Real-time command handling.

## Features

- **Dynamic Sprite Addition**: Add circles or squares with customizable properties.
- **Collision Detection**: Handles collisions between sprites with realistic velocity exchanges.
- **Customizable Parameters**: Adjust gravity and coefficient of restitution.
- **Interactive Commands**: Manage the simulation through a FIFO pipe or provided shell script.
- **Random Sprite Generation**: Generate multiple random sprites.

## Prerequisites

- C++ compiler (e.g., `g++`)
- `ncurses` library
- UNIX-like operating system (for FIFO pipes)

## Compilation and Execution

1. Compile the program:

   ```bash
   g++ physcii.cpp -o physcii -lncurses -pthread
   ```

2. Run the program:

   ```bash
   ./physcii
   ```

3. Open a separate terminal to send commands:

   ```bash
   ./controller.sh
   ```

## Commands

The simulation listens for commands through a FIFO pipe (`/tmp/physics_fifo`). The following commands are supported:

- **Add a sprite**:
  ```
  add <shapeType (C/S)> <Name> <Size> <X_position> <Y_position> <X_velocity> <Y_velocity>
  ```
  Example:
  ```
  add C Circle1 5 10 10 2 -3
  ```
- **Remove a sprite**:
  ```
  remove <Name>
  ```
  Example:
  ```
  remove Circle1
  ```
- **Set gravity**:
  ```
  set gravity <value>
  ```
  Example:
  ```
  set gravity 2
  ```
- **Set coefficient of restitution**:
  ```
  set coe <value>
  ```
  Example:
  ```
  set coe 0.8
  ```
- **Generate random sprites**:
  ```
  random <count>
  ```
  Example:
  ```
  random 10
  ```
- **Clear all sprites**:
  ```
  clear
  ```
- **Exit the command script**:
  ```
  exit
  ```

## Shell Script

The provided `controller.sh` simplifies interaction with the simulation. Ensure it is executable:

```bash
chmod +x controller.sh
```

Run the script:

```bash
./controller.sh
```

## Controls During Simulation

- Press `q` or `Q` to quit the simulation.

## Code Structure

- **physics\_simulation.cpp**: Main simulation logic.
- **command\_script.sh**: Script to send commands to the simulation via FIFO.

