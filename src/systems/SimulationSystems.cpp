#include "systems/SimulationSystems.h"
#include "common/AnimalConfig.h" // Need constants
#include "core/World.h"
#include "core/Random.h"
#include <algorithm>
#include <cmath>
#include <random>

namespace MetabolismSystem {
    // Helper function to apply damage directly to entity data
    void applyDamage(EntityManager& data, size_t entity_id, float amount) {
        if (entity_id >= data.getEntityCount() || !data.is_alive[entity_id]) {
            return; // Cannot damage invalid or dead entity
        }

        data.health[entity_id] -= amount;
        data.turns_since_damage[entity_id] = 0; // Reset regen timer

        if (data.health[entity_id] <= 0.0f) {
            data.is_alive[entity_id] = false; // Mark as dead
            data.health[entity_id] = 0.0f;       // Ensure health is 0 when dead
            // Actual removal from vectors happens in EntityManager::destroyDeadEntities
        }
    }

    void run(EntityManager& data) {
        size_t num_entities = data.getEntityCount();

        // --- Parallelize the loop using OpenMP ---
        // The 'i' variable is automatically made private to each thread.
        // Other variables accessed within the loop (like data, constants) are shared/global.
        #pragma omp parallel for
        for (size_t i = 0; i < num_entities; ++i) {
            // Note: It's crucial that operations on entity 'i' ONLY access data[i]
            // and do not write to data[j] where j != i within this loop.
            // applyDamage(data, i, ...) is okay because it applies damage *to entity i*.

            if (!data.is_alive[i]) continue;

            data.age[i]++;
            data.energy[i] -= 1.0f;

            data.current_damage[i] = data.base_damage[i];
            data.current_speed[i] = data.base_speed[i];
            data.current_sight_radius[i] = data.base_sight_radius[i];

            if (data.max_energy[i] > 0.0f) {
                float energy_percentage = data.energy[i] / data.max_energy[i];
                if (energy_percentage < 0.3f) {
                    MetabolismSystem::applyDamage(data, i, 5.0f);
                    if (!data.is_alive[i]) continue; // Re-check after damage
                    data.current_damage[i] = std::max(0.0f, data.current_damage[i] + 7.0f);
                    data.current_speed[i] = std::max(1.0f, data.current_speed[i] + 2.0f);
                    data.current_sight_radius[i] = std::max(1.0f, data.current_sight_radius[i] + 3.0f);
                } else if (energy_percentage < 0.5f) {
                     MetabolismSystem::applyDamage(data, i, 2.0f);
                     if (!data.is_alive[i]) continue; // Re-check after damage
                    data.current_damage[i] = std::max(0.0f, data.current_damage[i] + 2.0f);
                    data.current_speed[i] = std::max(1.0f, data.current_speed[i] + 1.0f);
                    data.current_sight_radius[i] = std::max(1.0f, data.current_sight_radius[i] + 1.0f);
                }
            }
            data.turns_since_damage[i]++;
            const int REGEN_DELAY_TURNS = 3;
            const float REGEN_AMOUNT = 1.0f;
            if (data.turns_since_damage[i] > REGEN_DELAY_TURNS) {
                float actual_regen_cap;
                float current_health_percentage = data.health[i] / data.max_health[i];
                if (current_health_percentage >= 0.90f) actual_regen_cap = data.max_health[i];
                else if (current_health_percentage >= 0.75f) actual_regen_cap = data.max_health[i] * 0.90f;
                else if (current_health_percentage >= 0.50f) actual_regen_cap = data.max_health[i] * 0.75f;
                else actual_regen_cap = data.max_health[i] * 0.60f;
                if (data.health[i] < actual_regen_cap) {
                    data.health[i] += REGEN_AMOUNT;
                    data.health[i] = std::min(data.health[i], actual_regen_cap);
                }
            }
            data.energy[i] = std::max(0.0f, data.energy[i]);
            data.energy[i] = std::min(data.energy[i], data.max_energy[i]);
        }
    }
}

namespace MovementSystem {

    // Helper function to move towards a coordinate target
    void moveTowards(EntityManager& data, size_t entity_id, const World& world, int target_x_coord, int target_y_coord) {
        if (!data.is_alive[entity_id]) return;

        // Check if already at target
        if (data.x[entity_id] == target_x_coord && data.y[entity_id] == target_y_coord) {
            return; // Already at the target coordinates, no movement needed.
        }

        for (int i = 0; i < data.current_speed[entity_id]; ++i) {
            int dx = target_x_coord - data.x[entity_id];
            int dy = target_y_coord - data.y[entity_id];

            int move_dx = 0;
            int move_dy = 0;

            // --- REVISED DIAGONAL MOVEMENT LOGIC ---
            // Move horizontally if there's a horizontal distance to cover
            if (dx != 0) {
                move_dx = (dx > 0) ? 1 : -1;
            }
            // Move vertically if there's a vertical distance to cover
            if (dy != 0) {
                move_dy = (dy > 0) ? 1 : -1;
            }
            // This allows for simultaneous movement in both x and y (diagonal).

            int new_x = data.x[entity_id] + move_dx;
            int new_y = data.y[entity_id] + move_dy;

            // Check world boundaries before updating position
            if (new_x >= 0 && new_x < world.getWidth()) {
                data.x[entity_id] = new_x;
            }
            if (new_y >= 0 && new_y < world.getHeight()) {
                data.y[entity_id] = new_y;
            }

            // If we reached the target *during* this speed step, we can stop early
            if (data.x[entity_id] == target_x_coord && data.y[entity_id] == target_y_coord) {
                break; // Stop moving for this turn if target reached
            }
        }
    }

    // Helper function to move away from a coordinate target
    void moveAwayFrom(EntityManager& data, size_t entity_id, const World& world, int target_x_coord, int target_y_coord) {
        if (!data.is_alive[entity_id]) return;

        // If trying to move away from the exact spot they are on, move randomly.
        if (data.x[entity_id] == target_x_coord && data.y[entity_id] == target_y_coord) {
            moveRandom(data, entity_id, world);
            return;
        }

        for (int i = 0; i < data.current_speed[entity_id]; ++i) {
            int dx = data.x[entity_id] - target_x_coord; // Direction *away* from target
            int dy = data.y[entity_id] - target_y_coord; // Direction *away* from target

            int move_dx = 0;
            int move_dy = 0;

            // --- REVISED DIAGONAL MOVEMENT LOGIC ---
            if (dx != 0) {
                move_dx = (dx > 0) ? 1 : -1;
            }
            if (dy != 0) {
                move_dy = (dy > 0) ? 1 : -1;
            }

            // If for some reason dx/dy calculation results in no move (e.g. target is far but aligned),
            // this could stall. A random nudge can help, but the current logic should prevent this.
            // As a fallback, if no move is calculated, nudge randomly.
            if (move_dx == 0 && move_dy == 0) {
                if (rng() % 2 == 0) {
                    move_dx = (rng() % 2 == 0) ? 1 : -1;
                } else {
                    move_dy = (rng() % 2 == 0) ? 1 : -1;
                }
            }

            int new_x = data.x[entity_id] + move_dx;
            int new_y = data.y[entity_id] + move_dy;

            // Check world boundaries before updating position
             if (new_x >= 0 && new_x < world.getWidth()) {
                data.x[entity_id] = new_x;
            }
            if (new_y >= 0 && new_y < world.getHeight()) {
                data.y[entity_id] = new_y;
            }
        }
    }

    // Helper function for random movement
    void moveRandom(EntityManager& data, size_t entity_id, const World& world) {
        if (!data.is_alive[entity_id]) return;

        std::uniform_int_distribution<int> dist(-1, 1);
        for (int i = 0; i < data.current_speed[entity_id]; ++i) {
            int new_x = data.x[entity_id] + dist(rng);
            int new_y = data.y[entity_id] + dist(rng);

            if (new_x >= 0 && new_x < world.getWidth() && new_y >= 0 && new_y < world.getHeight()) {
                data.x[entity_id] = new_x;
                data.y[entity_id] = new_y;
            }
        }
    }


    // Main Movement System run function
    void run(EntityManager& data, const World& world) {
        size_t num_entities = data.getEntityCount();

        // --- Parallelize the loop using OpenMP ---
        // Each thread processes a chunk of entities.
        // Reads (data.state, data.target_id, etc.) are safe.
        // Writes (data.x[i], data.y[i]) must ONLY be to the current entity 'i'. This is true for movement.
        #pragma omp parallel for
        for (size_t i = 0; i < num_entities; ++i) {
             if (!data.is_alive[i]) continue;

             AIState current_state = data.state[i];

             switch (current_state) {
                 case AIState::FLEEING:
                 case AIState::CHASING:
                 case AIState::PACK_HUNTING:
                 case AIState::HERDING:
                 {
                     size_t target_entity_id = data.target_id[i];
                     // Check if target is valid and alive.
                     if (target_entity_id != (size_t)-1 && target_entity_id < data.getEntityCount() && data.is_alive[target_entity_id]) {
                         int target_x = data.x[target_entity_id];
                         int target_y = data.y[target_entity_id];

                         if (current_state == AIState::FLEEING) {
                              moveAwayFrom(data, i, world, target_x, target_y);
                         } else { // CHASING, PACK_HUNTING, HERDING move TOWARDS animal target
                             moveTowards(data, i, world, target_x, target_y);
                         }
                     } else {
                          // Target invalid - default to random movement. AI should fix state next turn.
                         moveRandom(data, i, world);
                     }
                 }
                 break;

                 case AIState::SEEKING_FOOD:
                 {
                     int target_x_coord = data.target_x[i];
                     int target_y_coord = data.target_y[i];

                     // Check if coordinate target is valid.
                     if (target_x_coord != -1 && target_y_coord != -1) {
                         // Only move if not already there
                         if (data.x[i] != target_x_coord || data.y[i] != target_y_coord) {
                              moveTowards(data, i, world, target_x_coord, target_y_coord);
                         }
                     } else {
                         // Target coordinates invalid - default to random movement. AI should fix state next turn.
                         moveRandom(data, i, world);
                     }
                 }
                 break;

                 case AIState::WANDERING:
                     moveRandom(data, i, world);
                     break;

                 default: // Should not happen
                     moveRandom(data, i, world);
                     break;
             }
        } // End loop over entities
    } // End run function

} // End namespace MovementSystem


namespace AISystem {

    void run(EntityManager& data, const World& world) {
        size_t num_entities = data.getEntityCount();

        // --- Parallelize the loop using OpenMP ---
        // Each thread processes a chunk of entities.
        // Reads (world, data.is_alive, data.type, data.x, data.y, etc.) are safe.
        // Writes (data.state, data.target_id, data.target_x, data.target_y, data.max_health, data.health)
        // must ONLY be to the current entity 'i'. This is true for the AI logic.
        #pragma omp parallel for
        for (size_t i = 0; i < num_entities; ++i) {
                if (!data.is_alive[i]) continue;

                data.target_id[i] = (size_t)-1; data.target_x[i] = -1; data.target_y[i] = -1;

                // --- Variables local to the loop iteration ---
                // Use best_food_energy instead of best_food_amount
                float best_food_energy = 0.0f; // <-- Changed
                int food_x = -1;
                int food_y = -1;
                int herd_size = 0;

                switch (data.type[i]) {
                    case AnimalType::HERBIVORE:
                    {
                        // Herd Buff Calculation (Still here for now)
                        auto nearby_friends = world.getAnimalsNear(data, data.x[i], data.y[i], HERD_BONUS_RADIUS, AnimalType::HERBIVORE);
                        herd_size = nearby_friends.size();

                        float hp_bonus = (herd_size > 1) ? (herd_size - 1) * HERD_HP_BONUS_PER_MEMBER : 0.0f;
                        float old_max_health = data.max_health[i];
                        data.max_health[i] = data.base_max_health[i] + hp_bonus;
                        if (data.max_health[i] > old_max_health) data.health[i] += (data.max_health[i] - old_max_health);
                        data.health[i] = std::min(data.health[i], data.max_health[i]);

                        // Decision Making
                        auto predators = world.getAnimalsNear(data, data.x[i], data.y[i], static_cast<int>(data.current_sight_radius[i]), AnimalType::CARNIVORE);
                        auto omni_predators = world.getAnimalsNear(data, data.x[i], data.y[i], static_cast<int>(data.current_sight_radius[i]), AnimalType::OMNIVORE);
                        predators.insert(predators.end(), omni_predators.begin(), omni_predators.end());
                        if (!predators.empty()) {
                            data.state[i] = AIState::FLEEING; data.target_id[i] = predators[0]; continue;
                        }

                        // Seek Food if hungry - Prioritize by ENERGY
                        if (data.energy[i] < data.max_energy[i] * HERBIVORE_FOOD_SEEK_THRESHOLD_PERCENTAGE) {
                            // Use best_food_energy
                            best_food_energy = 0.0f; food_x = -1; food_y = -1; // Reset for this scan
                            int scan_radius = static_cast<int>(data.current_sight_radius[i]);

                            for (int dy = -scan_radius; dy <= scan_radius; ++dy) { for (int dx = -scan_radius; dx <= scan_radius; ++dx) {
                                    int check_x = data.x[i] + dx; int check_y = data.y[i] + dy;
                                    if (check_x >= 0 && check_x < world.getWidth() && check_y >= 0 && check_y < world.getHeight() && dx * dx + dy * dy <= scan_radius * scan_radius) {
                                        const Tile& tile = world.getTile(check_x, check_y);
                                        // --- MODIFIED COMPARISON ---
                                        if (tile.resource_type) { // Ensure there is a resource type
                                            float potential_energy = tile.resource_amount * tile.resource_type->nutritional_value;
                                            if (potential_energy > best_food_energy) { // <-- Compare energy
                                                best_food_energy = potential_energy; // <-- Store energy
                                                food_x = check_x; food_y = check_y;
                                            }
                                        }
                                    }
                                }
                            }
                            // Check if any food energy was found
                            if (best_food_energy > 0.0f) { // <-- Check energy > 0
                                data.state[i] = AIState::SEEKING_FOOD; data.target_x[i] = food_x; data.target_y[i] = food_y; continue;
                            }
                        }

                        // Seek out a herd if not in one - Uses calculated herd_size
                        if (herd_size <= 1) {
                            auto potential_herd = world.getAnimalsNear(data, data.x[i], data.y[i], HERD_DETECTION_RADIUS, AnimalType::HERBIVORE);
                            if (!potential_herd.empty()) {
                                size_t target_found_id = (size_t)-1;
                                for(size_t potential_target_id : potential_herd) {
                                    if (potential_target_id != i) {
                                        target_found_id = potential_target_id;
                                        break;
                                    }
                                }
                                if (target_found_id != (size_t)-1) {
                                    data.state[i] = AIState::HERDING; data.target_id[i] = target_found_id; continue;
                                }
                            }
                        }
                        data.state[i] = AIState::WANDERING;
                    } break;

                    // ... (CARNIVORE case remains the same) ...
                    case AnimalType::CARNIVORE:
                    {
                        // Priority 1: Flee from Omnivore packs
                        auto nearby_omnivores_for_pack_check = world.getAnimalsNear(data, data.x[i], data.y[i], 2, AnimalType::OMNIVORE);
                        if(nearby_omnivores_for_pack_check.size() >= OMNIVORE_PACK_THREAT_SIZE) {
                            data.state[i] = AIState::FLEEING; data.target_id[i] = nearby_omnivores_for_pack_check[0]; continue;
                        }

                        // --- NEW Priority 2: Confront Rival Carnivores ---
                        // Check for *other* carnivores within territorial radius
                        auto nearby_rival_carnivores = world.getAnimalsNear(data, data.x[i], data.y[i], CARNIVORE_TERRITORIAL_RADIUS, AnimalType::CARNIVORE);
                        // Exclude self from the list
                        // A simpler check: if the list is not empty AND contains an ID other than 'i'
                        if (!nearby_rival_carnivores.empty()) {
                            size_t rival_id = (size_t)-1;
                            for (size_t potential_rival_id : nearby_rival_carnivores) {
                                if (potential_rival_id != i) {
                                    rival_id = potential_rival_id;
                                    break; // Found a rival!
                                }
                            }
                            if (rival_id != (size_t)-1) {
                                data.state[i] = AIState::CHASING; // Use CHASING state for combat
                                data.target_id[i] = rival_id;
                                continue; // Decision made
                            }
                        }


                        // Old Priority 2 becomes NEW Priority 3: Hunt Herbivores
                        auto nearby_herbivores = world.getAnimalsNear(data, data.x[i], data.y[i], static_cast<int>(data.current_sight_radius[i]), AnimalType::HERBIVORE);
                        if (!nearby_herbivores.empty()) {
                            data.state[i] = AIState::CHASING; data.target_id[i] = nearby_herbivores[0]; continue;
                        }

                        // Old Priority 3 becomes NEW Priority 4: Hunt lone or small groups of Omnivores
                        auto nearby_omnivores_full_sight = world.getAnimalsNear(data, data.x[i], data.y[i], static_cast<int>(data.current_sight_radius[i]), AnimalType::OMNIVORE);
                        if (!nearby_omnivores_full_sight.empty()) {
                            data.state[i] = AIState::CHASING; data.target_id[i] = nearby_omnivores_full_sight[0]; continue;
                        }

                        // Old Priority 4 becomes NEW Priority 5: If no threats, no prey, no rivals, wander
                        data.state[i] = AIState::WANDERING;
                    } break;


                    case AnimalType::OMNIVORE:
                    {
                        auto nearby_carnivores_for_pack_check = world.getAnimalsNear(data, data.x[i], data.y[i], 2, AnimalType::CARNIVORE);
                        if(nearby_carnivores_for_pack_check.size() >= OMNIVORE_PACK_HUNT_SIZE) {
                            data.state[i] = AIState::FLEEING; data.target_id[i] = nearby_carnivores_for_pack_check[0]; continue;
                        }
                        auto nearby_herbivores = world.getAnimalsNear(data, data.x[i], data.y[i], static_cast<int>(data.current_sight_radius[i]), AnimalType::HERBIVORE);
                        if (!nearby_herbivores.empty()) {
                            data.state[i] = AIState::CHASING; data.target_id[i] = nearby_herbivores[0]; continue;
                        }

                        // Seek Grass if hungry - Prioritize by ENERGY
                        if (data.energy[i] < data.max_energy[i] * OMNIVORE_FOOD_SEEK_THRESHOLD_PERCENTAGE) {
                            // Use best_food_energy
                            best_food_energy = 0.0f; food_x = -1; food_y = -1; // Reset for this scan
                            int scan_radius = static_cast<int>(data.current_sight_radius[i]);

                            for (int dy = -scan_radius; dy <= scan_radius; ++dy) { for (int dx = -scan_radius; dx <= scan_radius; ++dx) {
                                    int check_x = data.x[i] + dx; int check_y = data.y[i] + dy;
                                    if (check_x >= 0 && check_x < world.getWidth() && check_y >= 0 && check_y < world.getHeight() && dx * dx + dy * dy <= scan_radius * scan_radius) {
                                        const Tile& tile = world.getTile(check_x, check_y);
                                        // --- MODIFIED COMPARISON ---
                                        if (tile.resource_type) { // Ensure there is a resource type
                                            float potential_energy = tile.resource_amount * tile.resource_type->nutritional_value;
                                            if (potential_energy > best_food_energy) { // <-- Compare energy
                                                best_food_energy = potential_energy; // <-- Store energy
                                                food_x = check_x; food_y = check_y;
                                            }
                                        }
                                    }
                                }
                            }
                            // Check if any food energy was found
                            if (best_food_energy > 0.0f) { // <-- Check energy > 0
                                data.state[i] = AIState::SEEKING_FOOD; data.target_x[i] = food_x; data.target_y[i] = food_y; continue;
                            }
                        }

                        auto nearby_carnivores_for_hunt = world.getAnimalsNear(data, data.x[i], data.y[i], static_cast<int>(data.current_sight_radius[i]), AnimalType::CARNIVORE);
                        if (!nearby_carnivores_for_hunt.empty()) {
                            size_t potential_carnivore_target_id = nearby_carnivores_for_hunt[0];
                            if (potential_carnivore_target_id != (size_t)-1 && potential_carnivore_target_id < data.getEntityCount() && data.is_alive[potential_carnivore_target_id]) {
                                auto allies_near_target = world.getAnimalsNear(data, data.x[potential_carnivore_target_id], data.y[potential_carnivore_target_id], 2, AnimalType::OMNIVORE);
                                if(allies_near_target.size() >= OMNIVORE_PACK_HUNT_SIZE) {
                                    data.state[i] = AIState::PACK_HUNTING; data.target_id[i] = potential_carnivore_target_id; continue;
                                }
                            }
                        }
                        data.state[i] = AIState::WANDERING;
                    } break;
                    default: data.state[i] = AIState::WANDERING; break;
                }
        }
    }
}

namespace ActionSystem {

    void run(EntityManager& data, World& world) {
        size_t num_entities = data.getEntityCount();

        // --- This loop is INTENTIONALLY SINGLE-THREADED ---
        // Parallelizing this loop with a simple #pragma omp parallel for
        // would introduce race conditions because:
        // 1. Combat modifies data for the TARGET entity (data.health[target_id]), not just the current entity (i).
        // 2. Resource consumption modifies the shared World::grid data (tile.resource_amount).
        // More complex synchronization (atomics, locks) or system restructuring is needed for parallel action resolution.
        // For now, processing actions sequentially is safe.

        for (size_t i = 0; i < num_entities; ++i) {
                if (!data.is_alive[i]) continue;

                AIState current_state = data.state[i];

                // --- Combat Action (High Priority if in combat state) ---
                if (current_state == AIState::CHASING || current_state == AIState::PACK_HUNTING) {
                    size_t target_entity_id = data.target_id[i];

                    if (target_entity_id != (size_t)-1 && target_entity_id < data.getEntityCount() && data.is_alive[target_entity_id]) {
                        int dx = std::abs(data.x[i] - data.x[target_entity_id]);
                        int dy = std::abs(data.y[i] - data.y[target_entity_id]);

                        // --- MODIFIED COMBAT CONDITION ---
                        // Allow combat if entities are adjacent OR on the same tile.
                        // The old condition (dx > 0 || dy > 0) prevented combat on the same tile, causing a deadlock.
                        if (dx <= 1 && dy <= 1)
                        {
                            // --- Attack ---
                            float damage_to_deal = data.current_damage[i];
                            MetabolismSystem::applyDamage(data, target_entity_id, damage_to_deal);

                            // --- Energy Gain from Kill (if target died) ---
                            // Only gain energy if the target is dead AFTER this action.
                            // And only if the killer's type is NOT the same as the target's type (no cannibalism energy gain for territorial fights).
                            if (!data.is_alive[target_entity_id] && data.type[i] != data.type[target_entity_id]) {
                                // --- NEW: Nutritional Value Calculation ---
                                int age_of_prey = data.age[target_entity_id];
                                int prime_age_of_prey = data.prime_age[target_entity_id];
                                float base_value = data.base_nutritional_value[target_entity_id];
                                float penalty = data.penalty_per_year[target_entity_id];
                                float min_value = data.minimum_nutritional_value[target_entity_id];

                                float nutritional_value = base_value;
                                if (age_of_prey > prime_age_of_prey) {
                                    nutritional_value -= (age_of_prey - prime_age_of_prey) * penalty;
                                }
                                nutritional_value = std::max(min_value, nutritional_value);

                                data.energy[i] += nutritional_value;
                                data.energy[i] = std::min(data.energy[i], data.max_energy[i]); // Cap energy
                            }
                        }
                    }
                }

                // --- Resource Consumption Action ---
                if (current_state == AIState::SEEKING_FOOD) {
                    int x = data.x[i];
                    int y = data.y[i];

                    // Check if the entity is on its target tile
                    if (x == data.target_x[i] && y == data.target_y[i]) {
                        Tile& tile = world.getTile(x, y);

                        if (tile.resource_type && tile.getConsumableAmount() > 0.0f) {
                            // Consume a fixed amount per turn, e.g., 1.0f unit of resource
                            float amount_to_consume = 1.0f;
                            float consumed = tile.consume(amount_to_consume);
                            data.energy[i] += consumed * tile.resource_type->nutritional_value;
                            data.energy[i] = std::min(data.energy[i], data.max_energy[i]); // Cap energy

                            // If food is gone, go back to wandering
                            if (tile.getConsumableAmount() <= 0.0f) {
                                data.state[i] = AIState::WANDERING;
                            }
                        }
                    }
                }
        } // End loop over entities
    } // End run function

} // End namespace ActionSystem

namespace ReproductionSystem {

    void run(EntityManager& data) {
        size_t current_entity_count = data.getEntityCount(); // Cache count before adding new ones
        for (size_t i = 0; i < current_entity_count; ++i) {
            if (!data.is_alive[i]) continue;

            float reproduce_energy_percentage = 0.0f;
            int min_reproduce_age = 0;
            float reproduce_cost = 0.0f;

            switch (data.type[i]) {
                case AnimalType::HERBIVORE:
                    reproduce_energy_percentage = HERBIVORE_REPRODUCE_ENERGY_PERCENTAGE;
                    min_reproduce_age = HERBIVORE_MIN_REPRODUCE_AGE;
                    reproduce_cost = HERBIVORE_REPRODUCE_ENERGY_COST;
                    break;
                case AnimalType::CARNIVORE:
                    reproduce_energy_percentage = CARNIVORE_REPRODUCE_ENERGY_PERCENTAGE;
                    min_reproduce_age = CARNIVORE_MIN_REPRODUCE_AGE;
                    reproduce_cost = CARNIVORE_REPRODUCE_ENERGY_COST;
                    break;
                case AnimalType::OMNIVORE:
                    reproduce_energy_percentage = OMNIVORE_REPRODUCE_ENERGY_PERCENTAGE;
                    min_reproduce_age = OMNIVORE_MIN_REPRODUCE_AGE;
                    reproduce_cost = OMNIVORE_REPRODUCE_ENERGY_COST;
                    break;
            }

            if (data.age[i] > min_reproduce_age && data.energy[i] > data.max_energy[i] * reproduce_energy_percentage) {
                data.energy[i] -= reproduce_cost;
                // Create a new entity of the same type
                switch (data.type[i]) {
                    case AnimalType::HERBIVORE:
                        data.createHerbivore(data.x[i], data.y[i]);
                        break;
                    case AnimalType::CARNIVORE:
                        data.createCarnivore(data.x[i], data.y[i]);
                        break;
                    case AnimalType::OMNIVORE:
                        data.createOmnivore(data.x[i], data.y[i]);
                        break;
                }
            }
        }
    } // End run function

} // End namespace ReproductionSystem