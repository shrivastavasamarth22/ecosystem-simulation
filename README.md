# Data-Oriented, Multithreaded Ecosystem Simulation

A high-performance C++ simulation that models complex predator-prey and **resource dynamics** using a **data-oriented architecture** and **multithreading**, visualized graphically with SFML.

## Overview

This project simulates a living ecosystem where each creature is represented purely as data processed by independent systems. It's designed for high performance and scalability, leveraging a Data-Oriented Design (DOD) and multithreading to create a "digital petri dish" where complex and unpredictable **emergent behaviors** arise from the interactions of entities and their environment, including the crucial factor of limited, regenerating resources and intra-species competition.

Entities in this simulation perceive their world, actively seek out food resources like grass and berries, form cooperative herds for defense, manage hunger, feel desperation when starving, suffer lasting injuries from fights, grow old and weak, provide different nutritional value based on their age, and compete fiercely with rivals of their own kind. The simulation is visualized in a dedicated graphical window with textures and real-time UI.

## Core Features

### Foundational Architecture
- **Data-Oriented Design (DOD):** Core simulation logic operates directly on attribute data stored in contiguous arrays (`EntityManager`), optimizing for cache efficiency.
- **Multithreading (OpenMP):** Computation-heavy simulation systems (AI, Movement, Metabolism) are parallelized across multiple CPU cores for significant performance gains at high entity counts.
- **System-Based Simulation Loop:** Simulation logic is organized into independent systems (AI, Movement, Action, Metabolism, Reproduction) that process entity data in batches within a carefully ordered main loop.
- **Spatial Partitioning:** Uses a grid-based spatial partitioning (`World::spatial_grid`) for efficient, near O(1) neighbor finding (vs. O(N) in a simple list), drastically improving performance for perception and interaction systems.
- **Biome-Based Terrain Generation:** The world is procedurally generated using a Voronoi diagram to create distinct, irregular biomes (Forest, Grassland, Barren). Each biome has a unique resource profile, influencing entity distribution and creating strategic points of interest.

### Entity Behavior & Survival
- **Intelligent Agent Behavior:** Entities possess states processed by the AI System (`WANDERING`, `FLEEING`, `CHASING`, `PACK_HUNTING`, `HERDING`, `SEEKING_FOOD`).
- **Dynamic Attribute System:** Every entity has stats for **Health (HP), Damage, Speed, and Sight Radius**. These stats change dynamically based on hunger levels and aging effects.
- **Hunger & Desperation System:** Low energy triggers a high-risk, high-reward "desperation mode," temporarily boosting an entity's combat and movement stats at the cost of health drain, forcing a desperate search for food.
- **Tiered Health Regeneration:** Entities can recover HP over time if they avoid damage, but severe injuries (tiers of health below max) permanently lower their maximum health cap, reflecting lasting injury.
- **Aging System:** As entities grow older, their base stats (like speed, damage, sight) decline, making them less effective and more vulnerable. The rate and impact of aging differ by species, being more pronounced in predators.

### Ecological & Social Dynamics
- **Resource System:** The world grid contains consumable, regenerating resources (starting with "Grass" and "Berries"). Entities must actively seek out specific resource types and consume them via the Action System to gain energy based on the resource's nutritional value. Over-consumption leads to local depletion, driving migration and starvation.
- **Age-Based Nutritional Value:** The energy gained from a kill is dynamically calculated based on the age and type of the killed entity, making older, weaker ones a lower-reward target and influencing predator hunting strategies.
- **Herding Behavior:** Herbivore entities possess a `HERDING` state and actively seek out others of their kind to form groups.
- **Herd Health Bonus:** Being in a herd grants a dynamic bonus to Max HP, representing "safety in numbers" and making herd members tougher targets for predators.
- **Strategic Hunting:** Carnivore entities hunt Herbivores (primary prey) or strategically hunt lone/small groups of Omnivores if they are not in a threatening pack size (`CHASING` state). Omnivores hunt Herbivores and can form packs to hunt powerful Carnivores (`PACK_HUNTING` state).
- **Basic Territorial Behavior:** Carnivores exhibit territoriality and will engage in lethal combat (`CHASING` state leading to combat) with other Carnivores that enter their territorial radius. Cannibalism does not provide energy gain in territorial fights.
- **Natural Population Control:** Complex interactions between resource availability, predator-prey relationships, aging, hunger, and intra-species conflict provide dynamic mechanisms for population booms, busts, and cycles.

### Visualization & User Interface
- **SFML Graphical Window:** Replaces console output with a dedicated window for visualization.
- **Textured Grid Rendering:** Draws the world grid using different textures for empty tiles and different resource types (Grass, Berries).
- **Textured Entity Rendering:** Draws living entities as sprites using textures corresponding to their species type.
- **Enhanced UI System:** Real-time overlay displaying simulation statistics with sprite-based counters and semi-transparent backgrounds for better readability.
- **Interactive Camera System:** Full camera control with smooth zoom (mouse wheel), pan (click & drag), reset functionality (R key), and intelligent bounds constraint.
- **Simulation State Management:** Visual pause indicator, simulation end notification with prominent center-screen message.
- **Background Music:** Plays an MP3 file on loop in the background for atmospheric effect.

### Controls & Interaction
- **Mouse Wheel:** Smooth zoom in/out with intelligent limits
- **Left Click + Drag:** Pan camera around the simulation world
- **R Key:** Reset camera to optimal view showing entire world
- **P Key:** Pause/unpause simulation
- **Escape Key:** Close simulation window

## Project Structure

```
ecosystem-simulation/
├── src/                          # Source files
│   ├── main.cpp                  # Entry point, initializes World/Renderer, runs main SFML loop
│   ├── core/                     # Core simulation logic
│   │   ├── World.cpp            # Manages grid, spatial partitioning, orchestrates system execution
│   │   ├── EntityManager.cpp    # Central data store (SoA) and entity lifecycle management
│   │   └── Random.cpp           # Global random number generator
│   ├── systems/                  # Simulation systems
│   │   └── SimulationSystems.cpp # AI, Movement, Action, Metabolism, Reproduction systems
│   ├── resources/                # Resource and environment management
│   │   ├── Resource.cpp         # Resource type definitions (Grass, Berries)
│   │   ├── Tile.cpp             # Individual grid tile management
│   │   └── Biome.cpp            # Biome type definitions (Forest, Grassland, etc.)
│   └── graphics/                 # Rendering and visualization
│       ├── GraphicsRenderer.cpp # SFML window, textures, UI rendering
│       ├── Camera.cpp           # Handles camera movement, zoom, and input
│       └── UIManager.cpp        # Manages all UI elements and overlays
│
├── include/                      # Header files
│   ├── core/                     # Core component headers
│   ├── systems/                  # System headers
│   ├── resources/                # Resource headers
│   ├── graphics/                 # Graphics headers
│       ├── GraphicsRenderer.h # Main renderer class
│       ├── Camera.h           # Camera class header
│       └── UIManager.h        # UI Manager class header
│   └── common/                   # Shared definitions
│       ├── AnimalConfig.h       # Balancing constants for all species
│       └── AnimalTypes.h        # Shared enums (AnimalType, AIState)
│
├── assets/                       # Game assets
│   ├── textures/                # Entity and tile sprites (PNG)
│   ├── fonts/                   # UI fonts (TTF)
│   └── audio/                   # Background music (MP3)
│
├── build/                        # Build artifacts
│   └── obj/                     # Compiled object files
│
├── external/                     # Third-party libraries
│   └── SFML/                    # SFML graphics library
│
├── docs/                         # Documentation
└── Makefile                      # Build configuration using g++ and OpenMP
```

## Building and Running

### Prerequisites
- A C++17 compatible compiler (e.g., g++, clang++)
- The `make` utility (recommended)
- **SFML 2.5.1 or later** (or compatible version for your compiler) with **Graphics, Window, System, and Audio modules**.
- OpenMP library (usually included with modern g++/clang++)

### Setting up SFML on Windows (MinGW-w64)
1.  Install MinGW-w64 with g++ and make (e.g., via Chocolatey `choco install mingw` or Scoop `scoop install make gcc`).
2.  Download the correct "MinGW-w64" build of SFML from the official website ([sfml-dev.org](https://www.sfml-dev.org/download.php)) that matches your compiler version.
3.  Extract the SFML zip into the `external/SFML/` directory in your project root.
4.  Copy the necessary SFML DLL files (`sfml-graphics-*.dll`, `sfml-window-*.dll`, `sfml-system-*.dll`, `sfml-audio-*.dll`, and potentially dependencies like `openal32.dll`, etc.) from `external/SFML/bin` into the **same directory as your `simulation.exe`**.

### Build with Make (Using OpenMP)
Open your terminal (Command Prompt/PowerShell on Windows, or your Linux terminal), navigate to the project root, and run:
```bash
make clean && make
```
*Note: The Makefile uses the `-fopenmp` flag during both compilation and linking to enable multithreading and links the necessary SFML libraries.*

### Run the Simulation
```bash
./simulation.exe # On Windows
./simulation     # On Linux/WSL
```

## Core Architecture

The simulation is built around a Data-Oriented Design (DOD) using an Entity-System pattern.

-   **Entities:** Abstract concepts, identified by indices (`size_t`), which group related data across the vectors in the `EntityManager`.
-   **EntityManager:** The central container for all entity attributes (position, health, energy, type, state, etc.) stored in vectors (the Structure of Arrays). It also manages entity creation (push_back) and fast destruction (swap-and-pop).
-   **World:** Represents the simulation environment. It contains the regenerating resource grid (`Tile`s), owns the `EntityManager`, and the spatial partitioning grid. Its main role is to orchestrate the simulation turn by calling the various Systems in the correct, dependency-respecting order. It provides environmental data to the Systems.
-   **Systems:** Stateless functions (grouped within namespaces) that implement distinct pieces of simulation logic. They iterate over the `EntityManager`'s data vectors, reading specific attributes and writing updated values. Systems are designed to be parallelizable (`AI`, `Movement`, `Metabolism`) or run sequentially (`Action`, `Reproduction`, `EntityManager::destroyDeadEntities`) based on data access patterns and thread safety requirements.

## Configuration & Balancing

Initial population counts, world size, spatial grid cell size, and simulation speed can be modified in `src/main.cpp`. The core balance of the ecosystem is determined by the extensive attribute and behavior constants defined in `include/common/AnimalConfig.h` and the resource properties in `src/resources/Resource.cpp`. Tweaking these values is essential for achieving stable or interesting population dynamics.

## Changelog

- **v1.0: Initial Console Predator-Prey**
    - Created the basic console-based simulation with a grid and simple Herbivore (`H`) and Carnivore (`W`) species. Animals moved randomly with a simple energy system.
- **v1.1: Omnivore & Group Hunting**
    - Added the `Omnivore` (`O`) species. Implemented basic group hunting logic (Omnivores hunt Carnivores in packs, Carnivores flee Omnivore packs). Renamed Carnivore symbol to 'C'.
- **v1.2: Intelligent Agent AI**
    - Reworked animal behavior from simple rules to a state-driven AI (FSM). Replaced simple energy-as-life with a robust attribute system (Health, Damage, Speed, Sight). Implemented basic Fleeing and Chasing behaviors.
- **v1.3: Advanced Survival Mechanics**
    - **Tiered Health Regeneration:** Implemented a multi-tiered system where entities heal over time, with severe wounds permanently lowering their max HP cap.
    - **Hunger & Desperation System:** Introduced a system where low energy causes health drain but temporarily boosts stats.
- **v1.4: Lifecycle Dynamics**
    - **Aging System:** Entities grow weaker as they age, affecting stats.
    - **Age-Based Nutritional Value:** Energy gained from kills depends on the prey's age and type.
- **v1.5: Social & Territorial Behavior**
    - **Herding Behavior:** Herbivores seek herds for a dynamic Max HP bonus.
    - **Basic Territoriality:** Carnivores fight other Carnivores within a specific radius (no energy gain from cannibalism).
- **v1.6: Resource System Integration**
    - **Consumable Resources:** Introduced resources (Grass, Berries) on the grid with depletion and regrowth.
    - **Food Seeking AI:** Herbivores/Omnivores now actively seek the highest-energy food sources when hungry. Starvation death implemented.
    - **Modular Resources:** Resource types defined externally.
- **v1.7: Data-Oriented Architecture**
    - Major refactor to a Data-Oriented Design. Replaced OOP Animal hierarchy with `EntityManager` (SoA) and System-based logic (`SimulationSystems`). Implemented fast entity destruction and spatial partitioning (`World::spatial_grid`) for performance. Removed `dynamic_cast` dependencies for core logic.
- **v1.8: Multithreading Implementation**
    - Integrated OpenMP. Parallelized key systems (AI, Movement, Metabolism). Maintained single-threaded execution for thread-sensitive systems (Action, Reproduction, Cleanup) with proper sequencing and synchronization.
- **v1.9: SFML Visual Overhaul Beginning**
    - Replaced console drawing with a dedicated SFML graphical window.
    - Implemented drawing of the world grid using textures for different tile/resource types.
    - Added background music playback using SFML Audio.
- **v2.0: SFML Visual Overhaul Complete**
    - Added animal sprites
    - Implemented Basic UI stats
    - Added basic keyboard events as well as play/pause functionality for the sim.
- **v2.1: Project Reorganization**
    - Restructured project with organized directory hierarchy (`src/`, `include/`, `assets/`, `build/`, `external/`, `docs/`)
    - Separated source code into logical modules (core, systems, resources, graphics)
    - Reorganized assets into categorized subdirectories (textures, fonts, audio)
    - Updated build system with clean object file management
    - Added comprehensive documentation structure
- **v2.2: Bug Fixes & Movement Enhancement**
    - Fixed critical bug where animals could get permanently stuck when seeking already-consumed resources
    - Implemented diagonal movement for more natural animal paths (replaced jerky L-shaped movement)
    - Resolved combat deadlock where entities on the same tile couldn't attack each other
    - Corrected UI entity count discrepancy between displayed totals and actual living entities
- **v2.3: Enhanced User Interface**
    - Replaced text-based species counters with visual sprite-based display
    - Added semi-transparent backgrounds to all UI elements for improved readability
    - Implemented "Simulation Ended" message with prominent center-screen display
    - Enhanced visual feedback and polish throughout the interface
- **v2.4: Interactive Camera System**
    - Added comprehensive camera system with smooth zoom, pan, and reset functionality
    - Implemented intelligent camera bounds and zoom constraints to prevent infinite scrolling
    - Added mouse wheel zoom with zoom-to-cursor behavior for intuitive navigation
    - Integrated left-click drag panning with proper coordinate transformation
    - Added R key reset to automatically fit entire simulation world in view

## License

This project is open source and available under the MIT License.

Copyright (c) 2025 Samarth Shrivastava