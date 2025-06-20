# Intelligent Ecosystem Simulation

A C++ simulation that models complex predator-prey dynamics using **state-driven intelligent agents** with features like health regeneration, hunger-driven desperation, and age-related decline in a 2D world.

## Overview

This project simulates a dynamic ecosystem where each creature is an autonomous agent governed by a set of attributes and an AI state machine. Unlike simple rule-based systems, animals in this simulation perceive their environment and make decisions based on their current state—whether they are wandering, chasing prey, fleeing from a threat, or hunting in a pack. Advanced features like a multi-tiered health regeneration system, a hunger mechanic that boosts stats in desperation, and age-related stat decline add significant depth and realism.

This attribute-driven design (Health, Damage, Speed, Sight) and state-based AI, coupled with nuanced survival mechanics, lead to complex and unpredictable **emergent behaviors**, creating a more realistic and engaging simulation of population dynamics.

## Features

- **Intelligent Agent-Based AI:** Animals perceive their surroundings and react with purpose.
- **Finite State Machine (FSM):** Each animal operates in a specific state (`WANDERING`, `FLEEING`, `CHASING`, `PACK_HUNTING`), dictating its actions.
- **Attribute-Driven System:** Every animal has unique stats for **Health, Damage, Speed, and Sight**, creating deep and tunable species differentiation.
- **Complex AI Behaviors:**
    - **Fleeing:** Herbivores and outnumbered animals will actively run from threats.
    - **Chasing:** Predators will lock onto and pursue prey, including Carnivores hunting lone/small groups of Omnivores.
    - **Pack Hunting:** Omnivores can form groups to take down stronger Carnivores.
- **Advanced Survival Mechanics:**
    - **Multi-Tier Health Regeneration:** Animals regenerate health if undamaged, but severe injuries permanently lower their maximum healing cap across four distinct tiers.
    - **Hunger System & Desperation Mode:** Low energy triggers temporary stat boosts (sight, speed, damage) at the cost of continuous health loss, simulating an adrenaline rush.
    - **Aging System:** Animals gradually weaken as they age, with base stats declining over time. This effect is most pronounced in Carnivores.
- **Real-time ASCII Visualization:** The world is rendered in the console, with live-updating population counts.
- **Extensible Object-Oriented Design:** The class structure makes it easy to add new species or modify existing behaviors.

## Project Structure

```
ecosystem-simulation/
├── main.cpp           # Entry point, simulation loop, and parameters
├── World.h/.cpp       # Manages the grid, all animals, and the update cycle
├── Animal.h/.cpp      # Abstract base class with core attributes, AI state, survival mechanics
├── Herbivore.h/.cpp   # Herbivore implementation (fleeing, grazing, aging)
├── Carnivore.h/.cpp   # Carnivore implementation (chasing, fleeing, aging)
├── Omnivore.h/.cpp    # Omnivore implementation (pack hunting, grazing, aging)
├── Makefile           # Build configuration
├── LICENSE            # MIT License
└── README.md          # This file
```

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

### Core Attributes
- **Base Stats:** `base_health` (implicitly `max_health` at birth), `base_damage`, `base_sight_radius`, `base_speed`. These are the animal's natural attributes and can decline with age.
- **Current Stats:** `current_damage`, `current_sight_radius`, `current_speed`. These are derived from base stats and can be temporarily boosted by the hunger system.
- **Health (HP):** An animal's life force. It dies when HP reaches 0.
- **Max Health:** The maximum HP an animal can have. Can be reduced by the regeneration cap after severe injury or potentially by aging.
- **Sight Radius:** The distance an animal can "see" to detect prey or predators (uses `current_sight_radius`).
- **Speed:** The number of tiles an animal can move per turn (uses `current_speed`).
- **Energy & Max Energy:** Used for survival cost, reproduction, and the hunger system.
- **Age:** Increments each turn and influences stat decline.

### Species-Specific AI

- **Herbivores (H):** Primarily `FLEEING` or `WANDERING` (grazing). Age slowly.
- **Carnivores (C):** `CHASE` Herbivores or vulnerable Omnivores, `FLEE` from Omnivore packs. Heavily affected by aging.
- **Omnivores (O):** `PACK_HUNTING` Carnivores, `CHASING` Herbivores, or `FLEEING` from stronger threats. Moderate aging effects. Also graze when wandering.

## Configuration & Balancing

Initial population counts and world size can be modified in `main.cpp`.
The core balance of the ecosystem is determined by the attribute constants (e.g., `HERBIVORE_HP`, `CARNIVORE_BASE_DMG`, reproduction energy percentages, age thresholds) defined at the top of each animal's implementation file. Tweaking these values will have a dramatic impact on the simulation's outcome.

## Termination Conditions

The simulation ends when:
1. One of the species is completely wiped out, causing the ecosystem to collapse.
2. The simulation reaches the maximum turn limit (2000 by default).

---

## Changelog

This section documents the major features added during the development of the simulation.

**Version 0.5: Aging System**
- **Feature:** Animals now experience age-related decline. As they grow older, their base stats (speed, damage, sight) gradually decrease.
- **Description:** A virtual `applyAgingPenalties()` method was added to the `Animal` class, called each turn in `postTurnUpdate()`. Derived classes (Herbivore, Carnivore, Omnivore) override this method to implement species-specific aging effects, with Carnivores being the most affected and Herbivores the least.
- **Thought Process:** To add more realism and a natural lifecycle end for animals beyond just being hunted or starving. This introduces population turnover and prevents "immortal" super-predators from dominating indefinitely. It also adds strategic depth, as older animals become less effective and more vulnerable.

**Version 0.4: Enhanced Reproduction & Carnivore AI**
- **Feature 1:** Standardized reproduction energy thresholds using percentages (`_REPRODUCE_ENERGY_PERCENTAGE`) for all species. Omnivores now also graze when wandering.
- **Description:** Similar to Herbivores, Carnivores and Omnivores now have explicit percentage-based energy requirements for reproduction, making balancing more consistent.
- **Thought Process:** To provide finer and more intuitive control over population growth rates for all species by using a common mechanism for reproduction triggers.

- **Feature 2:** Carnivores now hunt lone or small groups of Omnivores.
- **Description:** The Carnivore AI in `updateAI()` was modified. If no Herbivores are found, and nearby Omnivores do not constitute a pack threat (i.e., their group size is less than `OMNIVORE_PACK_THREAT_SIZE`), the Carnivore will target them. Energy gain from Omnivore kills was also added.
- **Thought Process:** To increase the interactivity between Carnivores and Omnivores and make Carnivores more opportunistic predators. This adds another layer to the food web and prevents Omnivores from being completely safe unless in a large pack.

**Version 0.3: Multi-Tier Health Regen & Herbivore Buff**
- **Feature 1:** Expanded Health Regeneration Cap to a 4-tier system.
- **Description:** The health regeneration cap is now more granular. Depending on the severity of an injury (current health percentage), an animal's maximum healable HP is capped at 100%, 90%, 75%, or 60% of its original max health.
- **Thought Process:** To make the consequences of injury more nuanced. Minor injuries can be fully recovered, while critical wounds leave permanent significant weaknesses, creating more varied individual animal states.

- **Feature 2:** Made it easier for Herbivores to reproduce.
- **Description:** Increased Herbivore energy gain from grazing, lowered their reproduction energy threshold (to 60% of max energy), and reduced their minimum age for reproduction.
- **Thought Process:** To address an imbalance where Herbivores were getting wiped out too quickly, thereby collapsing the ecosystem. Strengthening the base of the food pyramid is crucial for long-term simulation stability.

**Version 0.2: Health Regen & Hunger System**
- **Feature 1:** Implemented a Health Regeneration system.
- **Description:** Animals that avoid damage for a set number of turns begin to regenerate health. A 2-tier cap was initially introduced: if an animal dropped below 75% HP, its regeneration was capped at 75% of max HP; otherwise, it could heal to 100%.
- **Thought Process:** To allow animals to recover from non-fatal encounters, increasing survivability and making the ecosystem more resilient. The cap introduced the idea of lasting consequences from severe injuries.

- **Feature 2:** Implemented a Hunger System with "Desperation Mode."
- **Description:** Animals now have `base_` and `current_` stats. If an animal's energy drops below certain thresholds (50% and 30%), it enters a "hungry" or "desperate" state. In these states, it temporarily gains boosts to its `current_` damage, speed, and sight, but at the cost of losing health each turn due to starvation.
- **Thought Process:** To prevent animals (especially predators) from passively starving. This high-risk, high-reward mechanic makes hungry animals more dangerous and capable of a comeback, creating dramatic tension and a self-correcting element in the ecosystem.

**Version 0.1: Intelligent Agent Overhaul (Initial Major Refactor)**
- **Feature:** Replaced simple energy-as-life and random movement with a state-driven AI and new attributes.
- **Description:**
    - Introduced core attributes: Health (HP), Damage, Sight Radius, Speed. Energy became a separate resource for stamina/reproduction.
    - Implemented an AI State Machine (`WANDERING`, `FLEEING`, `CHASING`, `PACK_HUNTING`).
    - Animals now perceive their environment using `sight_radius` and change states accordingly.
    - Specific behaviors: Herbivores flee predators, Carnivores chase Herbivores, Omnivores hunt Herbivores and can pack-hunt Carnivores (or flee if outnumbered). Carnivores also flee Omnivore packs.
    - World update loop changed to a three-phase system: Perceive (updateAI) -> Act -> Post-Turn (metabolism, reproduction).
- **Thought Process:** To move beyond a simplistic simulation to one with more believable and emergent agent behaviors. The goal was to create more dynamic interactions and a more sustainable ecosystem by giving animals more intelligent ways to interact with each other and their environment. This was a foundational change that enabled all subsequent complex features.

**Version 0.0: Basic Predator-Prey Simulation**
- **Feature:** Initial implementation with Herbivores ('H') and Carnivores ('W' - Wolves).
- **Description:** Animals moved randomly. Herbivores gained energy over time, Carnivores hunted Herbivores based on proximity. Basic reproduction based on energy levels. Simulation was text-based on a 2D grid.
- **Thought Process:** To establish the basic framework of a predator-prey simulation using OOP principles, multiple files, and custom headers.

---

## License

This project is open source and available under the MIT License. See the `LICENSE` file for details.

Copyright (c) 2025 Samarth Shrivastava