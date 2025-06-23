# Changelog

## v2.0: SFML Visual Overhaul Complete
- Added animal sprites
- Implemented Basic UI stats
- Added basic keyboard events as well as play/pause functionality for the sim.

## v1.9: SFML Visual Overhaul Beginning
- Replaced console drawing with a dedicated SFML graphical window.
- Implemented drawing of the world grid using textures for different tile/resource types.
- Added background music playback using SFML Audio.

## v1.8: Multithreading Implementation
- Integrated OpenMP. Parallelized key systems (AI, Movement, Metabolism). Maintained single-threaded execution for thread-sensitive systems (Action, Reproduction, Cleanup) with proper sequencing and synchronization.

## v1.7: Data-Oriented Architecture
- Major refactor to a Data-Oriented Design. Replaced OOP Animal hierarchy with `EntityManager` (SoA) and System-based logic (`SimulationSystems`). Implemented fast entity destruction and spatial partitioning (`World::spatial_grid`) for performance. Removed `dynamic_cast` dependencies for core logic.

## v1.6: Resource System Integration
- **Consumable Resources:** Introduced resources (Grass, Berries) on the grid with depletion and regrowth.
- **Food Seeking AI:** Herbivores/Omnivores now actively seek the highest-energy food sources when hungry. Starvation death implemented.
- **Modular Resources:** Resource types defined externally.

## v1.5: Social & Territorial Behavior
- **Herding Behavior:** Herbivores seek herds for a dynamic Max HP bonus.
- **Basic Territoriality:** Carnivores fight other Carnivores within a specific radius (no energy gain from cannibalism).

## v1.4: Lifecycle Dynamics
- **Aging System:** Entities grow weaker as they age, affecting stats.
- **Age-Based Nutritional Value:** Energy gained from kills depends on the prey's age and type.

## v1.3: Advanced Survival Mechanics
- **Tiered Health Regeneration:** Implemented a multi-tiered system where entities heal over time, with severe wounds permanently lowering their max HP cap.
- **Hunger & Desperation System:** Introduced a system where low energy causes health drain but temporarily boosts stats.

## v1.2: Intelligent Agent AI
- Reworked animal behavior from simple rules to a state-driven AI (FSM). Replaced simple energy-as-life with a robust attribute system (Health, Damage, Speed, Sight). Implemented basic Fleeing and Chasing behaviors.

## v1.1: Omnivore & Group Hunting
- Added the `Omnivore` (`O`) species. Implemented basic group hunting logic (Omnivores hunt Carnivores in packs, Carnivores flee Omnivore packs). Renamed Carnivore symbol to 'C'.

## v1.0: Initial Console Predator-Prey
- Created the basic console-based simulation with a grid and simple Herbivore (`H`) and Carnivore (`W`) species. Animals moved randomly with a simple energy system.
