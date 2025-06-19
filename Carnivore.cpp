#include "Carnivore.h"
#include "World.h"
#include "Herbivore.h"
#include "Omnivore.h"

// --- Balancing Constants ---
const int CARNIVORE_HP = 80;
const int CARNIVORE_BASE_DMG = 25;
const int CARNIVORE_BASE_SIGHT = 5;
const int CARNIVORE_BASE_SPEED = 2;
const int CARNIVORE_MAX_ENERGY = 100; // New
const int CARNIVORE_STARTING_ENERGY = 70;
const int CARNIVORE_REPRODUCE_ENERGY_COST = 35;
const int OMNIVORE_PACK_THREAT_SIZE = 3;
const int ENERGY_FROM_KILL = 40;

Carnivore::Carnivore(int x, int y)
    : Animal(x, y, 'C', CARNIVORE_HP, CARNIVORE_BASE_DMG, CARNIVORE_BASE_SIGHT, CARNIVORE_BASE_SPEED,
             CARNIVORE_MAX_ENERGY, CARNIVORE_STARTING_ENERGY) {}

void Carnivore::updateAI(World& world) {
    target = nullptr;

    auto omnivores = world.getAnimalsNear<Omnivore>(x, y, getCurrentSightRadius()); // Use current sight
    if (omnivores.size() >= OMNIVORE_PACK_THREAT_SIZE) {
        current_state = AIState::FLEEING;
        target = omnivores[0];
        return;
    }

    auto prey = world.getAnimalsNear<Herbivore>(x, y, getCurrentSightRadius()); // Use current sight
    if (!prey.empty()) {
        current_state = AIState::CHASING;
        target = prey[0];
        return;
    }

    current_state = AIState::WANDERING;
}

void Carnivore::act(World& world) {
    if (target && target->isDead()) {
        target = nullptr;
        current_state = AIState::WANDERING;
    }

    switch (current_state) {
        case AIState::CHASING:
            if (target) {
                int dx = std::abs(x - target->getX());
                int dy = std::abs(y - target->getY());
                if (dx <= 1 && dy <= 1) {
                    target->takeDamage(getCurrentDamage()); // Use current damage
                    if (target->isDead()) {
                        energy = std::min(max_energy, energy + ENERGY_FROM_KILL); // Gain energy, cap at max
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
        default:
            moveRandom(world);
            break;
    }
}

std::unique_ptr<Animal> Carnivore::reproduce() {
    if (energy > CARNIVORE_MAX_ENERGY * 0.75 && age > 10) {
        energy -= CARNIVORE_REPRODUCE_ENERGY_COST;
        return std::make_unique<Carnivore>(x, y);
    }
    return nullptr;
}