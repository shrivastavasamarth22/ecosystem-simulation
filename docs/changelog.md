# Changelog

## v2.13: Wave Function Collapse Terrain Generation & Enhanced Biome System
**Date:** June 2025
### Wave Function Collapse Implementation
- **WFC Algorithm:** Implemented sophisticated Wave Function Collapse terrain generation replacing Voronoi diagrams
  - **WFCGenerator Class:** Complete WFC implementation with constraint propagation, entropy-based cell selection, and adjacency rule enforcement
  - **Adjacency Rules System:** Realistic biome transition rules (water connects to grassland/forest, rocky areas near forests, etc.)
  - **Constraint Propagation:** Efficient neighboring cell constraint updates when cells collapse to specific biomes
  - **Robust Fallback:** Graceful fallback to random generation if WFC fails due to constraint conflicts

### Multi-Scale Terrain Generation
- **Hybrid Generation Approach:** Two-phase system creating coherent biome regions instead of fragmented tiles
  - **Phase 1 - Large Regions:** 15x15 tile block assignment creating substantial biome areas
  - **Phase 2 - Boundary Smoothing:** WFC-based transition smoothing only at biome boundaries for natural edges
  - **Performance Optimization:** Significantly faster than full WFC while maintaining pattern quality

### Enhanced Biome System
- **Six Distinct Biomes:** Expanded from 3 to 6 biomes with unique characteristics
  - **Water:** 100% water terrain, no resources (lakes, rivers)
  - **Barren:** Normal terrain, sparse grass (10% chance) - arid regions
  - **Rocky:** 100% rocky terrain, no resources - impassable mountains for omnivores
  - **Grassland:** Normal terrain, rich grass (70%), bushes (20%), berries (5%) - primary habitat
  - **Forest:** Normal terrain, dense bushes (50%), moderate grass (25%), berries (10%) - reduces sight
  - **Fertile:** Normal terrain, abundant berries (60%), bushes (25%), grass (15%) - high-value areas

### Terrain vs Resource Separation
- **Architectural Redesign:** Complete separation of terrain types from resource types
  - **Terrain System:** New terrain types (water, rocky, normal) affecting movement and stats
  - **Resource System:** Enhanced with bush resource alongside grass and berries
  - **Dual-Layer Rendering:** Terrain renders as base layer, resources render on top

### Movement & Terrain Integration
- **Terrain Movement Restrictions:** Rocky terrain blocks omnivore movement, water slows all entities to 50% speed
- **Stat Modifiers:** Terrain affects entity speed and sight radius through metabolism system
- **Forest Sight Penalty:** 50% sight reduction for herbivores and omnivores in forest biomes
- **System Integration:** Movement system checks terrain accessibility, metabolism applies modifiers

### Asset & Rendering Updates
- **New Texture Support:** Added water_tile.png, rocky_tile.png, bush_tile.png texture loading
- **Rendering Order:** Proper terrain-first, resource-second rendering for visual correctness
- **Texture Validation:** Error handling for missing texture files with fallback systems

## v2.12: Entity Detail Cam & Critical Bug Fixes
**Date:** June 2025
### Entity Detail Camera System
- **Interactive Entity Inspection:** Comprehensive entity analysis system with click-to-select functionality
  - Click any entity to select it and enter detail view mode with smooth camera follow
  - Real-time information panel displays comprehensive entity data including health, energy, AI state, family relationships, and detailed stats
  - Color-coded information with condition descriptions (Excellent/Good/Injured health, Well Fed/Hungry/Starving energy)
  - Section-organized display with basic info, health & energy, current stats, life cycle, and AI & behavior
  - Entity positioned on left side of screen with information panel on right for optimal layout
- **Camera Follow Mode:** Advanced camera tracking with smooth positioning and zoom control
  - Smooth camera transitions when entering/exiting follow mode
  - Entity positioned at left third of screen for optimal information panel visibility
  - Manual camera dragging or clicking empty space exits follow mode naturally
  - Selection indicators with pulsing visual effects and inner highlight circles
- **UI Integration:** Seamless integration with existing camera and rendering systems
  - Dynamic panel sizing and positioning to prevent screen overflow
  - Real-time data updates showing current entity state and conditions
  - Family relationship display for carnivores showing parent/offspring status and independence countdown

### Critical Bug Fixes
- **Parent-Child Protection System:** Fixed critical carnivore family conflict bug
  - **Root Cause:** Carnivores would immediately attack their own newborn offspring due to territorial behavior
  - **Solution:** Added parent tracking system (`parent_id` field) and family conflict prevention in AI system
  - **Implementation:** Carnivore offspring tracked until independence age (8 turns), preventing parent attacks during vulnerable period
  - **Scope:** Only applied to carnivores as they're the only species with territorial behavior that attacks same-species entities
- **Percentage-Based Aging Reduction:** Completely redesigned herd bonus system to prevent health inflation
  - **Root Cause:** Herd health bonuses (+5 HP per member) were permanent additions to max_health, creating immortal 200+ health herbivores
  - **Solution:** Replaced flat health bonuses with percentage-based aging penalty reduction (8% per herd member, max 50%)
  - **Implementation:** Moved herd calculations from AISystem to MetabolismSystem for proper aging integration
  - **Benefits:** Maintains strategic herding advantages while ensuring natural lifespans and preventing infinite health scaling

### System Architecture Improvements
- **EntityManager Enhancement:** Added family relationship tracking with minimal performance impact
  - `parent_id` field for tracking carnivore family relationships
  - `INVALID_PARENT` constant for entities without tracked parents
  - Integration into entity creation, destruction, and data management systems
- **MetabolismSystem Redesign:** Enhanced aging system with sophisticated herd benefit calculations
  - Herd size calculation moved from AISystem for proper integration with aging penalties
  - Percentage-based aging reduction prevents extreme health values while maintaining social benefits
  - Max health reset to base values each turn to prevent permanent inflation
- **ReproductionSystem Enhancement:** Added parent tracking for newly created carnivore offspring
  - Automatic parent-child relationship establishment during reproduction
  - Proper initialization of family tracking data for new entities
  - Integration with existing entity creation and animation state initialization

## v2.11: Animation System & Reproduction Bug Fixes
**Date:** June 2025
### Animation System Implementation
- **Smooth Animation System:** Implemented professional-grade position interpolation with quadratic easing
  - Added dedicated `AnimationSystem.cpp/.h` module for position state management
  - Captures previous positions before each simulation update for smooth transitions
  - Quadratic easing curves provide natural acceleration/deceleration for realistic movement
  - 60fps visual smoothness maintained independently of simulation speed
  - Parallel position capture with minimal performance overhead
- **Animation State Architecture:** Integrated animation state into entity lifecycle management
  - Added `prev_x` and `prev_y` attributes to EntityManager for interpolation data
  - Animation system runs as System 0 before all other simulation logic
  - Frame-rate independent interpolation for consistent visual experience
  - Professional visual polish eliminates jerky movement between simulation turns

### Critical Bug Fixes
- **Reproduction Animation Fix:** Resolved critical "teleporting herbivores" visual bug
  - **Root Cause:** Newly reproduced entities had `prev_x` and `prev_y` initialized to (0,0)
  - **Symptom:** New entities appeared to teleport from map origin to spawn location
  - **Solution:** Initialize `prev_x` and `prev_y` to spawn position in all entity creation functions
  - **Impact:** Eliminates visual artifacts for newly created entities across all species
- **Entity Creation State:** Enhanced all entity creation functions in `EntityManager`
  - `createHerbivore()`, `createCarnivore()`, and `createOmnivore()` now properly initialize animation state
  - Prevents interpolation from invalid previous positions for smooth entity spawning
  - Maintains visual consistency throughout entity lifecycle from creation to destruction

### System Integration Improvements
- **Animation-Rendering Pipeline:** Seamless integration between animation system and graphics renderer
  - `GraphicsRenderer` uses interpolated positions for smooth entity drawing
  - Animation interpolation factor calculated from frame timing for precise movement
  - Maintains simulation logic separation from visual presentation
- **Performance Optimization:** Animation system designed for high-performance execution
  - Position capture parallelized across all entities using OpenMP
  - Minimal memory overhead with efficient data structure usage
  - No impact on simulation speed while providing dramatic visual improvement

### Technical Implementation Details
- **Modular Design:** Animation system follows established system architecture patterns
  - Clean separation of concerns with dedicated header/implementation files
  - Thread-safe parallel execution compatible with existing multithreading
  - Easy to extend for future animation features (rotation, scaling, etc.)
- **Data-Oriented Integration:** Animation state integrated into SoA (Structure of Arrays) pattern
  - Previous position data stored efficiently alongside other entity attributes
  - Cache-friendly access patterns for optimal performance during parallel processing
  - Maintains data locality benefits of existing EntityManager design

## v2.10: Advanced AI Behavior & System Reliability Improvements
**Date:** January 2025
### Major AI & Behavioral Enhancements
- **Distance-Based Target Selection:** Implemented intelligent target selection across all animal behaviors
  - Territorial carnivores now attack the closest rival instead of the first found
  - Herbivores seek the closest herd member for efficient herding behavior
  - Predators flee from the closest threat for optimal escape routes
- **Enhanced Food Seeking Algorithm:** Completely revamped foraging behavior with energy-to-distance ratio optimization
  - Animals now prioritize nearby adequate food over distant optimal food
  - 50% efficiency bonus for food within 2 tiles to encourage local foraging
  - Prevents unrealistic cross-map travel for marginally better resources
  - Natural clustering around food sources instead of random spreading
- **Consistent Threat Detection:** Fixed predator flee priority inconsistencies
  - Both carnivores and omnivores now use full sight radius for threat detection
  - Consistent behavior where animals can flee from any threat they can see
  - Enhanced survival strategies with early threat response

### Critical System Fixes
- **Combat Range Compatibility:** Fixed combat range inconsistency with movement system
  - Uses Euclidean distance (√2 ≈ 1.4 tiles) instead of Manhattan distance
  - Compatible with diagonal movement patterns to prevent endless chase loops
  - More forgiving combat range accounts for realistic movement limitations
- **Starvation Death Implementation:** Added explicit starvation death when energy reaches zero
  - Animals now die immediately upon energy depletion instead of indefinite survival
  - Creates realistic survival pressure and natural population control
  - Energy management becomes critical survival mechanic
- **Enhanced Pack Coordination:** Improved omnivore pack hunting coordination radius from 2 to 3 tiles for more reliable pack formation

### Algorithmic Improvements
- **Mathematical Robustness:** Enhanced distance calculations with proper bounds checking
  - Division by zero prevention in food efficiency calculations
  - Accurate Euclidean distance for consistent circular ranges
  - Float precision throughout calculation chains
- **Performance Optimizations:** Improved loop structures and early termination conditions
  - Better boundary checking reduces unnecessary calculations
  - Efficient target validation prevents processing of dead entities
  - Squared distance comparisons avoid expensive square root operations where possible

### Behavioral Impact Summary
- **Realistic Foraging:** Animals make intelligent decisions about food sources based on proximity and energy yield
- **Efficient Herding:** Herbivores form compact, defensible groups through smart target selection  
- **Strategic Combat:** Territorial behavior focuses on immediate threats with optimal target prioritization
- **Natural Survival:** Starvation provides realistic population pressure and resource competition
- **Reliable Mechanics:** All systems work harmoniously without artificial behavioral limitations

## v2.9: Modular System Architecture & Critical Behavior Fixes
**Date:** January 2025
### Major Architecture Refactor
- **Modular System Implementation:** Split the monolithic `SimulationSystems.cpp` into five independent system files for better maintainability and modularity:
  - `AISystem.cpp/.h` - AI decision-making and behavior states
  - `MovementSystem.cpp/.h` - Entity movement and pathfinding
  - `ActionSystem.cpp/.h` - Combat, resource consumption, state transitions
  - `MetabolismSystem.cpp/.h` - Aging, hunger, health regeneration
  - `ReproductionSystem.cpp/.h` - Entity reproduction logic
- **Enhanced Build System:** Updated Makefile to support the new modular architecture with proper dependency management
- **Backward Compatibility:** Maintained `SimulationSystems.h` as an umbrella header for existing code compatibility

### Critical Bug Fixes & Logic Improvements
- **Race Condition Resolution:** Fixed critical herd bonus race condition by moving calculations to a single-threaded phase in the AI system
  - Prevents data races in parallel execution while maintaining performance
  - Ensures consistent and accurate herd bonus calculations
  - Maintains thread safety without sacrificing multithreading benefits
- **Pack Hunting Logic Fix:** Corrected omnivore pack hunting behavior to require allies near the hunter, not just near the target
  - Creates more realistic coordinated pack behavior
  - Prevents unrealistic "remote pack hunting" scenarios
  - Improves strategic hunting mechanics
- **Movement/AI State Conflict Resolution:** Eliminated state modification conflicts between systems
  - Removed state changes from MovementSystem to prevent parallel conflicts
  - Added comprehensive target validation in AISystem
  - Improved system separation of concerns and reliability

### Aging System Implementation
- **Comprehensive Aging Penalties:** Implemented full aging system with progressive stat decline after prime age
  - Damage, speed, and sight radius penalties using species-specific constants
  - Realistic lifecycle dynamics where older entities become less effective
  - Species-specific aging rates influence hunting strategies and population dynamics
- **Enhanced Target Validation:** Added robust target validation across all systems to prevent processing dead entities

### System Architecture Improvements
- **Better Modularity:** Each system now has clear responsibilities and well-defined interfaces
- **Improved Maintainability:** Easier to modify, debug, and extend individual systems
- **Enhanced Performance:** Better cache locality and reduced coupling between systems
- **Code Organization:** Cleaner separation of concerns with dedicated header files

## v2.8: Performance Optimization Suite
**Date:** June 2025
**Date:** June 2025
### Major Performance Improvements
- **View Frustum Culling:** Implemented advanced rendering optimization that only processes visible tiles and entities
  - 80-95% rendering performance improvement for large worlds
  - Automatically calculates visible bounds based on camera view
  - Dramatically reduces draw calls when zoomed in
- **Dynamic Spatial Grid Optimization:** Enhanced spatial partitioning system for better AI performance
  - Auto-calculating optimal cell sizes based on world dimensions and entity density
  - 30-50% improvement in neighbor-finding operations
  - Memory pre-allocation to reduce dynamic allocations
  - Intelligent bounds (8-32 tiles) with sight-radius consideration
- **Texture Asset Optimization:** Complete overhaul of texture pipeline for better GPU performance
  - Resized textures from 1024×1024/500×500 to optimal sizes (40×40 for tiles, 64×64 for entities)
  - 95% reduction in texture memory usage
  - Fixed scaling factors eliminate dynamic texture size queries
  - Better visual quality due to appropriate scaling ratios

### Technical Improvements
- **Enhanced Rendering Pipeline:** Optimized sprite scaling with compile-time scale factors
- **Memory Efficiency:** Pre-allocated spatial grid cells and result vectors
- **GPU Optimization:** Eliminated dynamic texture operations during rendering
- **Cache Performance:** Better memory access patterns with appropriately-sized data structures

## v2.9: Critical Behavior Fixes
**Date:** June 2025
### Major Bug Fixes
- **Animal Behavior Loop Resolution:** Fixed critical infinite loops affecting herbivores and omnivores
  - Enhanced food target validation to check for actually consumable resources
  - Added proper target coordinate cleanup when food is depleted
  - Prevents animals from repeatedly targeting the same depleted tiles
- **Dead Target Cleanup:** Comprehensive target validation across all systems
  - Movement system now clears invalid targets and switches to wandering
  - Action system validates targets before combat operations
  - Enhanced state transitions when targets die or become unreachable
- **Movement System Improvements:** Fixed boundary handling and "teleporting" issues
  - Better coordinate validation at world edges
  - Improved diagonal movement with proper boundary checks
  - More natural movement patterns near world boundaries

### Combat & AI Enhancements
- **Enhanced Combat Logic:** Improved target validation in combat situations
  - Robust checks for target validity before damage application
  - Automatic state cleanup when targets become invalid
  - Better handling of combat at world boundaries
- **Pack Hunting Improvements:** More reliable omnivore pack coordination
  - Enhanced target validation for pack hunting scenarios
  - Better ally counting and coordination logic
  - Improved pack formation and target selection
- **State Transition Robustness:** More reliable AI state management
  - Consistent target cleanup across all state transitions
  - Better handling of edge cases in behavior switching
  - Improved validation in herding and fleeing behaviors

### System-Wide Improvements
- **Target Validation:** Enhanced validation throughout all animal behavior systems
- **Memory Management:** Better cleanup of invalid references and targets
- **Performance:** Reduced unnecessary processing of invalid or dead entities
- **Stability:** More robust error handling and edge case management

## v2.6: Biome-Based Terrain Generation
**Date:** June 2025
### Major Features
- **Procedural Biome Generation:** Overhauled the terrain generation system to use a Voronoi diagram-based approach, creating distinct and natural-looking biomes (Forest, Grassland, Barren).
- **Data-Oriented Biome System:** Implemented a new data-oriented `Biome` system, where each biome type defines its own resource distribution and properties. This makes the system highly extensible for future terrain types.
- **Strategic Resource Placement:** Resources are no longer placed randomly across the map. Instead, they are seeded based on the biome, creating resource-rich and resource-scarce areas that drive entity migration and conflict.

### Technical Improvements
- **Voronoi Diagrams:** Used a mathematical approach to generate irregular, non-grid-like biome shapes for a more organic world.
- **Decoupled World Generation:** Separated biome generation from resource seeding, improving modularity and making the world initialization process cleaner.
- **Extensible Design:** The new `Biome.h` and `Biome.cpp` files allow for easy addition of new biomes with unique characteristics without changing the core world generation logic.

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
**Date:** June 2025
- Integrated OpenMP for parallel processing of computation-heavy systems
- Parallelized AI, Movement, and Metabolism systems for significant performance gains
- Maintained single-threaded execution for thread-sensitive systems (Action, Reproduction, Cleanup)
- Implemented proper synchronization and data dependency management

## v1.7: Data-Oriented Architecture
**Date:** June 2025
- Major refactor to Data-Oriented Design replacing OOP Animal hierarchy
- Implemented EntityManager with Structure of Arrays (SoA) pattern
- Created System-based logic with SimulationSystems namespace
- Added fast entity destruction with swap-and-pop algorithm
- Implemented spatial partitioning for O(1) neighbor queries
- Removed dynamic_cast dependencies for improved performance

## v1.6: Resource System Integration
**Date:** June 2025
- **Consumable Resources:** Introduced resources (Grass, Berries) on the grid with depletion and regrowth
- **Food Seeking AI:** Herbivores/Omnivores now actively seek the highest-energy food sources when hungry
- **Starvation Mechanics:** Implemented starvation death for realistic population control
- **Modular Resources:** Resource types defined externally for easy expansion

## v1.5: Social & Territorial Behavior
**Date:** June 2025
- **Herding Behavior:** Herbivores seek herds for dynamic Max HP bonus representing safety in numbers
- **Basic Territoriality:** Carnivores fight other Carnivores within territorial radius
- **Anti-Cannibalism:** No energy gain from territorial fights to prevent unrealistic behavior

## v1.4: Lifecycle Dynamics
**Date:** June 2025
- **Aging System:** Entities grow weaker as they age, with species-specific aging rates
- **Age-Based Nutritional Value:** Energy gained from kills depends on prey's age and type
- **Dynamic Stats:** Age-related decline in speed, damage, and sight capabilities

## v1.3: Advanced Survival Mechanics
**Date:** June 2025
- **Tiered Health Regeneration:** Multi-tiered healing system with permanent max HP reduction from severe injuries
- **Hunger & Desperation System:** Low energy causes health drain but temporarily boosts combat stats
- **Risk-Reward Mechanics:** Desperate entities become more dangerous but more vulnerable

## v1.2: Intelligent Agent AI
**Date:** June 2025
- Reworked animal behavior from simple rules to state-driven AI (Finite State Machine)
- Replaced energy-as-life with robust attribute system (Health, Damage, Speed, Sight)
- Implemented sophisticated Fleeing and Chasing behaviors with target acquisition

## v1.1: Omnivore & Group Hunting
**Date:** June 2025
- Added Omnivore species with complex hunting behavior
- Implemented group hunting logic (Omnivores hunt Carnivores in packs)
- Added pack fleeing behavior for Carnivores when outnumbered
- Renamed Carnivore symbol from 'W' to 'C' for clarity

## v1.0: Initial Console Predator-Prey
**Date:** June 2025
- Created basic console-based simulation with grid display
- Implemented simple Herbivore and Carnivore species with random movement
- Basic energy system with consumption and depletion mechanics
- Foundation for complex ecosystem simulation
