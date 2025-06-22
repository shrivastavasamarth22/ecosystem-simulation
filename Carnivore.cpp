#include "Carnivore.h"
#include "World.h"
#include "Herbivore.h"
#include "Omnivore.h"

// --- Balancing Constants ---
const int CARNIVORE_BASE_HP = 80;
const int CARNIVORE_BASE_DMG = 25;
const int CARNIVORE_BASE_SIGHT = 5;
const int CARNIVORE_BASE_SPEED = 2;
const int CARNIVORE_MAX_ENERGY = 100; // New
const int CARNIVORE_STARTING_ENERGY = 70;
const int CARNIVORE_REPRODUCE_ENERGY_COST = 35;

// New/Adjusted constants for reproduction
const float CARNIVORE_REPRODUCE_ENERGY_PERCENTAGE = 0.70f; // << NEW: Need 70% of max energy
const int CARNIVORE_MIN_REPRODUCE_AGE = 10;                // Age requirement


const int OMNIVORE_PACK_THREAT_SIZE = 3;

Carnivore::Carnivore(int x, int y)
    : Animal(x, y, 'C', CARNIVORE_BASE_HP, CARNIVORE_BASE_DMG, CARNIVORE_BASE_SIGHT, CARNIVORE_BASE_SPEED,
             CARNIVORE_MAX_ENERGY, CARNIVORE_STARTING_ENERGY) {}


void Carnivore::applyAgingPenalties() {
    if (age > 50) {
        base_speed = std::max(1, base_speed - 1);
        base_damage = std::max(0, base_damage - 5);
    }

    if (age > 70) {
        base_speed = std::max(1, base_speed - 1);
        base_damage = std::max(0, base_damage - 10);
        base_sight_radius = std::max(1, base_sight_radius - 1);
    }

    base_speed = std::max(1, base_speed);
    base_damage = std::max(0, base_damage);
    base_sight_radius = std::max(1, base_sight_radius);
}

void Carnivore::updateAI(World& world) {
    target = nullptr; // Reset target each turn

    // --- Perception Phase ---
    auto nearby_omnivores = world.getAnimalsNear<Omnivore>(x, y, getCurrentSightRadius());
    auto nearby_herbivores = world.getAnimalsNear<Herbivore>(x, y, getCurrentSightRadius());

    // --- Decision Phase ---

    // Priority 1: Flee from Omnivore packs
    if (nearby_omnivores.size() >= OMNIVORE_PACK_THREAT_SIZE) {
        current_state = AIState::FLEEING;
        target = nearby_omnivores[0]; // Flee from the first one in the pack detected
        return;
    }

    // Priority 2: Hunt Herbivores
    if (!nearby_herbivores.empty()) {
        current_state = AIState::CHASING;
        target = nearby_herbivores[0]; // Target the first herbivore seen
        return;
    }

    // Priority 3: Hunt lone or small groups of Omnivores
    // This condition means there's no large omnivore pack threat, and no herbivores were found.
    // We check if there are *any* omnivores nearby that are NOT a pack threat.
    if (!nearby_omnivores.empty() && nearby_omnivores.size() < OMNIVORE_PACK_THREAT_SIZE) {
        current_state = AIState::CHASING; // Same state as chasing herbivores
        target = nearby_omnivores[0];   // Target the first lone/small group omnivore
        return;
    }

    // Priority 4: If no threats and no prey, wander
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
                if (dx <= 1 && dy <= 1) { // If adjacent, attack
                    target->takeDamage(getCurrentDamage());
                    if (target->isDead()) {
                        int energy_gained = target->getNutritionalValue();
                        energy = std::min(max_energy, energy + energy_gained);
                    }
                } else { // Not adjacent, move towards target
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
        // --- FIX ---
        case AIState::HERDING: // Explicitly handle HERDING
        case AIState::PACK_HUNTING: // PACK_HUNTING is an omnivore state, not carnivore
        default:
            // Safest default action for a Carnivore in an unexpected state
            // is to wander and re-evaluate next turn.
            moveRandom(world);
            break;
    }
}


int Carnivore::getNutritionalValue() const {
    const int BASE_NUTRITIONAL_VALUE = 50; // Very rewarding kill
    const int PRIME_AGE = 25;              // Prime age is much older
    const int PENALTY_PER_YEAR = 1;        // Value degrades slowly
    const int MINIMUM_VALUE = 15;

    int age_penalty = (age > PRIME_AGE) ? (age - PRIME_AGE) * PENALTY_PER_YEAR : 0;
    return std::max(MINIMUM_VALUE, BASE_NUTRITIONAL_VALUE - age_penalty);
}

std::unique_ptr<Animal> Carnivore::reproduce() {
    // Use the new percentage constant for the energy threshold
    if (energy > static_cast<int>(max_energy * CARNIVORE_REPRODUCE_ENERGY_PERCENTAGE) &&
        age > CARNIVORE_MIN_REPRODUCE_AGE) {
        energy -= CARNIVORE_REPRODUCE_ENERGY_COST;
        return std::make_unique<Carnivore>(x, y);
    }
    return nullptr;
}