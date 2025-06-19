# Ecosystem Simulation

A C++ ecosystem simulation that models complex predator-prey dynamics between herbivores, carnivores, and omnivores in a 2D world.

## Overview

This project simulates a complex ecosystem where herbivores feed on grass, carnivores hunt other animals, and omnivores can both graze and hunt. The simulation demonstrates emergent behaviors and population dynamics through simple rules governing movement, feeding, reproduction, and death.

## Features

- **Real-time ASCII visualization** of the ecosystem
- **Dynamic population tracking** of herbivores (H), carnivores (C), and omnivores (O)
- **Autonomous animal behavior** with movement, feeding, and reproduction
- **Energy-based survival system**
- **Age-based mortality**
- **Three distinct species with unique behaviors**
- **Configurable simulation parameters**

## Project Structure

```
ecosystem-simulation/
├── main.cpp           # Entry point and simulation loop
├── World.h/.cpp       # World management and rendering
├── Animal.h/.cpp      # Base animal class with common behaviors
├── Herbivore.h/.cpp   # Herbivore-specific behaviors
├── Carnivore.h/.cpp   # Carnivore-specific behaviors
├── Omnivore.h/.cpp    # Omnivore-specific behaviors
├── Makefile           # Build configuration
├── LICENSE            # MIT License
└── README.md          # This file
```

## Building and Running

### Prerequisites
- C++17 compatible compiler (g++, clang++)
- Make (optional, for using Makefile)

### Build with Make
```bash
make
```

### Build manually
```bash
g++ -std=c++17 -Wall -Wextra -g -o simulation main.cpp World.cpp Animal.cpp Herbivore.cpp Carnivore.cpp Omnivore.cpp
```

### Run the simulation
```bash
./simulation
```

## Simulation Rules

### Herbivores (H)
- Start with 10 energy points
- Gain 1 energy per turn from "grazing"
- Reproduce when energy reaches 15 (costs energy)
- Die of old age at 50 turns or when energy reaches 0
- Move randomly around the world

### Carnivores (C)
- Hunt herbivores and omnivores for energy
- Must catch prey to survive
- Reproduce when they have sufficient energy
- Die of old age or starvation
- Move randomly but hunt nearby prey

### Omnivores (O)
- Can both graze like herbivores and hunt like carnivores
- Flexible feeding strategy provides survival advantages
- Reproduce when they have sufficient energy
- Die of old age or starvation
- Move randomly and opportunistically feed

### World Environment
- 40x20 grid by default
- Wrapping boundaries (animals can move off edges)
- Empty spaces represent "grass" for herbivores and omnivores
- Real-time display updates every 200ms

## Configuration

Key simulation parameters can be modified in [`main.cpp`](main.cpp):

```cpp
const int WORLD_WIDTH = 40;          // World width
const int WORLD_HEIGHT = 20;         // World height  
const int INITIAL_HERBIVORES = 50;   // Starting herbivore population
const int INITIAL_CARNIVORES = 20;   // Starting carnivore population
const int INITIAL_OMNIVORES = 15;    // Starting omnivore population
const int SIMULATION_SPEED_MS = 200; // Update interval in milliseconds
```

Animal behavior constants are defined in their respective implementation files:
- [`Herbivore.cpp`](Herbivore.cpp) - herbivore parameters
- [`Carnivore.cpp`](Carnivore.cpp) - carnivore parameters
- [`Omnivore.cpp`](Omnivore.cpp) - omnivore parameters

## Class Hierarchy

```
Animal (abstract base class)
├── Herbivore
├── Carnivore
└── Omnivore
```

The [`Animal`](Animal.h) base class provides:
- Position and movement
- Energy management
- Age tracking
- Life/death state

Derived classes implement specific behaviors:
- `update()` - turn-based behavior
- `reproduce()` - species-specific reproduction

## Termination Conditions

The simulation ends when:
1. All species go extinct
2. Maximum turns reached (500 by default)

## Example Output

```
Population: H=45 C=18 O=12
........H.......C...................
....H...............................
..................H.................O
........................................
.....H..............C...............H..
........................................
```

## Development

### Adding New Features
1. Modify animal behaviors in respective class files
2. Add new parameters to simulation constants
3. Extend the [`World`](World.h) class for environmental features
4. Update the main loop for new simulation rules

### Clean Build
```bash
make clean
make
```

## License

This project is open source and available under the MIT License. See [LICENSE](LICENSE) for details.

Copyright (c) 2025 Samarth Shrivastava