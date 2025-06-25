# Project Architecture

## Overview
This ecosystem simulation uses a **Data-Oriented Design (DOD)** architecture with system-based processing for high performance and scalability. The project features a sophisticated graphics rendering system with interactive camera controls and comprehensive UI management.

## Core Architecture Components

### EntityManager (SoA Pattern)
- **Structure of Arrays** design for cache-efficient data access
- Stores all entity attributes in separate vectors
- Manages entity lifecycle with fast swap-and-pop deletion
- Contains 25+ different attributes per entity
- Optimized for parallel processing across multiple systems

### System-Based Processing
The simulation uses 5 main systems that process entities in a specific order:

1. **AI System** - Determines entity behavior states and targets (parallelized)
2. **Movement System** - Handles entity movement with diagonal pathfinding (parallelized)
3. **Action System** - Processes combat, resource consumption, state transitions (sequential)
4. **Metabolism System** - Handles aging, hunger, health regeneration (parallelized)
5. **Reproduction System** - Manages entity reproduction (sequential)

### Graphics & Rendering System
- **SFML-based rendering** with texture management and sprite drawing
- **Interactive camera system** with smooth zoom, pan, and reset functionality
- **Enhanced UI system** with sprite-based counters and semi-transparent backgrounds
- **Asset management** for textures, fonts, and audio files
- **View separation** between world rendering (camera view) and UI overlay (fixed view)

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
- `src/systems/` - System implementations with parallelization
- `src/resources/` - Resource and environment management
- `src/graphics/` - Rendering, camera system, and UI management
- `include/` - Header files organized by component type
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

## Recent Performance Optimizations (v2.5-v2.6)
- **Rendering Pipeline**: Implemented view frustum culling and optimized texture scaling
- **Spatial Grid**: Added auto-calculating optimal cell sizes and memory pre-allocation
- **Asset Management**: Resized textures from 1024×1024 to optimal sizes, reducing memory by 95%
- **Behavioral Systems**: Fixed infinite loops and improved target validation for more efficient processing
- **Memory Efficiency**: Reduced allocation overhead and improved cache locality throughout the codebase
