# Data-Oriented, Multithreaded Ecosystem Simulation

A high-performance C++ simulation that models complex predator-prey and **resource dynamics** using **data-oriented design** and **multithreading**, managing autonomous agents in a dynamic 2D world.

## Overview

This project simulates a living ecosystem where each creature is represented as data processed by independent systems. It goes far beyond simple rules, leveraging a Data-Oriented Design (DOD) and multithreading to create a fast and scalable "digital petri dish" where complex and unpredictable **emergent behaviors** arise from the interactions of entities and their environment, including the crucial factor of limited, regenerating resources.

Entities in this simulation perceive their world, actively seek out food resources like grass, form cooperative herds for defense, get hungry, feel desperation, suffer lasting injuries from fights, grow old and weak, and provide different nutritional value based on their age. This creates a deeply layered and realistic model of population dynamics, resource competition, and the struggle for survival, designed for performance at high entity counts.

## Core Features

- **Data-Oriented Design (DOD):** Core simulation logic operates directly on attribute data stored in contiguous arrays (`EntityManager`), optimizing for cache efficiency.
- **Multithreading (OpenMP):** Key simulation systems (AI, Movement, Metabolism) are parallelized to leverage multi-core processors, significantly increasing performance and scalability at high entity counts.
- **System-Based Simulation Loop:** Simulation logic is organized into independent systems (AI, Movement, Action, Metabolism, Reproduction) that process entity data in batches.
- **Intelligent Agent Behavior:** Entities possess states processed by the AI System (`WANDERING`, `FLEEING`, `CHASING`, `PACK_HUNTING`, `HERDING`, `SEEKING_FOOD`).
- **Dynamic Attribute System:** Every entity has stats for **Health, Damage, Speed, and Sight**. These stats change dynamically based on hunger and aging.
- **Resource System:** The world contains consumable, regenerating resources (starting with "Grass"). Entities must actively seek out and consume these resources via the Action System to gain energy. Over-consumption leads to depletion and localized scarcity, driving migration.
- **Complex Survival Mechanics (Managed by Metabolism/Action Systems):**
    - **Hunger & Desperation:** Low energy triggers a high-risk, high-reward "desperation mode," boosting an entity's stats at the cost of its health, forcing it to find food quickly.
    - **Tiered Health Regeneration:** Entities can recover from damage over time, but severe injuries permanently lower their maximum health cap across four different tiers, reflecting lasting injury.
    - **Aging System:** As entities grow older, their base stats (like speed and damage) decline, making them less effective hunters and more vulnerable. This effect is most pronounced in high-energy predators.
    - **Age-Based Nutritional Value:** The energy gained from a kill is dynamically calculated based on the age of the killed entity, making older, weaker ones a lower-reward target and influencing hunting strategy.
- **Advanced Social & AI Behaviors (Managed by AI/Movement/Action Systems):**
    - **Herding Behavior:** Herbivore entities seek each other out to form herds (`HERDING` state). Being in a group provides a dynamic **Max HP bonus**, representing "safety in numbers."
    - **Strategic Hunting:** Carnivore entities hunt Herbivores (primary prey) or lone/small groups of Omnivores if they are not in a threatening pack size (`CHASING` state).
    - **Pack Dynamics:** Omnivore entities form packs to hunt powerful Carnivores (`PACK_HUNTING` state), pooling their strength. Carnivores are smart enough to recognize and `FLEE` from a threatening Omnivore pack.
- **Real-time ASCII Visualization:** The world state, including entity locations and resource status, is rendered in the console with live-updating population counts.
- **Modular Resource Definitions:** Resource types and their properties are defined externally (`Resource.h/.cpp`), allowing easy addition of new consumables.

## Changelog

- **v1.0: Initial Predator-Prey Simulation**
    - Created the basic world grid with two species: Herbivores (`H`) and Carnivores (`W`).
    - Animals moved randomly and had a simple energy-based life system.
- **v1.5: Omnivore & Group Hunting Update**
    - Added the `Omnivore` (`O`) species, which can both graze and hunt Herbivores.
    - Implemented basic group hunting logic: Omnivores can hunt Carnivores in packs, and vice-versa.
    - Refactored `World` class with a generic `getAnimalsNear<T>()` function (using OOP types).
- **v2.0: Intelligent Agent Overhaul (The "AI Update")**
    - Reworked the entire system from rule-based to state-driven AI (Finite State Machine).
    - Replaced the simple energy system with a robust attribute system: **Health (HP), Damage, Speed, and Sight Radius**.
    - Implemented intelligent AI behaviors: **Fleeing** from threats, **Chasing** prey, and more nuanced **Pack Hunting**.
- **v2.5: Advanced Survival Mechanics**
    - **Tiered Health Regeneration:** Implemented a multi-tiered system where entities can heal from damage, but severe wounds permanently lower their maximum HP cap.
    - **Hunger & Desperation System:** Introduced a high-risk, high-reward state where low energy causes health drain but provides temporary stat boosts.
- **v3.0: Lifecycle & Ecosystem Balancing**
    - **Aging System:** Entities now grow weaker as they age, with base stats declining over time. The effect is most pronounced in high-energy predators.
    - **Age-Based Nutritional Value:** The energy gained from a kill is dependent on the age of the prey, making older, weaker entities less nutritious.
- **v3.5: Cooperative Social Behavior**
    - **Herding Behavior:** Herbivore entities possess a `HERDING` state and actively seek out others.
    - **Herd Health Bonus:** Being in a group grants a dynamic bonus to Max HP.
- **v4.0: Resource System Integration**
    - **Consumable Resources:** Introduced a resource layer (`Grass`) on the world grid.
    - **Resource Depletion & Regrowth:** Resources are consumed and regrow over time.
    - **Food Seeking AI:** Herbivores and Omnivores now have a `SEEKING_FOOD` state and move towards resources when hungry.
    - **Starvation:** Hunger is tied to resource availability; failure to find food leads to death.
    - **Modular Resource Definitions:** Resource types defined externally.
- **v5.0: Data-Oriented Design (DOD) Refactor (Phase 1)**
    - Replaced the OOP Animal class hierarchy with a central `EntityManager` holding entity data in parallel arrays (Structure of Arrays - SoA).
    - Simulation logic was rewritten into independent "Systems" (Metabolism, Movement, AI, Action, Reproduction) that operate directly on the `EntityManager`'s data.
    - Removed `dynamic_cast` where possible, replacing it with `AnimalType` enum checks for performance.
    - Implemented fast O(1) entity destruction (`EntityManager::destroyEntity`).
    - Implemented spatial partitioning (`World::spatial_grid`) for efficient neighbor finding (O(N*k) vs O(N^2)).
- **v5.5: Multithreading Implementation (Phase 2)**
    - Integrated OpenMP to enable multithreading.
    - Parallelized computation-heavy systems (`AISystem`, `MovementSystem`, `MetabolismSystem`) using `#pragma omp parallel for`.
    - Maintained single-threaded execution for systems modifying the underlying data structure (`ActionSystem`, `ReproductionSystem`, `EntityManager::destroyDeadEntities`) to ensure thread safety.
    - Finalized the system execution order in `World::update` to respect data dependencies and synchronization points.

## Building and Running

### Prerequisites
- A C++17 compatible compiler (e.g., g++, clang++)
- The `make` utility (recommended)
- OpenMP library (usually included with modern g++/clang++)

### Project Structure (Includes Subdirectory)
```
ecosystem-simulation/
├── main.cpp               # Entry point, simulation loop, and parameters
├── World.cpp              # Manages the grid, spatial partitioning, and system execution order
├── EntityManager.cpp      # Manages all entity data in parallel arrays (SoA)
├── SimulationSystems.cpp  # Contains implementations of all simulation systems (AI, Movement, Action, Metabolism, Reproduction)
├── Resource.cpp           # Defines specific resource types (e.g., Grass)
├── Tile.cpp               # Manages resource state on individual grid tiles
├── Random.cpp             # Defines the global random number generator
├── Makefile               # Build configuration
└── includes/
    ├── World.h
    ├── EntityManager.h
    ├── SimulationSystems.h
    ├── Resource.h
    ├── Tile.h
    ├── Random.h
    ├── AnimalConfig.h     # Centralized balancing constants
    └── AnimalTypes.h      # Defines shared enums (AnimalType, AIState)
```

### Build with Make (Using OpenMP)
```bash
# Clean previous builds and compile with OpenMP enabled
make clean && make
```
*Note: The Makefile uses the `-fopenmp` flag during both compilation and linking to enable multithreading.*

### Run the Simulation
```bash
./simulation
```

## Core Architecture

The simulation is built around a Data-Oriented Design (DOD) using an Entity-Component-System (ECS) inspired architecture, although it doesn't use separate "Components" directly but rather a central "Structure of Arrays".

-   **Entities:** Not objects in the OOP sense, but simply indices (`size_t`) that group related data across the vectors in the `EntityManager`.
-   **EntityManager:** The central data store holding all entity attributes (position, health, energy, type, state, etc.) in vectors. Manages entity creation (push_back) and fast destruction (swap-and-pop).
-   **World:** Contains the environment grid (`Tile`s), the `EntityManager`, and the spatial grid (mapping regions to entity IDs). It orchestrates the simulation by calling the systems in the correct order each turn. It provides necessary environmental data (`World::grid`, `World::spatial_grid`) to the systems.
-   **Systems:** Global functions (within namespaces) that implement the simulation logic by iterating over the `EntityManager`'s data vectors. They process entities based on their attributes and state and update their data. Examples: `AISystem`, `MovementSystem`, `ActionSystem`, `MetabolismSystem`, `ReproductionSystem`. Systems are designed to be parallelizable where possible, operating on data without conflicting writes across threads, or run sequentially when necessary (e.g., modifying vector structure, cross-entity writes like combat).

## Configuration & Balancing

Initial population counts, world size, spatial grid cell size, and simulation speed can be modified in `main.cpp`. The core balance of the ecosystem is determined by the extensive attribute and behavior constants defined in `includes/AnimalConfig.h` and the resource properties in `Resource.cpp`. Tweaking these values is essential for achieving stable or interesting population dynamics.

## License

This project is open source and available under the MIT License.

Copyright (c) 2025 Samarth Shrivastava