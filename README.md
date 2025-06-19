# Intelligent Ecosystem Simulation

A C++ simulation that models complex predator-prey dynamics using **state-driven intelligent agents** in a 2D world.

## Overview

This project simulates a dynamic ecosystem where each creature is an autonomous agent governed by a set of attributes and an AI state machine. Unlike simple rule-based systems, animals in this simulation perceive their environment and make decisions based on their current state—whether they are wandering, chasing prey, fleeing from a threat, or hunting in a pack.

This attribute-driven design (Health, Damage, Speed, Sight) and state-based AI lead to complex and unpredictable **emergent behaviors**, creating a more realistic and engaging simulation of population dynamics.

## Features

- **Intelligent Agent-Based AI:** Animals aren't just moving randomly; they perceive their surroundings and react with purpose.
- **Finite State Machine (FSM):** Each animal operates in a specific state (`WANDERING`, `FLEEING`, `CHASING`, `PACK_HUNTING`), dictating its actions.
- **Attribute-Driven System:** Every animal has unique stats for **Health, Damage, Speed, and Sight**, creating deep and tunable species differentiation.
- **Complex AI Behaviors:**
    - **Fleeing:** Herbivores and outnumbered animals will actively run from threats.
    - **Chasing:** Predators will lock onto and pursue prey.
    - **Pack Hunting:** Omnivores can form groups to take down stronger Carnivores, and Carnivores will flee from these packs.
- **Real-time ASCII Visualization:** The world is rendered in the console, with live-updating population counts.
- **Extensible Object-Oriented Design:** The class structure makes it easy to add new species or modify existing behaviors.

## Project Structure

```
ecosystem-simulation/
├── main.cpp           # Entry point, simulation loop, and parameters
├── World.h/.cpp       # Manages the grid, all animals, and the update cycle
├── Animal.h/.cpp      # Abstract base class with core attributes and AI state
├── Herbivore.h/.cpp   # Herbivore implementation (fleeing behavior)
├── Carnivore.h/.cpp   # Carnivore implementation (chasing/fleeing behavior)
├── Omnivore.h/.cpp    # Omnivore implementation (pack hunting behavior)
├── Makefile           # Build configuration
├── LICENSE            # MIT License
└── README.md          # This file
```

## Building and Running

### Prerequisites
- A C++17 compatible compiler (e.g., g++, clang++)
- The `make` utility (recommended)

### Build with Make
```bash
# Clean previous builds and compile
make clean && make
```

### Build Manually
```bash
g++ -std=c++17 -Wall -Wextra -g -o simulation main.cpp World.cpp Animal.cpp Herbivore.cpp Carnivore.cpp Omnivore.cpp
```

### Run the Simulation
```bash
./simulation
```

## Core Mechanics & AI Behavior

The old energy-based system has been replaced by a more robust attribute and AI system.

### Core Attributes
- **Health (HP):** An animal's life force. It dies when HP reaches 0.
- **Damage:** The amount of HP an animal inflicts when it attacks.
- **Sight Radius:** The distance an animal can "see" to detect prey or predators.
- **Speed:** The number of tiles an animal can move per turn.
- **Energy:** No longer tied to life. It is now used as "stamina" for reproduction and passive survival cost.

### Species-Specific AI

- **Herbivores (H):**
    - **Defensive:** Possess a large sight radius to detect danger early.
    - **Behavior:** Their primary AI state is `FLEEING` from any predator that enters their sight. Otherwise, they are `WANDERING` and grazing (gaining energy).

- **Carnivores (C):**
    - **Apex Predators:** High damage and speed make them lethal hunters.
    - **Behavior:** They will `CHASE` any Herbivore they see. However, they are smart enough to recognize a threat and will `FLEE` from an Omnivore pack.

- **Omnivores (O):**
    - **Opportunists:** The most complex AI. Their behavior depends on the situation.
    - **Behavior:** They will `CHASE` lone Herbivores. If they spot a Carnivore, they will assess the situation: if enough allied Omnivores are nearby, they will switch to `PACK_HUNTING`; if outnumbered, they will `FLEE`.

## Configuration & Balancing

Initial population counts and world size can be modified in `main.cpp`:
```cpp
const int WORLD_WIDTH = 50;
const int WORLD_HEIGHT = 25;
const int INITIAL_HERBIVORES = 40;
const int INITIAL_OMNIVORES = 15;
const int INITIAL_CARNIVORES = 5;
```
The core balance of the ecosystem is determined by the attribute constants defined at the top of each animal's implementation file (e.g., `Carnivore.cpp`, `Herbivore.cpp`). Tweaking these values will have a dramatic impact on the simulation's outcome.

## Class Hierarchy

The simulation uses a clear polymorphic hierarchy.
```
Animal (abstract base class)
 |
 +-- Herbivore
 +-- Carnivore
 +-- Omnivore
```
- The `Animal` base class provides the attribute framework, the `AIState` enum, and the movement functions.
- Derived classes implement the decision-making logic in `updateAI()` and execute actions in `act()`.

## Termination Conditions

The simulation ends when:
1. One of the species is completely wiped out, causing the ecosystem to collapse.
2. The simulation reaches the maximum turn limit (2000 by default).

## Example Output

```
--- Intelligent Agent Simulation ---
. . . . . . . . . . H . . . . . . . . . . . . . . . . .
. . . . . H . . . . . . . . C . . . . . . O . . . . . .
. . . . . . . . . . . . . . . . . . . . . . . . . . . .
. . H . . . . . . . . . . . . . . . . . . . . . . . . .
. . . . . . . O . . . . . . . . . O . . . . H . . . . .
------------------------------------
Turn: 124 | Herbivores (H): 32 | Carnivores (C): 4 | Omnivores (O): 11
```

## License

This project is open source and available under the MIT License. See the `LICENSE` file for details.

Copyright (c) 2025 Samarth Shrivastava