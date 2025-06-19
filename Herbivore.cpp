#include "Herbivore.h"
#include "World.h"
#include "Carnivore.h"
#include "Omnivore.h"

// --- Balancing Constants ---
const int HERBIVORE_HP = 30;
const int HERBIVORE_BASE_DMG = 2;
const int HERBIVORE_BASE_SIGHT = 7;
const int HERBIVORE_BASE_SPEED = 1;
const int HERBIVORE_MAX_ENERGY = 50; // New
const int HERBIVORE_STARTING_ENERGY = 30;
const int HERBIVORE_REPRODUCE_ENERGY_COST = 15; // Renamed for clarity

Herbivore::Herbivore(int x, int y)
    : Animal(x, y, 'H', HERBIVORE_HP, HERBIVORE_BASE_DMG, HERBIVORE_BASE_SIGHT, HERBIVORE_BASE_SPEED,
             HERBIVORE_MAX_ENERGY, HERBIVORE_STARTING_ENERGY) {}

void Herbivore::updateAI(World& world) {
    target = nullptr;

    auto predators = world.getAnimalsNear<Carnivore>(x, y, getCurrentSightRadius()); // Use current sight
    auto omni_predators = world.getAnimalsNear<Omnivore>(x, y, getCurrentSightRadius()); // Use current sight
    predators.insert(predators.end(), omni_predators.begin(), omni_predators.end());

    if (!predators.empty()) {
        current_state = AIState::FLEEING;
        target = predators[0];
    } else {
        current_state = AIState::WANDERING;
        if (energy < max_energy) energy++; // Graze, but don't exceed max energy
    }
}

void Herbivore::act(World& world) {
    if (target && target->isDead()) {
        target = nullptr;
        current_state = AIState::WANDERING;
    }

    switch (current_state) {
        case AIState::FLEEING:
            if (target) {
                moveAwayFrom(world, target->getX(), target->getY());
            }
            break;
        case AIState::WANDERING:
            moveRandom(world);
            break;
        default:
            moveRandom(world);
            break;
    }
}

std::unique_ptr<Animal> Herbivore::reproduce() {
    if (energy > HERBIVORE_MAX_ENERGY * 0.75 && age > 5) { // Reproduce if energy is high and old enough
        energy -= HERBIVORE_REPRODUCE_ENERGY_COST;
        return std::make_unique<Herbivore>(x, y);
    }
    return nullptr;
}