#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H

#include "AnimalTypes.h"
#include <vector>
#include <cstddef> // For size_t

class EntityManager {
public:
    EntityManager();

    // ... (createEntity, destroyEntity, createHerbivore, etc. are the same) ...

    size_t createEntity();
    void destroyEntity(size_t index);
    size_t createHerbivore(int start_x, int start_y);
    size_t createCarnivore(int start_x, int start_y);
    size_t createOmnivore(int start_x, int start_y);
    size_t getEntityCount() const;
    void clear();

public:
    // --- Attribute Arrays (The SoA) ---
    std::vector<bool>           is_alive;

    // Position & State
    std::vector<int>            x;
    std::vector<int>            y;
    std::vector<int>            target_x;
    std::vector<int>            target_y;
    std::vector<size_t>         target_id;
    std::vector<AnimalType>     type;
    std::vector<AIState>        state;

    // Health & Energy
    std::vector<int>            health;
    std::vector<int>            max_health;
    std::vector<int>            base_max_health;
    std::vector<int>            turns_since_damage;
    std::vector<int>            energy;
    std::vector<int>            max_energy;

    // Stats
    std::vector<int>            base_damage;
    std::vector<int>            base_sight_radius;
    std::vector<int>            base_speed;
    std::vector<int>            current_damage;
    std::vector<int>            current_sight_radius;
    std::vector<int>            current_speed;

    // Life Cycle
    std::vector<int>            age;

    // --- NEW: Nutritional Value Stats ---
    std::vector<int>            base_nutritional_value;
    std::vector<int>            prime_age;
    std::vector<int>            penalty_per_year;
    std::vector<int>            minimum_nutritional_value;


private:
    size_t num_entities;
};

#endif // ENTITY_MANAGER_H