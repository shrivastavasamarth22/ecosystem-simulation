#include "Omnivore.h"
#include "World.h"
#include "Herbivore.h"
#include "Carnivore.h"

// --- Balancing Constants ---
const int OMNIVORE_HP = 60;
const int OMNIVORE_BASE_DMG = 15;
const int OMNIVORE_BASE_SIGHT = 6;
const int OMNIVORE_BASE_SPEED = 1;
const int OMNIVORE_MAX_ENERGY = 80; // New
const int OMNIVORE_STARTING_ENERGY = 50;
const int OMNIVORE_REPRODUCE_ENERGY_COST = 25;
const int OMNIVORE_PACK_HUNT_SIZE = 3;
const int ENERGY_FROM_HERB_KILL = 20;
const int ENERGY_FROM_CARN_KILL = 50;


Omnivore::Omnivore(int x, int y)
    : Animal(x, y, 'O', OMNIVORE_HP, OMNIVORE_BASE_DMG, OMNIVORE_BASE_SIGHT, OMNIVORE_BASE_SPEED,
             OMNIVORE_MAX_ENERGY, OMNIVORE_STARTING_ENERGY) {}

void Omnivore::updateAI(World& world) {
    target = nullptr;

    auto carnivores = world.getAnimalsNear<Carnivore>(x, y, getCurrentSightRadius()); // Use current sight
    if (!carnivores.empty()) {
        auto allies = world.getAnimalsNear<Omnivore>(x, y, getCurrentSightRadius()); // Use current sight
        if (allies.size() >= OMNIVORE_PACK_HUNT_SIZE) {
            current_state = AIState::PACK_HUNTING;
            target = carnivores[0];
        } else {
            current_state = AIState::FLEEING;
            target = carnivores[0];
        }
        return;
    }

    auto herbivores = world.getAnimalsNear<Herbivore>(x, y, getCurrentSightRadius()); // Use current sight
    if (!herbivores.empty()) {
        current_state = AIState::CHASING;
        target = herbivores[0];
        return;
    }

    current_state = AIState::WANDERING;
    if (energy < max_energy) energy++; // Graze
}

void Omnivore::act(World& world) {
    if (target && target->isDead()) {
        target = nullptr;
        current_state = AIState::WANDERING;
    }

    switch (current_state) {
        case AIState::PACK_HUNTING: // Omnivores hunt carnivores
        case AIState::CHASING:      // Omnivores hunt herbivores
            if (target) {
                int dx = std::abs(x - target->getX());
                int dy = std::abs(y - target->getY());
                if (dx <= 1 && dy <= 1) {
                    target->takeDamage(getCurrentDamage()); // Use current damage
                    if (target->isDead()) {
                        if (dynamic_cast<Carnivore*>(target)) {
                            energy = std::min(max_energy, energy + ENERGY_FROM_CARN_KILL);
                        } else if (dynamic_cast<Herbivore*>(target)) {
                            energy = std::min(max_energy, energy + ENERGY_FROM_HERB_KILL);
                        }
                    }
                } else {
                    moveTowards(world, target->getX(), target->getY());
                }
            }
            break;
        case AIState::FLEEING:
            if (target) {
                moveAwayFrom(world, target->getX(), target->getY());
            }
            break;
        case AIState::WANDERING:
            moveRandom(world);
            break;
    }
}

std::unique_ptr<Animal> Omnivore::reproduce() {
    if (energy > OMNIVORE_MAX_ENERGY * 0.75 && age > 8) {
        energy -= OMNIVORE_REPRODUCE_ENERGY_COST;
        return std::make_unique<Omnivore>(x, y);
    }
    return nullptr;
}