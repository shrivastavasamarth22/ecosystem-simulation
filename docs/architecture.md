# Project Architecture

## Overview
This ecosystem simulation uses a **Data-Oriented Design (DOD)** architecture with system-based processing for high performance and scalability.

## Core Architecture Components

### EntityManager (SoA Pattern)
- **Structure of Arrays** design for cache-efficient data access
- Stores all entity attributes in separate vectors
- Manages entity lifecycle with fast swap-and-pop deletion
- Contains 25+ different attributes per entity

### System-Based Processing
The simulation uses 5 main systems that process entities in a specific order:

1. **AI System** - Determines entity behavior states and targets
2. **Movement System** - Handles entity movement based on AI decisions  
3. **Action System** - Processes combat, resource consumption, state transitions
4. **Metabolism System** - Handles aging, hunger, health regeneration
5. **Reproduction System** - Manages entity reproduction

### Spatial Partitioning
- Grid-based spatial partitioning for O(1) neighbor queries
- Enables efficient perception and interaction systems
- Dramatically improves performance over O(N) brute force

### Multithreading
- Uses **OpenMP** for parallelizing computationally heavy systems
- AI, Movement, and Metabolism systems run in parallel
- Thread-sensitive systems (Action, Reproduction) remain sequential

## Data Flow
```
Environment Update → Spatial Grid Update → AI System → Movement System → Action System → Metabolism System → Cleanup → Reproduction System
```

## Directory Structure
- `src/core/` - Core simulation components (World, EntityManager, Random)
- `src/systems/` - System implementations
- `src/resources/` - Resource and environment management
- `src/graphics/` - Rendering and visualization
- `include/` - Header files organized by component
- `external/` - Third-party libraries (SFML)
- `assets/` - Game assets (textures, fonts, audio)
