# Data-Oriented, Multithreaded Ecosystem Simulation

A high-performance C++ simulation that models complex predator-prey and **resource dynamics** using a **data-oriented architecture** and **multithreading**, visualized graphically with SFML.

## Latest Updates

- **v2.13: Wave Function Collapse Terrain Generation & Enhanced Biome System**
    - **Wave Function Collapse (WFC) Implementation:** Replaced Voronoi diagram biome generation with a sophisticated WFC algorithm that creates more natural, realistic terrain patterns with proper adjacency rules between biome types.
    - **Multi-Scale Terrain Generation:** Implemented hybrid approach using large region assignment (15x15 tile blocks) followed by WFC-based boundary smoothing, creating coherent biome areas instead of fragmented individual tiles.
    - **Enhanced Biome Diversity:** Expanded from 3 to 6 distinct biomes (Water, Barren, Rocky, Grassland, Forest, Fertile) with unique terrain/resource combinations and realistic adjacency relationships.
    - **Terrain vs Resource Separation:** Complete architectural redesign separating terrain types (affecting movement/stats) from resource types (consumable items), enabling richer environmental interactions.
    - **New Terrain System:** Added water terrain (slows movement), rocky terrain (blocks omnivores), and normal terrain with proper movement restrictions and stat modifiers integrated into movement and metabolism systems.
    - **Bush Resource Addition:** Introduced bush resource type alongside existing grass and berries, with proper texture support and biome-specific distribution patterns.
    - **Intelligent Adjacency Rules:** WFC uses realistic biome transition rules (e.g., water connects to grassland/forest for rivers, rocky areas near forests for mountains) creating natural-looking landscapes.
    - **Robust Fallback System:** If WFC generation fails due to constraint conflicts, system gracefully falls back to random generation ensuring simulation always starts successfully.
- **v2.12: Entity Detail Cam & Critical Bug Fixes**
    - **Entity Detail Camera System:** Implemented comprehensive entity inspection feature - click any entity to enter detail view mode with camera follow and real-time information panel showing health, energy, AI state, family relationships, and detailed stats.
    - **Parent-Child Protection for Carnivores:** Fixed critical bug where carnivores would immediately attack their own newborn offspring. Added parent tracking system that prevents family conflicts until offspring reach independence age (8 turns).
    - **Percentage-Based Aging Reduction:** Completely redesigned herd bonus system from flat health increases to sophisticated aging reduction. Herbivores in herds now receive 8% aging penalty reduction per herd member (max 50%), preventing immortal 200+ health herbivores while maintaining strategic herding benefits.
    - **Interactive Entity Selection:** Advanced selection system with visual indicators, smooth camera follow mode, and comprehensive real-time data display including family relationships, life stage, and condition assessments.
    - **Enhanced UI Polish:** Color-coded entity information, section-organized detail panels, and intuitive interaction design with click-to-exit functionality.
- **v2.11: Animation System & Reproduction Bug Fixes**
    - **Smooth Animation System:** Implemented position interpolation with quadratic easing for natural entity movement, eliminating jerky transitions and providing professional visual polish.
    - **Animation State Capture:** Added `AnimationSystem` that captures previous positions before each simulation update, enabling smooth interpolation between turns.
    - **Reproduction Animation Fix:** Fixed critical bug where newly reproduced entities appeared to "teleport" from (0,0) to spawn location by properly initializing previous positions for new entities.
    - **Performance-Optimized Rendering:** Animation interpolation runs independently of simulation logic, maintaining 60fps visual smoothness regardless of simulation speed.
- **v2.10: Advanced AI Behavior & System Reliability Improvements**
    - **Distance-Based Target Selection:** Implemented intelligent target selection where all animals (territorial carnivores, herding herbivores, fleeing entities) choose the closest appropriate target instead of the first found, creating optimal and realistic behavior patterns.
    - **Enhanced Food Seeking Algorithm:** Completely revamped foraging with energy-to-distance ratio optimization, preventing unrealistic cross-map travel while encouraging efficient local foraging with proximity bonuses for nearby food sources.
    - **Consistent Threat Detection:** Fixed predator flee priority issues by making all threat detection use full sight radius, allowing animals to flee from any visible danger rather than waiting until threats are dangerously close.
    - **Combat Range Compatibility:** Fixed combat system to use Euclidean distance (√2 ≈ 1.4 tiles) instead of Manhattan distance, making it compatible with diagonal movement and preventing endless chase loops.
    - **Starvation Death Implementation:** Added explicit starvation death when energy reaches zero, creating realistic survival pressure and natural population control through resource competition.
    - **Enhanced Mathematical Robustness:** Improved distance calculations, boundary checking, and target validation across all systems for more reliable and consistent behavior.

## Overview

This project simulates a living ecosystem where each creature is represented purely as data processed by independent systems. It's designed for high performance and scalability, leveraging a Data-Oriented Design (DOD) and multithreading to create a "digital petri dish" where complex and unpredictable **emergent behaviors** arise from the interactions of entities and their environment, including the crucial factor of limited, regenerating resources and intra-species competition.

Entities in this simulation perceive their world, actively seek out food resources like grass and berries, form cooperative herds for defense, manage hunger, feel desperation when starving, suffer lasting injuries from fights, grow old and weak, provide different nutritional value based on their age, and compete fiercely with rivals of their own kind. The simulation is visualized in a dedicated graphical window with textures and real-time UI.

## Core Features

### Foundational Architecture
- **Data-Oriented Design (DOD):** Core simulation logic operates directly on attribute data stored in contiguous arrays (`EntityManager`), optimizing for cache efficiency.
- **Modular System Architecture:** Simulation logic is organized into 6 independent, focused systems (AI, Movement, Action, Metabolism, Reproduction, Animation) that process entity data in batches within a carefully ordered main loop. Each system has its own dedicated `.cpp` and `.h` files for better maintainability and modularity.
- **Smooth Animation System:** Professional-grade animation interpolation with quadratic easing provides smooth entity movement between simulation turns, maintaining visual fluidity regardless of simulation speed.
- **Multithreading (OpenMP):** Computation-heavy simulation systems (AI, Movement, Metabolism, Animation) are parallelized across multiple CPU cores for significant performance gains at high entity counts, with thread-safe execution and proper race condition prevention.
- **Dynamic Spatial Partitioning:** Uses an auto-optimizing grid-based spatial partitioning (`World::spatial_grid`) that calculates optimal cell sizes based on world dimensions and entity density for efficient, near O(1) neighbor finding.
- **View Frustum Culling:** Advanced rendering optimization that only processes visible tiles and entities, providing dramatic performance improvements for large worlds (80-95% rendering speedup when zoomed in).
- **Optimized Asset Pipeline:** Uses appropriately-sized textures (40×40 for tiles, 64×64 for entities) with fixed scaling factors to minimize GPU memory usage and eliminate dynamic scaling overhead.
- **Biome-Based Terrain Generation:** The world is procedurally generated using Wave Function Collapse (WFC) algorithm to create natural, realistic terrain with proper adjacency rules. Six distinct biomes (Water, Barren, Rocky, Grassland, Forest, Fertile) each have unique terrain and resource profiles, with multi-scale generation creating large coherent regions rather than scattered individual tiles.

### Entity Behavior & Survival
- **Intelligent Agent Behavior:** Entities possess states processed by the AI System (`WANDERING`, `FLEEING`, `CHASING`, `PACK_HUNTING`, `HERDING`, `SEEKING_FOOD`) with distance-based target selection for optimal decision-making.
- **Dynamic Attribute System:** Every entity has stats for **Health (HP), Damage, Speed, and Sight Radius**. These stats change dynamically based on hunger levels and aging effects.
- **Advanced Food Seeking:** Sophisticated foraging algorithm that prioritizes energy-to-distance ratio, preventing unrealistic long-distance travel while encouraging local resource utilization with proximity bonuses.
- **Hunger & Desperation System:** Low energy triggers a high-risk, high-reward "desperation mode," temporarily boosting an entity's combat and movement stats at the cost of health drain, forcing a desperate search for food.
- **Starvation Mechanics:** Animals die immediately when energy reaches zero, creating realistic survival pressure and natural population control through resource competition.
- **Tiered Health Regeneration:** Entities can recover HP over time if they avoid damage, but severe injuries (tiers of health below max) permanently lower their maximum health cap, reflecting lasting injury.
- **Comprehensive Aging System:** As entities grow older, their base stats (like speed, damage, sight) decline progressively after their prime age using dedicated aging penalties. This creates realistic lifecycle dynamics where older entities become less effective and more vulnerable, with species-specific aging rates that influence hunting strategies and population dynamics.

### Ecological & Social Dynamics
- **Advanced Resource & Terrain System:** The world features a dual-layer system with terrain types (water, rocky, normal) affecting movement speed and accessibility, and resource types (grass, berries, bush) providing consumable energy. Terrain restrictions prevent certain animal types from accessing specific areas (e.g., omnivores blocked by rocky terrain), while terrain modifiers affect movement speed and sight radius.
- **Age-Based Nutritional Value:** The energy gained from a kill is dynamically calculated based on the age and type of the killed entity, making older, weaker ones a lower-reward target and influencing predator hunting strategies.
- **Intelligent Herding Behavior:** Herbivore entities possess a `HERDING` state and actively seek out the closest herd members to form optimal groups. Herd aging reduction calculations are processed in a thread-safe manner to prevent race conditions while maintaining performance.
- **Herd Aging Reduction:** Being in a herd provides percentage-based aging penalty reduction (8% per member, max 50%), representing "safety in numbers" and community wisdom. This prevents health inflation while maintaining strategic herding benefits and ensuring natural lifespans.
- **Family Protection System:** Carnivores track parent-child relationships to prevent attacking their own offspring until independence age, eliminating unnatural family conflicts while maintaining territorial behavior against non-relatives.
- **Strategic Hunting with Distance Optimization:** Carnivore entities hunt Herbivores (primary prey) or strategically hunt lone/small groups of Omnivores if they are not in a threatening pack size (`CHASING` state). Omnivores hunt Herbivores and can form coordinated packs to hunt powerful Carnivores (`PACK_HUNTING` state), with all hunting behaviors using closest-target selection for optimal engagement.
- **Enhanced Territorial Behavior:** Carnivores exhibit territoriality and will engage in lethal combat (`CHASING` state leading to combat) with the closest rival Carnivores within their territorial radius, excluding family members. Combat uses Euclidean distance for realistic engagement ranges compatible with diagonal movement. Cannibalism does not provide energy gain in territorial fights.
- **Consistent Threat Response:** All predator-prey interactions use full sight radius for threat detection, allowing animals to flee from any visible danger rather than waiting until threats are dangerously close.
- **Natural Population Control:** Complex interactions between resource availability, predator-prey relationships, aging, hunger, starvation death, and intra-species conflict provide dynamic mechanisms for population booms, busts, and cycles.

### Visualization & User Interface
- **SFML Graphical Window:** Replaces console output with a dedicated window for visualization.
- **Smooth Animation System:** Professional-grade position interpolation with quadratic easing creates fluid entity movement between simulation turns, providing cinematic visual quality.
- **Entity Detail Camera System:** Interactive entity inspection with click-to-select, camera follow mode, and comprehensive real-time information panels showing health, energy, AI state, family relationships, and detailed stats.
- **Textured Grid Rendering:** Draws the world grid using different textures for empty tiles and different resource types (Grass, Berries).
- **Textured Entity Rendering:** Draws living entities as sprites using textures corresponding to their species type with smooth animated movement and selection indicators.
- **Enhanced UI System:** Real-time overlay displaying simulation statistics with sprite-based counters, semi-transparent backgrounds, and interactive entity detail panels with color-coded information.
- **Interactive Camera System:** Full camera control with smooth zoom (mouse wheel), pan (click & drag), reset functionality (R key), entity follow mode, and intelligent bounds constraint.
- **Simulation State Management:** Visual pause indicator, simulation end notification with prominent center-screen message.
- **Background Music:** Plays an MP3 file on loop in the background for atmospheric effect.

### Controls & Interaction
- **Mouse Wheel:** Smooth zoom in/out with intelligent limits
- **Left Click + Drag:** Pan camera around the simulation world
- **Left Click on Entity:** Select entity for detailed inspection and camera follow
- **Left Click on Empty Space:** Deselect entity and return to normal camera mode
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
│   │   ├── Random.cpp           # Global random number generator
│   │   └── WFCGenerator.cpp     # Wave Function Collapse terrain generation algorithm
│   ├── systems/                  # Modular simulation systems
│   │   ├── AISystem.cpp         # AI decision-making and behavior states
│   │   ├── MovementSystem.cpp   # Entity movement and pathfinding
│   │   ├── ActionSystem.cpp     # Combat, resource consumption, state transitions
│   │   ├── MetabolismSystem.cpp # Aging, hunger, health regeneration
│   │   ├── ReproductionSystem.cpp # Entity reproduction logic
│   │   └── AnimationSystem.cpp  # Position capture and animation state management
│   ├── resources/                # Resource and environment management
│   │   ├── Resource.cpp         # Resource type definitions (Grass, Berries, Bush)
│   │   ├── Tile.cpp             # Individual grid tile management with terrain support
│   │   ├── Biome.cpp            # Biome type definitions (Water, Rocky, Forest, etc.)
│   │   └── Terrain.cpp          # Terrain type definitions and movement rules
│   └── graphics/                 # Rendering and visualization
│       ├── GraphicsRenderer.cpp # SFML window, textures, UI rendering, animation interpolation
│       ├── Camera.cpp           # Handles camera movement, zoom, and input
│       └── UIManager.cpp        # Manages all UI elements and overlays
│
├── include/                      # Header files
│   ├── core/                     # Core component headers
│   │   ├── WFCGenerator.h       # Wave Function Collapse algorithm interface
│   │   └── ...                  # Other core headers
│   ├── systems/                  # Individual system headers
│   │   ├── AISystem.h           # AI system interface
│   │   ├── MovementSystem.h     # Movement system interface
│   │   ├── ActionSystem.h       # Action system interface
│   │   ├── MetabolismSystem.h   # Metabolism system interface
│   │   ├── ReproductionSystem.h # Reproduction system interface
│   │   ├── AnimationSystem.h    # Animation system interface
│   │   └── SimulationSystems.h  # Umbrella header (backward compatibility)
│   ├── resources/                # Resource headers
│   │   ├── Terrain.h            # Terrain type definitions and movement rules
│   │   └── ...                  # Other resource headers
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
-   **EntityManager:** The central container for all entity attributes (position, health, energy, type, state, etc.) stored in vectors (the Structure of Arrays). It also manages entity creation (push_back) and fast destruction (swap-and-pop), with proper animation state initialization for newly created entities.
-   **World:** Represents the simulation environment. It contains the regenerating resource grid (`Tile`s), owns the `EntityManager`, and the spatial partitioning grid. Its main role is to orchestrate the simulation turn by calling the various Systems in the correct, dependency-respecting order. It provides environmental data to the Systems.
-   **Systems:** Stateless functions (grouped within namespaces) that implement distinct pieces of simulation logic. They iterate over the `EntityManager`'s data vectors, reading specific attributes and writing updated values. Systems are designed to be parallelizable (`AI`, `Movement`, `Metabolism`, `Animation`) or run sequentially (`Action`, `Reproduction`, `EntityManager::destroyDeadEntities`) based on data access patterns and thread safety requirements.
-   **Animation System:** Captures entity positions before each simulation update and enables smooth interpolation between turns, providing professional visual quality with quadratic easing curves.

## Configuration & Balancing

Initial population counts, world size, spatial grid cell size, and simulation speed can be modified in `src/main.cpp`. The core balance of the ecosystem is determined by the extensive attribute and behavior constants defined in `include/common/AnimalConfig.h` and the resource properties in `src/resources/Resource.cpp`. Tweaking these values is essential for achieving stable or interesting population dynamics.

## License

This project is open source and available under the MIT License.

Copyright (c) 2025 Samarth Shrivastava