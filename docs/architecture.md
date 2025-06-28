# Project Architecture

## Overview
This ecosystem simulation uses a **Data-Oriented Design (DOD)** architecture with system-based processing for high performance and scalability. The project features a sophisticated graphics rendering system with interactive camera controls, comprehensive UI management, and professional-grade animation interpolation.

## Core Architecture Components

### EntityManager (SoA Pattern)
- **Structure of Arrays** design for cache-efficient data access
- Stores all entity attributes in separate vectors
- Manages entity lifecycle with fast swap-and-pop deletion
- Contains 25+ different attributes per entity including animation state (`prev_x`, `prev_y`)
- Optimized for parallel processing across multiple systems
- **Animation State Management**: Properly initializes previous positions for newly created entities to prevent visual artifacts

### System-Based Processing
The simulation uses 6 main systems that process entities in a specific order, each implemented as a separate module:

0. **Animation System** (`AnimationSystem.cpp/.h`) - Captures position state before simulation updates (parallelized)
   - Stores current positions as previous positions for next frame's interpolation
   - Ensures smooth visual transitions between simulation turns
   - Prevents newly created entities from appearing to "teleport" from origin
1. **AI System** (`AISystem.cpp/.h`) - Determines entity behavior states and targets (parallelized)
   - Handles target validation and state transitions
   - Manages herd bonus calculations in thread-safe manner
   - Implements pack hunting logic with proper ally detection
2. **Movement System** (`MovementSystem.cpp/.h`) - Handles entity movement with diagonal pathfinding (parallelized)
   - Focuses purely on movement without state modification
   - Clears invalid targets and switches to wandering when needed
   - Maintains separation from AI decision-making
3. **Action System** (`ActionSystem.cpp/.h`) - Processes combat, resource consumption, state transitions (sequential)
   - Thread-sensitive operations requiring sequential execution
   - Handles all entity interactions and state changes
4. **Metabolism System** (`MetabolismSystem.cpp/.h`) - Handles aging, hunger, health regeneration (parallelized)
   - Implements comprehensive aging penalties for stats after prime age
   - Uses species-specific constants for realistic lifecycle dynamics
   - Manages health regeneration and energy consumption
5. **Reproduction System** (`ReproductionSystem.cpp/.h`) - Manages entity reproduction (sequential)
   - Handles entity creation and population dynamics
   - Properly initializes animation state for newly created entities

### Graphics & Rendering System
- **SFML-based rendering** with texture management and sprite drawing
- **Interactive camera system** with smooth zoom, pan, and reset functionality
- **Enhanced UI system** with sprite-based counters and semi-transparent backgrounds
- **Asset management** for textures, fonts, and audio files
- **View separation** between world rendering (camera view) and UI overlay (fixed view)
- **Animation Interpolation**: Smooth position interpolation with quadratic easing for professional visual quality

### Animation System Architecture
- **Position State Capture**: Stores entity positions before each simulation update
- **Interpolation Engine**: Calculates smooth transitions between previous and current positions
- **Quadratic Easing**: Natural acceleration/deceleration curves for realistic movement
- **Performance Optimization**: Parallel position capture with minimal overhead
- **Entity Lifecycle Support**: Proper animation state for newly created and destroyed entities
- **Frame Rate Independence**: Smooth 60fps animation regardless of simulation speed

### Camera System Architecture
- **Smooth zoom** with configurable speed and intelligent bounds
- **Pan constraints** that prevent viewing beyond simulation boundaries
- **Zoom-to-cursor** functionality for intuitive navigation
- **Automatic reset** to optimal world-fitting view
- **Delta-time based** smooth transitions for professional feel

### Spatial Partitioning
- Grid-based spatial partitioning for O(1) neighbor queries
- Enables efficient perception and interaction systems
- Dramatically improves performance over O(N) brute force
- Updated each turn to maintain spatial coherence

### Multithreading
- Uses **OpenMP** for parallelizing computationally heavy systems
- AI, Movement, and Metabolism systems run in parallel
- Thread-sensitive systems (Action, Reproduction) remain sequential
- Proper synchronization and data dependency management

## Data Flow
```
Environment Update → Spatial Grid Update → AI System (||) → Movement System (||) → Action System → Metabolism System (||) → Cleanup → Reproduction System → Camera Update → Rendering
```

## User Interaction Flow
```
Input Events → Event Handler → Camera System Updates → Rendering System → Display
```

## Directory Structure
- `src/core/` - Core simulation components (World, EntityManager, Random)
- `src/systems/` - Individual system implementations with dedicated files for each system
  - `AISystem.cpp` - AI decision-making and behavior management
  - `MovementSystem.cpp` - Entity movement and pathfinding logic
  - `ActionSystem.cpp` - Combat and resource interaction processing
  - `MetabolismSystem.cpp` - Aging, hunger, and health management
  - `ReproductionSystem.cpp` - Entity reproduction and lifecycle
- `src/resources/` - Resource and environment management
- `src/graphics/` - Rendering, camera system, and UI management
- `include/` - Header files organized by component type
  - `include/systems/` - Individual system headers with clear interfaces
- `external/` - Third-party libraries (SFML)
- `assets/` - Game assets (textures, fonts, audio)
- `build/` - Compiled object files and build artifacts

## Performance Considerations
- **Data-oriented design** maximizes cache efficiency with Structure of Arrays (SoA) pattern
- **Parallel processing** utilizes multiple CPU cores via OpenMP for AI, Movement, and Metabolism systems
- **Dynamic spatial partitioning** automatically optimizes cell sizes based on world dimensions and entity density
- **View frustum culling** eliminates rendering of off-screen content, providing 80-95% performance improvement for large worlds
- **Optimized asset pipeline** uses appropriately-sized textures with fixed scaling to minimize GPU memory usage
- **Efficient target validation** prevents processing of dead or invalid entities across all systems
- **Memory pre-allocation** reduces dynamic allocations in spatial grid and rendering systems
- **Smooth interpolation** provides responsive user experience without performance cost

## Recent Performance Optimizations (v2.8-v2.10)
- **Advanced AI Behavior Systems**: Implemented distance-based target selection and energy-to-distance optimization
- **Enhanced Food Seeking**: Sophisticated foraging algorithms with proximity bonuses and realistic travel limitations
- **Combat System Reliability**: Euclidean distance-based combat ranges compatible with diagonal movement patterns
- **Starvation Mechanics**: Explicit energy-based death system for realistic survival pressure
- **Consistent Threat Detection**: Full sight radius threat detection across all predator-prey interactions
- **Modular System Architecture**: Split monolithic systems into dedicated modules for better maintainability
- **Race Condition Fixes**: Resolved critical threading issues in herd bonus calculations and pack hunting
- **Aging System Implementation**: Added comprehensive aging penalties with species-specific decline rates
- **Movement/AI Separation**: Improved system boundaries to prevent state conflicts in parallel execution
- **Enhanced Target Validation**: Robust validation across all systems prevents processing of invalid entities
- **Rendering Pipeline**: Implemented view frustum culling and optimized texture scaling
- **Spatial Grid**: Added auto-calculating optimal cell sizes and memory pre-allocation
- **Asset Management**: Resized textures from 1024×1024 to optimal sizes, reducing memory by 95%
- **Behavioral Systems**: Fixed infinite loops and improved target validation for more efficient processing
- **Memory Efficiency**: Reduced allocation overhead and improved cache locality throughout the codebase
