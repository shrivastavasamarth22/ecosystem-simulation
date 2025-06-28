#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H

#include "common/AnimalTypes.h"
#include <vector>
#include <cstddef> // For size_t

// Family relationship constants
const size_t INVALID_PARENT = static_cast<size_t>(-1);

class EntityManager {
public:
    EntityManager();

    // ... (createEntity, destroyEntity, createHerbivore, etc. are the same) ...

    size_t createEntity();
    void destroyEntity(size_t index);
    void destroyDeadEntities();
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
    std::vector<int>            prev_x;
    std::vector<int>            prev_y;
    std::vector<int>            target_x;
    std::vector<int>            target_y;
    std::vector<size_t>         target_id;
    std::vector<AnimalType>     type;
    std::vector<AIState>        state;

    // Health & Energy
    std::vector<float>          health;
    std::vector<float>          max_health;
    std::vector<float>          base_max_health;
    std::vector<int>            turns_since_damage;
    std::vector<float>          energy;
    std::vector<float>          max_energy;

    // Stats
    std::vector<float>          base_damage;
    std::vector<float>          base_sight_radius;
    std::vector<float>          base_speed;
    std::vector<float>          current_damage;
    std::vector<float>          current_sight_radius;
    std::vector<float>          current_speed;

    // Life Cycle
    std::vector<int>            age;
    
    // Family Relationships  
    std::vector<size_t>         parent_id;        // ID of parent entity (INVALID_ENTITY if none)
    std::vector<int>            independence_age; // Age at which offspring becomes independent from parent

    // --- NEW: Nutritional Value Stats ---
    std::vector<float>          base_nutritional_value;
    std::vector<int>            prime_age;
    std::vector<float>          penalty_per_year;
    std::vector<float>          minimum_nutritional_value;


private:
    size_t num_entities;
};

#endif // ENTITY_MANAGER_H
