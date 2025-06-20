#include "Herbivore.h"
#include "World.h"
#include "Carnivore.h"
#include "Omnivore.h"

// --- Balancing Constants ---
const int HERBIVORE_HP = 30;
const int HERBIVORE_BASE_DMG = 2;
const int HERBIVORE_BASE_SIGHT = 7;
const int HERBIVORE_BASE_SPEED = 1;
const int HERBIVORE_MAX_ENERGY = 50;
const int HERBIVORE_STARTING_ENERGY = 30; // Starts with a decent amount
const int HERBIVORE_REPRODUCE_ENERGY_COST = 15; // Cost to reproduce

// New/Adjusted constants for easier reproduction
const int HERBIVORE_GRAZE_ENERGY_GAIN = 2;      // << INCREASED: Gain more from grazing
const float HERBIVORE_REPRODUCE_ENERGY_PERCENTAGE = 0.65f; // << LOWERED: Need 60% of max energy
const int HERBIVORE_MIN_REPRODUCE_AGE = 3;       // << LOWERED: Can reproduce at age > 3


Herbivore::Herbivore(int x, int y)
    : Animal(x, y, 'H', HERBIVORE_HP, HERBIVORE_BASE_DMG, HERBIVORE_BASE_SIGHT, HERBIVORE_BASE_SPEED,
             HERBIVORE_MAX_ENERGY, HERBIVORE_STARTING_ENERGY) {}


void Herbivore::applyAgingPenalties() {
    if (age > 40) {
        base_speed = std::max(1, base_speed - 1);
    }

    if (age > 50) {
        base_damage = std::max(1, base_damage - 1);
    }

    base_speed = std::max(1, base_speed);
}

void Herbivore::updateAI(World& world) {
    target = nullptr; // Reset target each turn

    auto predators = world.getAnimalsNear<Carnivore>(x, y, getCurrentSightRadius());
    auto omni_predators = world.getAnimalsNear<Omnivore>(x, y, getCurrentSightRadius());
    predators.insert(predators.end(), omni_predators.begin(), omni_predators.end());

    if (!predators.empty()) {
        current_state = AIState::FLEEING;
        target = predators[0];
    } else {
        current_state = AIState::WANDERING;
        // Graze if wandering and not full on energy
        if (energy < max_energy) {
            energy += HERBIVORE_GRAZE_ENERGY_GAIN; // Use the new constant for grazing
            energy = std::min(energy, max_energy); // Cap at max_energy
        }
    }
}


void Herbivore::act(World& world) {
    // Check if target died or disappeared
    if (target && target->isDead()) {
        target = nullptr;
        current_state = AIState::WANDERING; // Revert to wandering if target is gone
    }

    switch (current_state) {
        case AIState::FLEEING:
            if (target) {
                moveAwayFrom(world, target->getX(), target->getY());
            } else { // Should not happen if target is properly managed, but as a fallback
                moveRandom(world);
            }
            break;
        case AIState::WANDERING:
            moveRandom(world);
            break;
        default: // Should not reach here
            moveRandom(world);
            break;
    }
}

std::unique_ptr<Animal> Herbivore::reproduce() {
    // Check if energy is above the new threshold and age is sufficient
    if (energy > static_cast<int>(max_energy * HERBIVORE_REPRODUCE_ENERGY_PERCENTAGE) &&
        age > HERBIVORE_MIN_REPRODUCE_AGE) {
        energy -= HERBIVORE_REPRODUCE_ENERGY_COST; // Deduct energy cost
        return std::make_unique<Herbivore>(x, y); // Create a new herbivore
    }
    return nullptr; // Cannot reproduce
}