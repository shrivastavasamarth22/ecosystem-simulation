#include "Carnivore.h"
#include "World.h"
#include "Herbivore.h"
#include "Omnivore.h"

// --- Balancing Constants ---
const int CARNIVORE_HP = 80;
const int CARNIVORE_DMG = 25;
const int CARNIVORE_SIGHT = 5;
const int CARNIVORE_SPEED = 2; // Faster than herbivores
const int CARNIVORE_ENERGY = 20;
const int CARNIVORE_REPRODUCE_ENERGY = 35;
const int OMNIVORE_PACK_THREAT_SIZE = 3;

Carnivore::Carnivore(int x, int y)
    : Animal(x, y, 'C', CARNIVORE_HP, CARNIVORE_DMG, CARNIVORE_SIGHT, CARNIVORE_SPEED, CARNIVORE_ENERGY) {}

void Carnivore::updateAI(World& world) {
    // Priority 1: Flee from Omnivore packs
    auto omnivores = world.getAnimalsNear<Omnivore>(x, y, sight_radius);
    if (omnivores.size() >= OMNIVORE_PACK_THREAT_SIZE) {
        current_state = AIState::FLEEING;
        target = omnivores[0];
        return;
    }

    // Priority 2: Hunt Herbivores
    auto prey = world.getAnimalsNear<Herbivore>(x, y, sight_radius);
    if (!prey.empty()) {
        current_state = AIState::CHASING;
        target = prey[0];
        return;
    }

    // If nothing else, wander
    current_state = AIState::WANDERING;
    target = nullptr;
}

void Carnivore::act(World& world) {
    switch (current_state) {
        case AIState::CHASING:
            if (target && !target->isDead()) {
                // Check if adjacent to attack
                int dx = std::abs(x - target->getX());
                int dy = std::abs(y - target->getY());
                if (dx <= 1 && dy <= 1) {
                    target->takeDamage(damage);
                    if (target->isDead()) {
                        energy += 20; // Energy from kill
                    }
                } else {
                    moveTowards(target->getX(), target->getY());
                }
            }
            break;
        case AIState::FLEEING:
            if (target && !target->isDead()) {
                moveAwayFrom(target->getX(), target->getY());
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

std::unique_ptr<Animal> Carnivore::reproduce() {
    if (energy > CARNIVORE_REPRODUCE_ENERGY) {
        energy /= 2;
        return std::make_unique<Carnivore>(x, y);
    }
    return nullptr;
}