# Changelog

## v2.5: Graphics System Refactor & Custom Cursor
**Date:** June 2025
### Major Features
- **Custom Cursor:** Implemented a custom cursor using a sprite for improved visual consistency.

### Technical Improvements
- **Graphics System Refactor:** Decoupled graphics logic by refactoring the `GraphicsRenderer` class into two dedicated classes:
  - `Camera`: Manages the world view, including zoom, pan, and other camera controls.
  - `UIManager`: Manages all UI elements, including stats, overlays, and the new custom cursor.
- **Improved Modularity:** This refactoring significantly improves code organization, making the graphics system more modular and easier to maintain.

## v2.4: Interactive Camera System
**Date:** June 2025
### Major Features
- **Comprehensive Camera System:** Added full camera control with smooth zoom, pan, and reset functionality
- **Mouse Controls:** Implemented mouse wheel zoom with zoom-to-cursor behavior and left-click drag panning
- **Intelligent Constraints:** Added camera bounds and zoom limits to prevent infinite scrolling and viewing beyond world
- **Smooth Interpolation:** Implemented delta-time based smooth zoom transitions for professional user experience
- **Reset Functionality:** Added R key to automatically reset camera to optimal world-fitting view

### Technical Improvements
- Enhanced graphics renderer with separate camera view management
- Implemented proper coordinate transformation between screen and world space
- Added delta-time based camera updates for frame-rate independent smooth movement
- Optimized view switching between world rendering and UI overlay

## v2.3: Enhanced User Interface
**Date:** June 2025
### UI Enhancements
- **Sprite-based Statistics:** Replaced text-based species counters (H, C, O) with actual animal sprite icons
- **Semi-transparent Backgrounds:** Added professional-looking backgrounds to all UI elements for improved readability
- **Simulation End Screen:** Implemented prominent center-screen "Simulation Ended" message with semi-transparent overlay
- **Enhanced Layout:** Improved UI positioning and spacing with larger, more visible animal sprites

### Visual Polish
- Better visual hierarchy with proper background sizing and positioning
- Improved text centering and alignment within UI backgrounds
- More intuitive and visually appealing statistics display

## v2.2: Bug Fixes & Movement Enhancement
**Date:** June 2025
### Critical Bug Fixes
- **Fixed Stuck Animals:** Resolved bug where animals seeking food would become permanently stuck if another animal consumed their target resource first
- **Combat Deadlock Fix:** Fixed issue where two entities on the same tile couldn't attack each other, causing infinite chase loops
- **UI Count Correction:** Fixed discrepancy where UI displayed total entity count instead of living entity count

### Movement Improvements
- **Diagonal Movement:** Replaced jerky L-shaped movement with smooth diagonal pathfinding for more natural animal behavior
- **Enhanced AI Logic:** Improved state transition logic to prevent edge cases in food-seeking behavior
- **Better Pathfinding:** More direct and visually appealing movement patterns for chasing, fleeing, and herding

### Code Quality
- Improved ActionSystem logic with better state management
- Enhanced MovementSystem with simultaneous x/y movement capabilities
- Cleaner main loop condition handling

## v2.1: Project Reorganization
**Date:** June 2025
### Project Structure
- Restructured project with organized directory hierarchy (`src/`, `include/`, `assets/`, `build/`, `external/`, `docs/`)
- Separated source code into logical modules (core, systems, resources, graphics)
- Reorganized assets into categorized subdirectories (textures, fonts, audio)
- Updated build system with clean object file management
- Added comprehensive documentation structure

### Resource System Extension
- **Berry Resource Type:** Added berries as a high-nutrition, low-abundance resource type
- **Resource Balancing:** Implemented different spawn rates and nutritional values for varied ecosystem dynamics
- **Asset Integration:** Added berry texture and proper resource type mapping

## v2.0: SFML Visual Overhaul Complete
**Date:** June 2025
- Added animal sprites with proper texture mapping for each species type
- Implemented comprehensive UI statistics display with turn counter and population counts
- Added keyboard event handling including play/pause functionality (P key)
- Enhanced window management with escape key handling and proper cleanup

## v1.9: SFML Visual Overhaul Beginning
**Date:** June 2025
- Replaced console drawing with a dedicated SFML graphical window
- Implemented drawing of the world grid using textures for different tile/resource types
- Added background music playback using SFML Audio for atmospheric enhancement

## v1.8: Multithreading Implementation
**Date:** May 2025
- Integrated OpenMP for parallel processing of computation-heavy systems
- Parallelized AI, Movement, and Metabolism systems for significant performance gains
- Maintained single-threaded execution for thread-sensitive systems (Action, Reproduction, Cleanup)
- Implemented proper synchronization and data dependency management

## v1.7: Data-Oriented Architecture
**Date:** May 2025
- Major refactor to Data-Oriented Design replacing OOP Animal hierarchy
- Implemented EntityManager with Structure of Arrays (SoA) pattern
- Created System-based logic with SimulationSystems namespace
- Added fast entity destruction with swap-and-pop algorithm
- Implemented spatial partitioning for O(1) neighbor queries
- Removed dynamic_cast dependencies for improved performance

## v1.6: Resource System Integration
**Date:** April 2025
- **Consumable Resources:** Introduced resources (Grass, Berries) on the grid with depletion and regrowth
- **Food Seeking AI:** Herbivores/Omnivores now actively seek the highest-energy food sources when hungry
- **Starvation Mechanics:** Implemented starvation death for realistic population control
- **Modular Resources:** Resource types defined externally for easy expansion

## v1.5: Social & Territorial Behavior
**Date:** April 2025
- **Herding Behavior:** Herbivores seek herds for dynamic Max HP bonus representing safety in numbers
- **Basic Territoriality:** Carnivores fight other Carnivores within territorial radius
- **Anti-Cannibalism:** No energy gain from territorial fights to prevent unrealistic behavior

## v1.4: Lifecycle Dynamics
**Date:** March 2025
- **Aging System:** Entities grow weaker as they age, with species-specific aging rates
- **Age-Based Nutritional Value:** Energy gained from kills depends on prey's age and type
- **Dynamic Stats:** Age-related decline in speed, damage, and sight capabilities

## v1.3: Advanced Survival Mechanics
**Date:** March 2025
- **Tiered Health Regeneration:** Multi-tiered healing system with permanent max HP reduction from severe injuries
- **Hunger & Desperation System:** Low energy causes health drain but temporarily boosts combat stats
- **Risk-Reward Mechanics:** Desperate entities become more dangerous but more vulnerable

## v1.2: Intelligent Agent AI
**Date:** February 2025
- Reworked animal behavior from simple rules to state-driven AI (Finite State Machine)
- Replaced energy-as-life with robust attribute system (Health, Damage, Speed, Sight)
- Implemented sophisticated Fleeing and Chasing behaviors with target acquisition

## v1.1: Omnivore & Group Hunting
**Date:** February 2025
- Added Omnivore species with complex hunting behavior
- Implemented group hunting logic (Omnivores hunt Carnivores in packs)
- Added pack fleeing behavior for Carnivores when outnumbered
- Renamed Carnivore symbol from 'W' to 'C' for clarity

## v1.0: Initial Console Predator-Prey
**Date:** January 2025
- Created basic console-based simulation with grid display
- Implemented simple Herbivore and Carnivore species with random movement
- Basic energy system with consumption and depletion mechanics
- Foundation for complex ecosystem simulation
