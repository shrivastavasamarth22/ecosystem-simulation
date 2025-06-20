# Intelligent Ecosystem Simulation

A C++ simulation that models complex predator-prey dynamics using **state-driven intelligent agents** in a dynamic 2D world.

## Overview

This project simulates a living ecosystem where each creature is an autonomous agent governed by a sophisticated set of attributes and an AI state machine. This goes far beyond simple rules, creating a "digital petri dish" where complex and unpredictable **emergent behaviors** arise from the interactions of the animals and their environment.

Animals in this simulation perceive their world, form cooperative herds, get hungry, feel desperation, suffer lasting injuries from fights, grow old and weak, and provide different nutritional value based on their age. This creates a deeply layered and realistic model of population dynamics and the struggle for survival.

## Core Features

- **Intelligent Agent-Based AI:** Animals perceive their surroundings and react with purpose using a Finite State Machine (`WANDERING`, `FLEEING`, `CHASING`, `PACK_HUNTING`, `HERDING`).
- **Dynamic Attribute System:** Every animal has unique stats for **Health, Damage, Speed, and Sight**.
- **Complex Survival Mechanics:**
    - **Hunger & Desperation:** Low energy triggers a high-risk, high-reward "desperation mode," boosting an animal's stats at the cost of its health.
    - **Tiered Health Regeneration:** Animals can recover from damage over time, but severe injuries permanently lower their maximum health cap across four different tiers.
    - **Aging System:** As animals grow older, their base stats (like speed and damage) decline, making them more vulnerable. This effect is most pronounced in high-energy predators.
    - **Age-Based Nutritional Value:** The energy gained from a kill depends on the age of the prey, making older, weaker animals a lower-reward target.
- **Advanced Social & AI Behaviors:**
    - **Herding Behavior:** Herbivores actively seek each other out to form herds. Being in a herd provides a dynamic **Max HP bonus** to its members, making them much more resilient to predators.
    - **Strategic Hunting:** Predators will hunt lone members of other predator species if easier prey is unavailable.
    - **Pack Dynamics:** Omnivores form packs to hunt powerful Carnivores, and Carnivores are smart enough to flee from these groups.
- **Real-time ASCII Visualization:** The world is rendered in the console with live-updating population counts.
- **Extensible Object-Oriented Design:** The class structure makes it easy to add new species or modify existing behaviors.

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
    - **Health Regeneration:** Implemented a multi-tiered system where animals can heal from damage, but severe wounds permanently lower their maximum HP.
    - **Hunger & Desperation System:** Introduced a high-risk, high-reward state where low energy causes health drain but provides temporary stat boosts.
- **v3.0: Lifecycle & Ecosystem Balancing**
    - **Aging System:** Animals now grow weaker as they age, with base stats declining over time. The effect is most pronounced for Carnivores and least for Herbivores.
    - **Age-Based Nutritional Value:** The energy a predator gains from a kill is now dependent on the age of the prey, making older, weaker animals less nutritious and balancing the aging mechanic.
- **v3.5: Cooperative Social Behavior**
    - **Herding Behavior:** Herbivores now possess a `HERDING` AI state. Lone herbivores actively seek out others to form groups.
    - **Herd Health Bonus:** Members of a herd gain a dynamic bonus to their maximum and current HP based on the number of nearby friends, representing "safety in numbers."

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
g++ -std=c++17 -Wall -Wextra -g -o simulation main.cpp World.cpp Animal.cpp Herbivore.cpp Carnivore.cpp Omnivore.cpp
```

### Run the Simulation
```bash
./simulation
```

## Core Mechanics & AI Behavior

### Species-Specific AI

- **Herbivores (H):**
    - **Cooperative & Defensive:** Their primary survival strategy is forming herds. Lone individuals will actively seek out others. Being in a herd grants a significant, dynamic health bonus, making them much tougher to kill.
    - **Behavior:** `FLEEING` from predators is their top priority. If safe, their next goal is `HERDING`. If already in a group or unable to find one, they will `WANDER` and graze.

- **Carnivores (C):**
    - **Apex Predators:** High damage and speed make them lethal hunters. However, they are hit hard by the effects of aging and must be cautious of both Omnivore packs and large, healthy Herbivore herds.
    - **Behavior:** They will `CHASE` Herbivores or lone Omnivores. They will `FLEE` from an Omnivore pack.

- **Omnivores (O):**
    - **Opportunists & Pack Hunters:** The most complex AI. Their strength lies in their numbers and adaptability.
    - **Behavior:** They will `CHASE` lone Herbivores. If they spot a Carnivore, they will assess the situation: if enough allied Omnivores are nearby, they will switch to `PACK_HUNTING`; if outnumbered, they will `FLEE`.

## Configuration & Balancing

Initial population counts and world size can be modified in `main.cpp`. The core balance of the ecosystem is determined by the attribute constants defined at the top of each animal's implementation file (e.g., `Carnivore.cpp`). Tweaking these values will have a dramatic impact on the simulation's outcome.

## Class Hierarchy

The simulation uses a clear polymorphic hierarchy.
```
Animal (abstract base class)
 |
 +-- Herbivore
 +-- Carnivore
 +-- Omnivore
```
- The `Animal` base class provides the attribute framework, the `AIState` enum, lifecycle functions (`postTurnUpdate`), and the `getNutritionalValue` method.
- Derived classes implement the decision-making logic in `updateAI()`, execute actions in `act()`, and provide specific overrides for `applyAgingPenalties()` and `getNutritionalValue()`.

## License

This project is open source and available under the MIT License.

Copyright (c) 2025 Samarth Shrivastava