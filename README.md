# Intelligent Ecosystem Simulation

A C++ simulation that models complex predator-prey dynamics using **state-driven intelligent agents** in a 2D world, featuring advanced survival mechanics like **health regeneration** and a **hunger-driven desperation mode**.

## Overview

This project simulates a dynamic ecosystem where each creature is an autonomous agent governed by a set of attributes and an AI state machine. Unlike simple rule-based systems, animals in this simulation perceive their environment and make decisions based on their current state—whether they are wandering, chasing prey, fleeing from a threat, or hunting in a pack.

The simulation is further deepened by:
-   A **Health Regeneration** system, allowing animals to recover from non-critical wounds over time, but with lasting consequences for severe injuries.
-   A **Hunger System** where starving animals enter a "desperation mode," temporarily boosting their combat and sensory capabilities at the cost of their health, creating dramatic high-stakes encounters.

This combination of attribute-driven design (Health, Damage, Speed, Sight, Energy) and state-based AI leads to complex and unpredictable **emergent behaviors**, creating a more realistic and engaging simulation of population dynamics and individual survival.

## Features

-   **Intelligent Agent-Based AI:** Animals perceive their surroundings and react with purpose using a Finite State Machine (FSM).
-   **Core Animal States:** `WANDERING`, `FLEEING`, `CHASING`, `PACK_HUNTING`.
-   **Rich Attribute System:** Every animal has unique stats for **Health, Max Health, Base/Current Damage, Base/Current Sight, Base/Current Speed, Energy, and Max Energy**.
-   **Advanced Survival Mechanics:**
    -   **Health Regeneration:** Animals regenerate health if they avoid damage for several turns. Severe injuries (dropping below 75% HP) can permanently lower their maximum healable health, simulating lasting scars.
    -   **Hunger & Desperation Mode:** Low energy triggers a "desperation mode." Animals progressively lose health but gain temporary, significant boosts to their damage, speed, and sight, increasing their chances of a crucial hunt while on the brink.
-   **Complex AI Behaviors:**
    -   **Fleeing:** Herbivores and outnumbered animals actively run from threats.
    -   **Chasing:** Predators lock onto and pursue prey, their effectiveness influenced by hunger.
    -   **Pack Hunting:** Omnivores can form groups to take down stronger Carnivores; Carnivores will flee from these packs.
-   **Real-time ASCII Visualization:** The world is rendered in the console, with live-updating population counts.
-   **Extensible Object-Oriented Design:** The class structure makes it easy to add new species or modify existing behaviors.

## Project Structure

```
ecosystem-simulation/
├── main.cpp           # Entry point, simulation loop, and parameters
├── World.h/.cpp       # Manages the grid, all animals, and the update cycle
├── Animal.h/.cpp      # Abstract base class with core attributes, AI state, and survival mechanics
├── Herbivore.h/.cpp   # Herbivore implementation (fleeing, grazing, reproduction logic)
├── Carnivore.h/.cpp   # Carnivore implementation (chasing, fleeing, hunting logic)
├── Omnivore.h/.cpp    # Omnivore implementation (pack hunting, grazing, hunting logic)
├── Makefile           # Build configuration
├── LICENSE            # MIT License
└── README.md          # This file
```

## Building and Running

### Prerequisites
-   A C++17 compatible compiler (e.g., g++, clang++)
-   The `make` utility (recommended)

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

### Core Attributes
-   **Health (HP):** An animal's life force. Dies when HP reaches 0.
-   **Max Health:** The animal's maximum possible health.
-   **Base Stats (Damage, Sight, Speed):** The animal's natural, unaltered attributes.
-   **Current Stats (Damage, Sight, Speed):** These can be temporarily boosted by the hunger system.
-   **Energy:** Represents hunger and stamina. Used for survival, actions, and reproduction. Triggers desperation mode when low.
-   **Max Energy:** The maximum energy an animal can store.

### Species-Specific AI & Survival

-   **Herbivores (H):**
    -   **Behavior:** Prioritize `FLEEING` from predators. If safe, they `WANDER` and graze to replenish energy. Reproduction is tuned for sustainable population growth.
    -   **Survival:** Benefit from health regeneration. The hunger system applies, but their primary "desperation" is to find a safe place to graze.

-   **Carnivores (C):**
    -   **Behavior:** `CHASE` Herbivores. They `FLEE` from Omnivore packs.
    -   **Survival:** Health regeneration is crucial after tough fights. The hunger system makes them exceptionally dangerous when starving, boosting their hunt success odds.

-   **Omnivores (O):**
    -   **Behavior:** The most versatile. They `CHASE` Herbivores. They engage in `PACK_HUNTING` against Carnivores if they have numbers, otherwise, they `FLEE` from lone Carnivores. They also graze if no hunting opportunities are present.
    -   **Survival:** Benefit from both health regeneration and the hunger system, making them resilient and adaptable threats.

## Configuration & Balancing

Initial population counts and world size can be modified in `main.cpp`:
```cpp
const int WORLD_WIDTH = 50;
const int WORLD_HEIGHT = 25;
const int INITIAL_HERBIVORES = 40; // Tuned for better sustainability
const int INITIAL_OMNIVORES = 15;
const int INITIAL_CARNIVORES = 5;
```
The core balance of the ecosystem is determined by the attribute constants (HP, damage, energy, reproduction thresholds, etc.) defined at the top of each animal's implementation file (e.g., `Carnivore.cpp`, `Herbivore.cpp`). Tweaking these values will have a dramatic impact on the simulation's outcome.

## Class Hierarchy

The simulation uses a clear polymorphic hierarchy:
```
Animal (abstract base class)
 |
 +-- Herbivore
 +-- Carnivore
 +-- Omnivore
```
-   The `Animal` base class provides the attribute framework, the `AIState` enum, movement functions, and the core logic for **Health Regeneration** and the **Hunger System** in its `postTurnUpdate()` method.
-   Derived classes implement the species-specific decision-making logic in `updateAI()` and execute actions in `act()`.

## Termination Conditions

The simulation ends when:
1.  One of the species is completely wiped out, causing the ecosystem to collapse.
2.  The simulation reaches the maximum turn limit (default: 2000).

## Example Output

```
--- Intelligent Agent Simulation ---
. . . . . . . . . . H . . . . . . . . . . . . . . . . .
. . . . . H . . . . . . . . C . . . . . . O . . . . . .  (C might be faster/stronger if hungry!)
. . . . . . . . . . . . . . . . . . . . . . . . . . . .
. . H . . . . . . . . . . . . . . . . . . . . . . . . .  (H might be healing from a past encounter)
. . . . . . . O . . . . . . . . . O . . . . H . . . . .
------------------------------------
Turn: 124 | Herbivores (H): 32 | Carnivores (C): 4 | Omnivores (O): 11
```

## License

This project is open source and available under the MIT License. See the `LICENSE` file for details.
