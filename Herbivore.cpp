#include "Herbivore.h"
#include "World.h"
#include "Carnivore.h"
#include "Omnivore.h"

// --- Balancing Constants ---
const int HERBIVORE_HP = 30;
const int HERBIVORE_DMG = 2;
const int HERBIVORE_SIGHT = 7;
const int HERBIVORE_SPEED = 1;
const int HERBIVORE_ENERGY = 10;
const int HERBIVORE_REPRODUCE_ENERGY = 15;

Herbivore::Herbivore(int x, int y)
    : Animal(x, y, 'H', HERBIVORE_HP, HERBIVORE_DMG, HERBIVORE_SIGHT, HERBIVORE_SPEED, HERBIVORE_ENERGY) {}

void Herbivore::updateAI(World& world) {
    // Proactively reset state. This prevents dangling pointers from the previous turn.
    target = nullptr;

    // Check for predators
    auto predators = world.getAnimalsNear<Carnivore>(x, y, sight_radius);
    auto omni_predators = world.getAnimalsNear<Omnivore>(x, y, sight_radius);
    predators.insert(predators.end(), omni_predators.begin(), omni_predators.end());

    if (!predators.empty()) {
        current_state = AIState::FLEEING;
        target = predators[0]; // Flee from the first one seen
    } else {
        // If no threats, just wander and graze
        current_state = AIState::WANDERING;
        energy++;
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
                // Pass the world object to the movement function
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
    if (energy > HERBIVORE_REPRODUCE_ENERGY) {
        energy /= 2;
        return std::make_unique<Herbivore>(x, y);
    }
    return nullptr;
}