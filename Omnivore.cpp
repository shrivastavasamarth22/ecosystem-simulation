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

// New/Adjusted constants for reproduction
const float OMNIVORE_REPRODUCE_ENERGY_PERCENTAGE = 0.65f; // << NEW: Need 65% of max energy
const int OMNIVORE_MIN_REPRODUCE_AGE = 8;                 // Age requirement


Omnivore::Omnivore(int x, int y)
    : Animal(x, y, 'O', OMNIVORE_HP, OMNIVORE_BASE_DMG, OMNIVORE_BASE_SIGHT, OMNIVORE_BASE_SPEED,
             OMNIVORE_MAX_ENERGY, OMNIVORE_STARTING_ENERGY) {}


void Omnivore::applyAgingPenalties() {
    if (age > 45) {
        base_speed = std::max(1, base_speed - 1);
        base_damage = std::max(0, base_damage - 3);
    }
    if (age > 60) {
        base_sight_radius = std::max(1, base_sight_radius - 1);
        base_damage = std::max(0, base_damage - 2);
    }

    base_speed = std::max(1, base_speed);
    base_damage = std::max(0, base_damage);
    base_sight_radius = std::max(1, base_sight_radius);
}


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
        case AIState::PACK_HUNTING:
        case AIState::CHASING:
            if (target) {
                int dx = std::abs(x - target->getX());
                int dy = std::abs(y - target->getY());
                if (dx <= 1 && dy <= 1) {
                    target->takeDamage(getCurrentDamage());
                    if (target->isDead()) {
                        // --- UPDATED LOGIC ---
                        int energy_gained = target->getNutritionalValue();
                        energy = std::min(max_energy, energy + energy_gained);
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

int Omnivore::getNutritionalValue() const {
    const int BASE_NUTRITIONAL_VALUE = 35;
    const int PRIME_AGE = 15;
    const int PENALTY_PER_YEAR = 1;
    const int MINIMUM_VALUE = 10;

    int age_penalty = (age > PRIME_AGE) ? (age - PRIME_AGE) * PENALTY_PER_YEAR : 0;
    return std::max(MINIMUM_VALUE, BASE_NUTRITIONAL_VALUE - age_penalty);
}

std::unique_ptr<Animal> Omnivore::reproduce() {
    // Use the new percentage constant for the energy threshold
    if (energy > static_cast<int>(max_energy * OMNIVORE_REPRODUCE_ENERGY_PERCENTAGE) &&
        age > OMNIVORE_MIN_REPRODUCE_AGE) {
        energy -= OMNIVORE_REPRODUCE_ENERGY_COST;
        return std::make_unique<Omnivore>(x, y);
    }
    return nullptr;
}