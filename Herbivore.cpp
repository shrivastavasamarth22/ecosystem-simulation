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
    // Check for predators
    auto predators = world.getAnimalsNear<Carnivore>(x, y, sight_radius);
    auto omni_predators = world.getAnimalsNear<Omnivore>(x, y, sight_radius);
    predators.insert(predators.end(), omni_predators.begin(), omni_predators.end());

    if (!predators.empty()) {
        current_state = AIState::FLEEING;
        target = predators[0]; // Flee from the first one seen
        return;
    }

    // If no threats, just wander
    current_state = AIState::WANDERING;
    target = nullptr;
    energy++; // Graze
}

void Herbivore::act(World& world) {
    switch (current_state) {
        case AIState::FLEEING:
            if (target && !target->isDead()) {
                moveAwayFrom(target->getX(), target->getY());
            } else {
                // Target is gone, go back to wandering
                current_state = AIState::WANDERING;
            }
            break;
        case AIState::WANDERING:
            moveRandom(world);
            break;
        default:
            // Other states don't apply to herbivores
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