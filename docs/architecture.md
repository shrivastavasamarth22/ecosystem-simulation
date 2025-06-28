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
   - No longer calculates herd health bonuses (moved to MetabolismSystem)
   - Implements pack hunting logic with proper ally detection
2. **Movement System** (`MovementSystem.cpp/.h`) - Handles entity movement with diagonal pathfinding (parallelized)
   - Focuses purely on movement without state modification
   - Clears invalid targets and switches to wandering when needed
   - Maintains separation from AI decision-making
3. **Action System** (`ActionSystem.cpp/.h`) - Processes combat, resource consumption, state transitions (sequential)
   - Thread-sensitive operations requiring sequential execution
   - Handles all entity interactions and state changes
4. **Metabolism System** (`MetabolismSystem.cpp/.h`) - Handles aging, hunger, health regeneration, herd benefits (parallelized)
   - Implements comprehensive aging penalties with herd-based aging reduction for herbivores
   - Calculates percentage-based aging reduction (8% per herd member, max 50%)
   - Resets max_health to base values to prevent health inflation
   - Uses species-specific constants for realistic lifecycle dynamics
   - Manages health regeneration and energy consumption
5. **Reproduction System** (`ReproductionSystem.cpp/.h`) - Manages entity reproduction with family tracking (sequential)
   - Handles entity creation and population dynamics
   - Tracks parent-child relationships for carnivores to prevent family conflicts
   - Properly initializes animation state for newly created entities

### Graphics & Rendering System
- **SFML-based rendering** with texture management and sprite drawing
- **Interactive camera system** with smooth zoom, pan, reset functionality, and entity follow mode
- **Entity Detail Camera System** with click-to-select functionality and real-time information panels
- **Enhanced UI system** with sprite-based counters, semi-transparent backgrounds, and detailed entity inspection
- **Selection Indicators** with pulsing visual effects and color-coded information display
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

## Recent Performance Optimizations & Major Features (v2.8-v2.12)
- **Entity Detail Camera System (v2.12)**: Interactive entity inspection with click-to-select, camera follow, and comprehensive real-time information panels
- **Family Protection System (v2.12)**: Parent-child relationship tracking for carnivores prevents unnatural family conflicts
- **Percentage-Based Aging Reduction (v2.12)**: Redesigned herd benefits from health inflation to sophisticated aging penalty reduction
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

## Entity Detail Camera System

### Interactive Entity Inspection
The Entity Detail Camera System provides comprehensive entity analysis through interactive selection:

- **Click-to-Select**: Click any entity to select it and enter detail view mode
- **Camera Follow**: Smooth camera tracking positions the selected entity on the left side of the screen
- **Real-time Information Panel**: Comprehensive entity data displayed on the right side including:
  - Basic information (type, position, age, life stage)
  - Health and energy status with condition descriptions
  - Current vs base stats showing aging/hunger effects
  - Life cycle information (prime age, nutritional value, aging penalties)
  - AI state and current target information
  - Family relationships (for carnivores showing parent/offspring status)

### Family Relationship System
- **Parent Tracking**: Carnivores track parent-child relationships using `parent_id` field
- **Independence System**: Young carnivores are protected from parents until independence age (8 turns)
- **Family Conflict Prevention**: AI system excludes family members from territorial targeting
- **UI Integration**: Entity detail panel shows family status and independence countdown

### Camera Integration
- **Smooth Follow Mode**: Camera smoothly tracks selected entity movement
- **Positioning Logic**: Entity positioned on left third of screen for optimal information panel layout
- **Exit Conditions**: Selection cleared by clicking empty space or manual camera dragging
- **Visual Indicators**: Pulsing selection circles with inner highlight effects

## Performance Considerations
- **Data-oriented design** maximizes cache efficiency with Structure of Arrays (SoA) pattern
- **Parallel processing** utilizes multiple CPU cores via OpenMP for AI, Movement, and Metabolism systems
- **Dynamic spatial partitioning** automatically optimizes cell sizes based on world dimensions and entity density
- **View frustum culling** eliminates rendering of off-screen content, providing 80-95% performance improvement for large worlds
- **Optimized asset pipeline** uses appropriately-sized textures with fixed scaling to minimize GPU memory usage
- **Efficient target validation** prevents processing of dead or invalid entities across all systems
- **Memory pre-allocation** reduces dynamic allocations in spatial grid and rendering systems
- **Smooth interpolation** provides responsive user experience without performance cost
