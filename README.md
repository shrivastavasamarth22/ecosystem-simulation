# Intelligent Ecosystem Simulation

A C++ simulation that models complex predator-prey and **resource dynamics** using **state-driven intelligent agents** in a dynamic 2D world.

## Overview

This project simulates a living ecosystem where each creature is an autonomous agent governed by a sophisticated set of attributes and an AI state machine. This goes far beyond simple rules, creating a "digital petri dish" where complex and unpredictable **emergent behaviors** arise from the interactions of the animals and their environment, including the crucial factor of limited, regenerating resources.

Animals in this simulation perceive their world, actively seek out food resources like grass, form cooperative herds for defense, get hungry, feel desperation, suffer lasting injuries from fights, grow old and weak, and provide different nutritional value based on their age. This creates a deeply layered and realistic model of population dynamics, resource competition, and the struggle for survival.

## Core Features

- **Intelligent Agent-Based AI:** Animals perceive their surroundings and react with purpose using a Finite State Machine (`WANDERING`, `FLEEING`, `CHASING`, `PACK_HUNTING`, `HERDING`, `SEEKING_FOOD`).
- **Dynamic Attribute System:** Every animal has unique stats for **Health, Damage, Speed, and Sight**. Stats can change dynamically based on hunger and aging.
- **Resource System:** The world contains consumable, regenerating resources (starting with "Grass"). Animals must actively seek out and consume these resources to gain energy. Over-consumption leads to depletion and localized scarcity, driving migration.
- **Complex Survival Mechanics:**
    - **Hunger & Desperation:** Low energy triggers a high-risk, high-reward "desperation mode," boosting an animal's stats at the cost of its health, forcing it to find food quickly.
    - **Tiered Health Regeneration:** Animals can recover from damage over time, but severe injuries permanently lower their maximum health cap across four different tiers, reflecting lasting injury.
    - **Aging System:** As animals grow older, their base stats (like speed and damage) decline, making them less effective hunters and more vulnerable to predators. This effect is most pronounced in high-energy predators.
    - **Age-Based Nutritional Value:** The energy gained by a predator from a kill is dynamically calculated based on the age of the prey, making older, weaker animals a lower-reward target and influencing hunting strategy.
- **Advanced Social & AI Behaviors:**
    - **Herding Behavior:** Herbivores actively seek each other out to form herds. Being in a herd provides a dynamic **Max HP bonus** to its members, representing "safety in numbers" and making them much more resilient to predators.
    - **Strategic Hunting:** Predators will hunt Herbivores (primary prey) or lone/small groups of Omnivores if the latter are not in a threatening pack size.
    - **Pack Dynamics:** Omnivores form packs to hunt powerful Carnivores, pooling their strength. Carnivores are smart enough to recognize and `FLEE` from a threatening Omnivore pack.
- **Real-time ASCII Visualization:** The world state, including animal locations and resource status, is rendered in the console with live-updating population counts.
- **Extensible Object-Oriented Design:** The class structure makes it easy to add new species, new resource types, or modify existing behaviors and mechanics.

## Changelog

- **v1.0: Initial Predator-Prey Simulation**
    - Created the basic world grid with two species: Herbivores (`H`) and Carnivores (`W`).
    - Animals moved randomly and had a simple energy-based life system.
- **v1.5: Omnivore & Group Hunting Update**
    - Added the `Omnivore` (`O`) species, which can both graze and hunt Herbivores.
    - Implemented basic group hunting logic: Omnivores can hunt Carnivores in packs, and vice-versa.
    - Refactored `World` class with a generic `getAnimalsNear<T>()` function.
- **v2.0: Intelligent Agent Overhaul (The "AI Update")**
    - Reworked the entire system from rule-based to state-driven AI (Finite State Machine).
    - Replaced the simple energy system with a robust attribute system: **Health (HP), Damage, Speed, and Sight Radius**.
    - Implemented intelligent AI behaviors: **Fleeing** from threats, **Chasing** prey, and more nuanced **Pack Hunting**.
- **v2.5: Advanced Survival Mechanics**
    - **Tiered Health Regeneration:** Implemented a multi-tiered system where animals can heal from damage, but severe wounds permanently lower their maximum HP cap.
    - **Hunger & Desperation System:** Introduced a high-risk, high-reward state where low energy causes health drain but provides temporary stat boosts.
- **v3.0: Lifecycle & Ecosystem Balancing**
    - **Aging System:** Animals now grow weaker as they age, with base stats declining over time. The effect is most pronounced for Carnivores and least for Herbivores.
    - **Age-Based Nutritional Value:** The energy a predator gains from a kill is now dependent on the age of the prey, making older, weaker animals less nutritious and balancing the aging mechanic.
- **v3.5: Cooperative Social Behavior**
    - **Herding Behavior:** Herbivores now possess a `HERDING` AI state. Lone herbivores actively seek out others to form groups.
    - **Herd Health Bonus:** Members of a herd gain a dynamic bonus to their maximum and current HP based on the number of nearby friends, representing "safety in numbers."
- **v4.0: Resource System Integration**
    - **Consumable Resources:** Introduced a resource layer (`Grass`) on the world grid.
    - **Resource Depletion & Regrowth:** Resources are consumed by Herbivores and Omnivores and regrow over time, introducing scarcity.
    - **Food Seeking AI:** Herbivores and Omnivores now have a `SEEKING_FOOD` state and will actively move towards resource-rich tiles when hungry.
    - **Starvation:** Hunger is now directly tied to resource availability; failure to find food leads to death.
    - **Modular Resource Definitions:** Resource types and their properties are defined externally (`Resource.h/.cpp`), allowing easy addition of new consumables.

## Building and Running

### Prerequisites
- A C++17 compatible compiler (e.g., g++, clang++)
- The `make` utility (recommended)

### Build with Make
```bash
# Clean previous builds and compile
make clean && make
```

### Build Manually
```bash
g++ -std=c++17 -Wall -Wextra -g -o simulation main.cpp World.cpp Animal.cpp Herbivore.cpp Carnivore.cpp Omnivore.cpp Resource.cpp Tile.cpp
```

### Run the Simulation
```bash
./simulation
```

## Core Mechanics & AI Behavior

### Species-Specific AI

- **Herbivores (H):**
    - **Cooperative & Defensive:** Their primary survival strategy is forming herds (`HERDING`). Lone individuals will actively seek out others (`SEEKING_HERD`). Being in a herd grants a significant, dynamic health bonus, making them much tougher to kill.
    - **Foraging:** When hungry and not fleeing, they will actively seek out and move towards tiles with grass (`SEEKING_FOOD`). If on a grass tile and not full on energy, they will consume it.
    - **Behavior Priority:** `FLEEING` > (`SEEKING_FOOD` if hungry) > `HERDING` > `WANDERING` (if in herd or cannot find one).

- **Carnivores (C):**
    - **Apex Predators:** High damage and speed make them lethal hunters. However, they are hit hard by the effects of aging and must be cautious of both Omnivore packs and large, healthy Herbivore herds. Their nutritional gain from kills depends on the prey's age.
    - **Behavior Priority:** `FLEEING` from Omnivore packs > `CHASING` Herbivores > `CHASING` lone/small groups of Omnivores > `WANDERING`.

- **Omnivores (O):**
    - **Opportunists & Pack Hunters:** The most complex AI. Their strength lies in their numbers and adaptability to eat different food sources.
    - **Foraging & Hunting:** When hungry, they prioritize seeking out Herbivores (`CHASING`). If no Herbivores are found, they will seek out grass (`SEEKING_FOOD`). If they spot a Carnivore, they will assess the situation: if enough allied Omnivores are nearby (`PACK_HUNTING`), they will hunt it; otherwise, they will `FLEE`. They also passively consume grass when on a resource tile if not full.
    - **Behavior Priority:** `FLEEING` from Carnivore packs > (`CHASING` Herbivores or `SEEKING_FOOD` Grass if hungry) > (`PACK_HUNTING` Carnivores if conditions met) > `WANDERING`.

## Configuration & Balancing

Initial population counts, world size, and simulation speed can be modified in `main.cpp`. The core balance of the ecosystem is determined by the attribute constants defined at the top of each animal's implementation file (e.g., `Carnivore.cpp`) and the resource properties in `Resource.cpp`. Tweaking these values will have a dramatic impact on the simulation's outcome, including population levels, migration patterns, and ecosystem stability.

## Class Hierarchy

The simulation uses a clear polymorphic hierarchy.
```
Animal (abstract base class)
 |
 +-- Herbivore
 +-- Carnivore
 +-- Omnivore
```
- The `Animal` base class provides the attribute framework (base and current stats), the `AIState` enum, core lifecycle functions (`postTurnUpdate` handling aging, hunger, regeneration), common movement methods, and the `tryConsumeResource` and `getNutritionalValue` methods.
- Derived classes implement the species-specific decision-making logic in `updateAI()`, execute unique actions in `act()`, and provide specific overrides for `applyAgingPenalties()` and `getNutritionalValue()` to define their aging profile and nutritional worth.

## License

This project is open source and available under the MIT License.

Copyright (c) 2025 Samarth Shrivastava