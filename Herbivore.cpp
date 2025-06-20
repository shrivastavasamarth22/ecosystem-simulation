#include "Herbivore.h"
#include "World.h"
#include "Carnivore.h"
#include "Omnivore.h"

// --- Balancing Constants ---
const int HERBIVORE_BASE_HP = 30; // Renamed from HERBIVORE_HP for clarity
const int HERBIVORE_BASE_DMG = 2;
const int HERBIVORE_BASE_SIGHT = 7;
const int HERBIVORE_BASE_SPEED = 1;
const int HERBIVORE_MAX_ENERGY = 50;
const int HERBIVORE_STARTING_ENERGY = 30;
const int HERBIVORE_REPRODUCE_ENERGY_COST = 15;
const int HERBIVORE_GRAZE_ENERGY_GAIN = 2;
const float HERBIVORE_REPRODUCE_ENERGY_PERCENTAGE = 0.63f;
const int HERBIVORE_MIN_REPRODUCE_AGE = 3;

// --- NEW Herding Constants ---
const int HERD_DETECTION_RADIUS = 10;   // How far a Herbivore can see to find a herd
const int HERD_BONUS_RADIUS = 3;        // How close friends must be to count for HP bonus
const int HERD_HP_BONUS_PER_MEMBER = 5; // +5 max HP for each nearby friend


Herbivore::Herbivore(int x, int y)
    : Animal(x, y, 'H', HERBIVORE_BASE_HP, HERBIVORE_BASE_DMG, HERBIVORE_BASE_SIGHT, HERBIVORE_BASE_SPEED,
             HERBIVORE_MAX_ENERGY, HERBIVORE_STARTING_ENERGY) {}


void Herbivore::updateAI(World& world) {
    target = nullptr; // Reset target each turn

    // --- Herd Buff Calculation ---
    // First, determine buffs from the current situation before making decisions.
    auto nearby_friends = world.getAnimalsNear<Herbivore>(x, y, HERD_BONUS_RADIUS);
    int herd_size = nearby_friends.size();

    // Calculate the HP bonus from the herd
    int hp_bonus = (herd_size > 1) ? (herd_size - 1) * HERD_HP_BONUS_PER_MEMBER : 0;
    int old_max_health = max_health;
    max_health = base_max_health + hp_bonus;

    // If max health increased, give the bonus current health.
    if (max_health > old_max_health) {
        health += (max_health - old_max_health);
    }
    // Crucially, clamp current health to the new max_health.
    // This handles both gaining and losing herd members.
    health = std::min(health, max_health);


    // --- AI Decision Making ---
    // Priority 1: Flee from immediate threats
    auto predators = world.getAnimalsNear<Carnivore>(x, y, getCurrentSightRadius());
    auto omni_predators = world.getAnimalsNear<Omnivore>(x, y, getCurrentSightRadius());
    predators.insert(predators.end(), omni_predators.begin(), omni_predators.end());

    if (!predators.empty()) {
        current_state = AIState::FLEEING;
        target = predators[0];
        return; // Fleeing overrides all other behaviors
    }

    // Priority 2: Seek out a herd if not in one
    if (herd_size <= 1) { // If alone
        auto potential_herd = world.getAnimalsNear<Herbivore>(x, y, HERD_DETECTION_RADIUS);
        if (potential_herd.size() > 1) {
            current_state = AIState::HERDING;
            // Target the first other herbivore found to move towards the group
            target = potential_herd[0] == this ? potential_herd[1] : potential_herd[0];
            return;
        }
    }

    // Priority 3: If in a herd and safe, or alone and can't find a herd, wander and graze
    current_state = AIState::WANDERING;
    if (energy < max_energy) {
        energy += HERBIVORE_GRAZE_ENERGY_GAIN;
        energy = std::min(energy, max_energy);
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
            } else {
                moveRandom(world);
            }
            break;
        case AIState::HERDING:
            if (target) {
                // Move towards the herd target
                moveTowards(world, target->getX(), target->getY());
            } else {
                // Target might have died, revert to wandering to re-evaluate next turn
                current_state = AIState::WANDERING;
                moveRandom(world);
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

// applyAgingPenalties() and reproduce() methods
void Herbivore::applyAgingPenalties() {
    if (age > 40) {
        base_speed = std::max(1, base_speed - 1);
    }
    base_speed = std::max(1, base_speed);
}

int Herbivore::getNutritionalValue() const {
    const int BASE_NUTRITIONAL_VALUE = 45;
    const int PRIME_AGE = 8;
    const int PENALTY_PER_YEAR = 2;
    const int MINIMUM_VALUE = 10;
    int age_penalty = (age > PRIME_AGE) ? (age - PRIME_AGE) * PENALTY_PER_YEAR : 0;
    return std::max(MINIMUM_VALUE, BASE_NUTRITIONAL_VALUE - age_penalty);
}

std::unique_ptr<Animal> Herbivore::reproduce() {
    if (energy > static_cast<int>(max_energy * HERBIVORE_REPRODUCE_ENERGY_PERCENTAGE) &&
        age > HERBIVORE_MIN_REPRODUCE_AGE) {
        energy -= HERBIVORE_REPRODUCE_ENERGY_COST;
        return std::make_unique<Herbivore>(x, y);
    }
    return nullptr;
}