#include "Omnivore.h"
#include "World.h"
#include "Herbivore.h"
#include "Carnivore.h"

// --- Balancing Constants ---
const int OMNIVORE_HP = 60;
const int OMNIVORE_DMG = 15;
const int OMNIVORE_SIGHT = 6;
const int OMNIVORE_SPEED = 1;
const int OMNIVORE_ENERGY = 15;
const int OMNIVORE_REPRODUCE_ENERGY = 25;
const int OMNIVORE_PACK_HUNT_SIZE = 3;

Omnivore::Omnivore(int x, int y)
    : Animal(x, y, 'O', OMNIVORE_HP, OMNIVORE_DMG, OMNIVORE_SIGHT, OMNIVORE_SPEED, OMNIVORE_ENERGY) {}

void Omnivore::updateAI(World& world) {
    // Priority 1: Hunt Carnivores in packs
    auto carnivores = world.getAnimalsNear<Carnivore>(x, y, sight_radius);
    if (!carnivores.empty()) {
        auto allies = world.getAnimalsNear<Omnivore>(x, y, sight_radius);
        if (allies.size() >= OMNIVORE_PACK_HUNT_SIZE) {
            current_state = AIState::PACK_HUNTING;
            target = carnivores[0];
            return;
        } else {
            // Not enough allies, flee!
            current_state = AIState::FLEEING;
            target = carnivores[0];
            return;
        }
    }
    
    // Priority 2: Hunt Herbivores
    auto herbivores = world.getAnimalsNear<Herbivore>(x, y, sight_radius);
    if (!herbivores.empty()) {
        current_state = AIState::CHASING;
        target = herbivores[0];
        return;
    }

    // If nothing else, wander and graze
    current_state = AIState::WANDERING;
    target = nullptr;
    energy++;
}

void Omnivore::act(World& world) {
     if (target && target->isDead()) {
        target = nullptr;
        current_state = AIState::WANDERING;
    }

    switch (current_state) {
        case AIState::PACK_HUNTING: // Same logic as chasing
        case AIState::CHASING:
            if (target) {
                int dx = std::abs(x - target->getX());
                int dy = std::abs(y - target->getY());
                if (dx <= 1 && dy <= 1) {
                    target->takeDamage(damage);
                    if (target->isDead()) {
                        energy += 15;
                    }
                } else {
                    moveTowards(target->getX(), target->getY());
                }
            }
            break;
        case AIState::FLEEING:
            if (target) {
                moveAwayFrom(target->getX(), target->getY());
            }
            break;
        case AIState::WANDERING:
            moveRandom(world);
            break;
    }
}

std::unique_ptr<Animal> Omnivore::reproduce() {
    if (energy > OMNIVORE_REPRODUCE_ENERGY) {
        energy /= 2;
        return std::make_unique<Omnivore>(x, y);
    }
    return nullptr;
}