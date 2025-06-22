#include "includes/EntityManager.h"
#include "includes/AnimalConfig.h"
#include "includes/AnimalTypes.h" // Needed for AnimalType enum used in createEntity
#include "includes/Random.h"
#include <algorithm>
#include <cmath> // For std::abs in destroyEntity (if used there, not currently)
#include <random> // <-- Include random for the definition
#include <vector> // Needed for vector operations


EntityManager::EntityManager() : num_entities(0) {}

size_t EntityManager::getEntityCount() const {
    return num_entities;
}

void EntityManager::clear() {
    is_alive.clear();
    x.clear();
    y.clear();
    target_x.clear();
    target_y.clear();
    target_id.clear();
    type.clear();
    state.clear();
    health.clear();
    max_health.clear();
    base_max_health.clear();
    turns_since_damage.clear();
    energy.clear();
    max_energy.clear();
    base_damage.clear();
    base_sight_radius.clear();
    base_speed.clear();
    current_damage.clear();
    current_sight_radius.clear();
    current_speed.clear();
    age.clear();
    base_nutritional_value.clear();
    prime_age.clear();
    penalty_per_year.clear();
    minimum_nutritional_value.clear();

    num_entities = 0;
}

size_t EntityManager::createEntity() {
    size_t id = num_entities;

    is_alive.push_back(true);
    x.push_back(0);
    y.push_back(0);
    target_x.push_back(-1);
    target_y.push_back(-1);
    target_id.push_back(-1); // Use -1 or a large value for "no target"
    state.push_back(AIState::WANDERING);
    health.push_back(0);
    max_health.push_back(0);
    base_max_health.push_back(0);
    turns_since_damage.push_back(0);
    energy.push_back(0);
    max_energy.push_back(0);
    base_damage.push_back(0);
    base_sight_radius.push_back(0);
    base_speed.push_back(0);
    current_damage.push_back(0);
    current_sight_radius.push_back(0);
    current_speed.push_back(0);
    age.push_back(0);
    base_nutritional_value.push_back(0);
    prime_age.push_back(0);
    penalty_per_year.push_back(0);
    minimum_nutritional_value.push_back(0);
    type.push_back(AnimalType::HERBIVORE); // Default, will be overwritten by helpers

    num_entities++;
    return id;
}

void EntityManager::destroyEntity(size_t index) {
    if (num_entities == 0 || index >= num_entities) return;

    size_t last_index = num_entities - 1;

    if (index != last_index) {
        // Fast removal: swap the last element with the one to be removed
        is_alive[index] = is_alive[last_index];
        x[index] = x[last_index];
        y[index] = y[last_index];
        target_x[index] = target_x[last_index];
        target_y[index] = target_y[last_index];
        target_id[index] = target_id[last_index];
        type[index] = type[last_index];
        state[index] = state[last_index];
        health[index] = health[last_index];
        max_health[index] = max_health[last_index];
        base_max_health[index] = base_max_health[last_index];
        turns_since_damage[index] = turns_since_damage[last_index];
        energy[index] = energy[last_index];
        max_energy[index] = max_energy[last_index];
        base_damage[index] = base_damage[last_index];
        base_sight_radius[index] = base_sight_radius[last_index];
        base_speed[index] = base_speed[last_index];
        current_damage[index] = current_damage[last_index];
        current_sight_radius[index] = current_sight_radius[last_index];
        current_speed[index] = current_speed[last_index];
        age[index] = age[last_index];
        base_nutritional_value[index] = base_nutritional_value[last_index];
        prime_age[index] = prime_age[last_index];
        penalty_per_year[index] = penalty_per_year[last_index];
        minimum_nutritional_value[index] = minimum_nutritional_value[last_index];
    }

    // Pop the back of all vectors
    is_alive.pop_back();
    x.pop_back();
    y.pop_back();
    target_x.pop_back();
    target_y.pop_back();
    target_id.pop_back();
    type.pop_back();
    state.pop_back();
    health.pop_back();
    max_health.pop_back();
    base_max_health.pop_back();
    turns_since_damage.pop_back();
    energy.pop_back();
    max_energy.pop_back();
    base_damage.pop_back();
    base_sight_radius.pop_back();
    base_speed.pop_back();
    current_damage.pop_back();
    current_sight_radius.pop_back();
    current_speed.pop_back();
    age.pop_back();
    base_nutritional_value.pop_back();
    prime_age.pop_back();
    penalty_per_year.pop_back();
    minimum_nutritional_value.pop_back();

    num_entities--;
}

// --- Creation Helpers ---

size_t EntityManager::createHerbivore(int start_x, int start_y) {
    size_t id = createEntity();
    type[id] = AnimalType::HERBIVORE;
    x[id] = start_x;
    y[id] = start_y;
    
    base_max_health[id] = HERBIVORE_BASE_HP;
    max_health[id] = HERBIVORE_BASE_HP;
    health[id] = HERBIVORE_BASE_HP;
    max_energy[id] = HERBIVORE_MAX_ENERGY;
    energy[id] = HERBIVORE_STARTING_ENERGY;
    base_damage[id] = HERBIVORE_BASE_DMG;
    base_speed[id] = HERBIVORE_BASE_SPEED;
    base_sight_radius[id] = HERBIVORE_BASE_SIGHT;
    
    // Set nutritional stats
    base_nutritional_value[id] = HERBIVORE_BASE_NUTRITIONAL_VALUE;
    prime_age[id] = HERBIVORE_PRIME_AGE;
    penalty_per_year[id] = HERBIVORE_PENALTY_PER_YEAR;
    minimum_nutritional_value[id] = HERBIVORE_MINIMUM_VALUE;

    return id;
}

size_t EntityManager::createCarnivore(int start_x, int start_y) {
    size_t id = createEntity();
    type[id] = AnimalType::CARNIVORE;
    x[id] = start_x;
    y[id] = start_y;

    base_max_health[id] = CARNIVORE_BASE_HP;
    max_health[id] = CARNIVORE_BASE_HP;
    health[id] = CARNIVORE_BASE_HP;
    max_energy[id] = CARNIVORE_MAX_ENERGY;
    energy[id] = CARNIVORE_STARTING_ENERGY;
    base_damage[id] = CARNIVORE_BASE_DMG;
    base_speed[id] = CARNIVORE_BASE_SPEED;
    base_sight_radius[id] = CARNIVORE_BASE_SIGHT;
    
    // Set nutritional stats
    base_nutritional_value[id] = CARNIVORE_BASE_NUTRITIONAL_VALUE;
    prime_age[id] = CARNIVORE_PRIME_AGE;
    penalty_per_year[id] = CARNIVORE_PENALTY_PER_YEAR;
    minimum_nutritional_value[id] = CARNIVORE_MINIMUM_VALUE;

    return id;
}

size_t EntityManager::createOmnivore(int start_x, int start_y) {
    size_t id = createEntity();
    type[id] = AnimalType::OMNIVORE;
    x[id] = start_x;
    y[id] = start_y;

    base_max_health[id] = OMNIVORE_BASE_HP;
    max_health[id] = OMNIVORE_BASE_HP;
    health[id] = OMNIVORE_BASE_HP;
    max_energy[id] = OMNIVORE_MAX_ENERGY;
    energy[id] = OMNIVORE_STARTING_ENERGY;
    base_damage[id] = OMNIVORE_BASE_DMG;
    base_speed[id] = OMNIVORE_BASE_SPEED;
    base_sight_radius[id] = OMNIVORE_BASE_SIGHT;
    
    // Set nutritional stats
    base_nutritional_value[id] = OMNIVORE_BASE_NUTRITIONAL_VALUE;
    prime_age[id] = OMNIVORE_PRIME_AGE;
    penalty_per_year[id] = OMNIVORE_PENALTY_PER_YEAR;
    minimum_nutritional_value[id] = OMNIVORE_MINIMUM_VALUE;

    return id;
}

void EntityManager::destroyDeadEntities() {
    for (size_t i = num_entities; i-- > 0; ) {
        if (!is_alive[i]) {
            // The destroyEntity function handles the swap-and-pop
            destroyEntity(i);
        }
    }
}