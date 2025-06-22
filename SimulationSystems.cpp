#include "includes/SimulationSystems.h"
#include "includes/AnimalConfig.h" // Need constants
#include "includes/World.h"
#include <algorithm>
#include <cmath>
#include <random>

extern std::mt19937 rng;
namespace MetabolismSystem {
    // Helper function to apply damage directly to entity data
    void applyDamage(EntityManager& data, size_t entity_id, int amount) {
        if (entity_id >= data.getEntityCount() || !data.is_alive[entity_id]) {
            return; // Cannot damage invalid or dead entity
        }

        data.health[entity_id] -= amount;
        data.turns_since_damage[entity_id] = 0; // Reset regen timer

        if (data.health[entity_id] <= 0) {
            data.is_alive[entity_id] = false; // Mark as dead
            data.health[entity_id] = 0;       // Ensure health is 0 when dead
            // Actual removal from vectors happens in EntityManager::destroyDeadEntities
        }
    }

    void run(EntityManager& data) {
        size_t num_entities = data.getEntityCount();

        // Loop through all entities to apply metabolism rules
        for (size_t i = 0; i < num_entities; ++i) {
            // Only apply metabolism to living entities
            if (!data.is_alive[i]) continue;

            // --- Aging ---
            data.age[i]++;
            // Aging penalties will be applied by a separate Aging System later,
            // which will modify base_stats based on age.

            // --- Base Energy Cost of Living ---
            data.energy[i]--;


            // --- HUNGER SYSTEM ---
            // Reset current stats to potentially aged base stats (This should happen in AgingSystem or its own stat update system)
            // For now, let's keep the stats update logic within Metabolism run for simplicity in this step.
            data.current_damage[i] = data.base_damage[i];
            data.current_speed[i] = data.base_speed[i];
            data.current_sight_radius[i] = data.base_sight_radius[i];


            if (data.max_energy[i] > 0) { // Avoid division by zero
                float energy_percentage = static_cast<float>(data.energy[i]) / data.max_energy[i];

                if (energy_percentage < 0.3f) { // Desperate state
                    MetabolismSystem::applyDamage(data, i, 5); // Starvation damage
                    if (!data.is_alive[i]) continue; // Stop if starved to death

                    data.current_damage[i] = std::max(0, data.current_damage[i] + 7);
                    data.current_speed[i] = std::max(1, data.current_speed[i] + 2);
                    data.current_sight_radius[i] = std::max(1, data.current_sight_radius[i] + 3);
                } else if (energy_percentage < 0.5f) { // Hungry state
                    MetabolismSystem::applyDamage(data, i, 2); // Starvation damage
                    if (!data.is_alive[i]) continue; // Stop if starved to death

                    data.current_damage[i] = std::max(0, data.current_damage[i] + 2);
                    data.current_speed[i] = std::max(1, data.current_speed[i] + 1);
                    data.current_sight_radius[i] = std::max(1, data.current_sight_radius[i] + 1);
                }
            }
            // Death from hunger is handled by applyDamage setting is_alive = false.
            // The actual destruction will happen later in EntityManager cleanup.


            // --- HEALTH REGENERATION (4 Tiers) ---
            data.turns_since_damage[i]++;
            const int REGEN_DELAY_TURNS = 3;
            const int REGEN_AMOUNT = 1;

            if (data.turns_since_damage[i] > REGEN_DELAY_TURNS) {
                int actual_regen_cap;
                float current_health_percentage = static_cast<float>(data.health[i]) / data.max_health[i];

                if (current_health_percentage >= 0.90f) actual_regen_cap = data.max_health[i];
                else if (current_health_percentage >= 0.75f) actual_regen_cap = static_cast<int>(data.max_health[i] * 0.90f);
                else if (current_health_percentage >= 0.50f) actual_regen_cap = static_cast<int>(data.max_health[i] * 0.75f);
                else actual_regen_cap = static_cast<int>(data.max_health[i] * 0.60f);

                if (data.health[i] < actual_regen_cap) {
                    data.health[i] += REGEN_AMOUNT;
                    data.health[i] = std::min(data.health[i], actual_regen_cap); // Clamp to the determined cap
                }
            }

            // --- Cap Energy ---
            // Ensure energy doesn't exceed max after grazing/eating or fall below 0 passively
            data.energy[i] = std::max(0, data.energy[i]);
            data.energy[i] = std::min(data.energy[i], data.max_energy[i]);

        } // End loop over entities
    } // End run function
}

namespace MovementSystem {

    // Helper function to move towards a coordinate target
    void moveTowards(EntityManager& data, size_t entity_id, const World& world, int target_x_coord, int target_y_coord) {
        if (!data.is_alive[entity_id]) return;

        for (int i = 0; i < data.current_speed[entity_id]; ++i) {
            int dx = target_x_coord - data.x[entity_id];
            int dy = target_y_coord - data.y[entity_id];

            int move_dx = 0;
            int move_dy = 0;

            if (std::abs(dx) > std::abs(dy)) {
                move_dx = (dx > 0) ? 1 : -1;
            } else if (dy != 0) {
                move_dy = (dy > 0) ? 1 : -1;
            } else if (dx != 0) {
                 move_dx = (dx > 0) ? 1 : -1;
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

    // Helper function to move away from a coordinate target
    void moveAwayFrom(EntityManager& data, size_t entity_id, const World& world, int target_x_coord, int target_y_coord) {
        if (!data.is_alive[entity_id]) return;

        for (int i = 0; i < data.current_speed[entity_id]; ++i) {
            int dx = data.x[entity_id] - target_x_coord;
            int dy = data.y[entity_id] - target_y_coord;

            int move_dx = 0;
            int move_dy = 0;

            if (std::abs(dx) > std::abs(dy)) {
                move_dx = (dx > 0) ? 1 : -1;
            } else if (dy != 0) {
                move_dy = (dy > 0) ? 1 : -1;
            } else if (dx != 0) {
                move_dx = (dx > 0) ? 1 : -1;
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

        // Loop through all entities to apply movement based on their state
        for (size_t i = 0; i < num_entities; ++i) {
            if (!data.is_alive[i]) continue;

            AIState current_state = data.state[i];

            switch (current_state) {
                case AIState::FLEEING:
                case AIState::CHASING:
                case AIState::PACK_HUNTING:
                case AIState::HERDING:
                {
                    // These states target an animal ID
                    size_t target_entity_id = data.target_id[i];
                    // Check if target is valid and alive
                    if (target_entity_id != (size_t)-1 && target_entity_id < data.getEntityCount() && data.is_alive[target_entity_id]) {
                        // Get target coordinates from the entity manager data
                        int target_x = data.x[target_entity_id];
                        int target_y = data.y[target_entity_id];

                        if (current_state == AIState::FLEEING) {
                            moveAwayFrom(data, i, world, target_x, target_y);
                        } else { // CHASING, PACK_HUNTING, HERDING move TOWARDS animal target
                            moveTowards(data, i, world, target_x, target_y);
                        }
                    } else {
                        // Target is invalid/dead - AI system should ideally handle state transition.
                        // For robustness, if somehow state is targeting an invalid entity,
                        // we can default to random movement or wandering.
                        // Let's change state to wandering so AI re-evaluates next turn.
                        data.state[i] = AIState::WANDERING;
                        data.target_id[i] = (size_t)-1; // Clear invalid target
                        // Fall through to WANDERING case or call moveRandom? Let's moveRandom.
                        moveRandom(data, i, world);
                    }
                }
                break; // Break from the state block

                case AIState::SEEKING_FOOD:
                {
                    // This state targets coordinates
                    int target_x_coord = data.target_x[i];
                    int target_y_coord = data.target_y[i];

                    // Check if coordinate target is valid (-1 means invalid/no target)
                    if (target_x_coord != -1 && target_y_coord != -1) {
                        // If we reached the target tile, the AI System should ideally change state.
                        // For robustness, if state is SEEKING_FOOD but already on the tile,
                        // default to random movement or wandering.
                        if (data.x[i] == target_x_coord && data.y[i] == target_y_coord) {
                            // Reached target, AI should handle state transition.
                            // Defaulting to random movement here.
                            moveRandom(data, i, world);
                        } else {
                            // Move towards the coordinate target
                            moveTowards(data, i, world, target_x_coord, target_y_coord);
                        }
                    } else {
                        // Target coordinates invalid - revert state to wander.
                        data.state[i] = AIState::WANDERING;
                        data.target_x[i] = -1; // Clear invalid target
                        data.target_y[i] = -1;
                        moveRandom(data, i, world);
                    }
                }
                break; // Break from the state block

                case AIState::WANDERING:
                    // Move randomly
                    moveRandom(data, i, world);
                    break;

                default:
                    // Should not happen, but as a fallback
                    moveRandom(data, i, world);
                    break;
            }
        } // End loop over entities
    } // End run function

} // End namespace MovementSystem


namespace AISystem {

    void run(EntityManager& data, const World& world) {
        size_t num_entities = data.getEntityCount();

        // Loop through all entities to update their AI state and target
        for (size_t i = 0; i < num_entities; ++i) {
                if (!data.is_alive[i]) continue;

                // Reset targets from previous turn
                data.target_id[i] = (size_t)-1;
                data.target_x[i] = -1;
                data.target_y[i] = -1;

                // --- DECLARE FOOD SEEKING VARIABLES HERE ---
                int best_food_amount = 0;
                int food_x = -1;
                int food_y = -1;

                // --- DECLARE HERD SEEKING VARIABLES HERE (Herbivore only) ---
                // Need herd_size outside the block if used later
                int herd_size = 0;


                switch (data.type[i]) {
                    case AnimalType::HERBIVORE:
                    {
                        // Herd Buff Calculation (still in AISystem for now)
                        // Recalculate herd_size here for the buff
                        auto nearby_friends = world.getAnimalsNear(data, data.x[i], data.y[i], HERD_BONUS_RADIUS, AnimalType::HERBIVORE);
                        herd_size = nearby_friends.size(); // <-- Use the declared variable

                        int hp_bonus = (herd_size > 1) ? (herd_size - 1) * HERD_HP_BONUS_PER_MEMBER : 0;
                        int old_max_health = data.max_health[i];
                        data.max_health[i] = data.base_max_health[i] + hp_bonus;
                        if (data.max_health[i] > old_max_health) data.health[i] += (data.max_health[i] - old_max_health);
                        data.health[i] = std::min(data.health[i], data.max_health[i]);

                        // Decision Making
                        auto predators = world.getAnimalsNear(data, data.x[i], data.y[i], data.current_sight_radius[i], AnimalType::CARNIVORE);
                        auto omni_predators = world.getAnimalsNear(data, data.x[i], data.y[i], data.current_sight_radius[i], AnimalType::OMNIVORE);
                        predators.insert(predators.end(), omni_predators.begin(), omni_predators.end());
                        if (!predators.empty()) {
                            data.state[i] = AIState::FLEEING; data.target_id[i] = predators[0]; continue;
                        }

                        // Seek Food if hungry - Use the declared variables
                        if (data.energy[i] < static_cast<int>(data.max_energy[i] * HERBIVORE_FOOD_SEEK_THRESHOLD_PERCENTAGE)) {
                            best_food_amount = 0; food_x = -1; food_y = -1; // Reset for this scan
                            int scan_radius = data.current_sight_radius[i];
                            for (int dy = -scan_radius; dy <= scan_radius; ++dy) { for (int dx = -scan_radius; dx <= scan_radius; ++dx) {
                                    int check_x = data.x[i] + dx; int check_y = data.y[i] + dy;
                                    if (check_x >= 0 && check_x < world.getWidth() && check_y >= 0 && check_y < world.getHeight() && dx * dx + dy * dy <= scan_radius * scan_radius) {
                                        const Tile& tile = world.getTile(check_x, check_y);
                                        if (tile.resource_type && tile.resource_amount > best_food_amount) {
                                            best_food_amount = tile.resource_amount; food_x = check_x; food_y = check_y;
                                        }
                                    }
                                }
                            }
                            if (best_food_amount > 0) {
                                data.state[i] = AIState::SEEKING_FOOD; data.target_x[i] = food_x; data.target_y[i] = food_y; continue;
                            }
                        }

                        // Seek out a herd if not in one - Use the calculated herd_size
                        if (herd_size <= 1) {
                            auto potential_herd = world.getAnimalsNear(data, data.x[i], data.y[i], HERD_DETECTION_RADIUS, AnimalType::HERBIVORE);
                            if (potential_herd.size() > 1) {
                                data.state[i] = AIState::HERDING;
                                for(size_t potential_target_id : potential_herd) { if (potential_target_id != i) { data.target_id[i] = potential_target_id; break; } } continue;
                            }
                        }

                        data.state[i] = AIState::WANDERING;
                    } break;


                    case AnimalType::CARNIVORE:
                    {
                        // ... (Carnivore AI logic remains the same, uses existing variables) ...
                        auto nearby_omnivores = world.getAnimalsNear(data, data.x[i], data.y[i], data.current_sight_radius[i], AnimalType::OMNIVORE);
                        int pack_check_radius = 2; auto nearby_omnivores_for_pack_check = world.getAnimalsNear(data, data.x[i], data.y[i], pack_check_radius, AnimalType::OMNIVORE);
                        if(nearby_omnivores_for_pack_check.size() >= OMNIVORE_PACK_THREAT_SIZE) {
                            data.state[i] = AIState::FLEEING; data.target_id[i] = nearby_omnivores_for_pack_check[0]; continue;
                        }
                        auto nearby_herbivores = world.getAnimalsNear(data, data.x[i], data.y[i], data.current_sight_radius[i], AnimalType::HERBIVORE);
                        if (!nearby_herbivores.empty()) {
                            data.state[i] = AIState::CHASING; data.target_id[i] = nearby_herbivores[0]; continue;
                        }
                        if (!nearby_omnivores.empty()) { //nearby_omnivores from sight_radius, check if not pack threat by size
                            data.state[i] = AIState::CHASING; data.target_id[i] = nearby_omnivores[0]; continue;
                        }
                        data.state[i] = AIState::WANDERING;
                    } break;


                    case AnimalType::OMNIVORE:
                    {
                        auto nearby_carnivores_for_pack_check = world.getAnimalsNear(data, data.x[i], data.y[i], 2, AnimalType::CARNIVORE);
                        if(nearby_carnivores_for_pack_check.size() >= OMNIVORE_PACK_HUNT_SIZE) {
                            data.state[i] = AIState::FLEEING; data.target_id[i] = nearby_carnivores_for_pack_check[0]; continue;
                        }
                        auto nearby_herbivores = world.getAnimalsNear(data, data.x[i], data.y[i], data.current_sight_radius[i], AnimalType::HERBIVORE);
                        if (!nearby_herbivores.empty()) {
                            data.state[i] = AIState::CHASING; data.target_id[i] = nearby_herbivores[0]; continue;
                        }

                        // Seek Grass if hungry - Use the declared variables
                        if (data.energy[i] < static_cast<int>(data.max_energy[i] * OMNIVORE_FOOD_SEEK_THRESHOLD_PERCENTAGE)) {
                            best_food_amount = 0; food_x = -1; food_y = -1; // Reset for this scan
                            int scan_radius = data.current_sight_radius[i];
                            for (int dy = -scan_radius; dy <= scan_radius; ++dy) { for (int dx = -scan_radius; dx <= scan_radius; ++dx) {
                                    int check_x = data.x[i] + dx; int check_y = data.y[i] + dy;
                                    if (check_x >= 0 && check_x < world.getWidth() && check_y >= 0 && check_y < world.getHeight() && dx * dx + dy * dy <= scan_radius * scan_radius) {
                                        const Tile& tile = world.getTile(check_x, check_y);
                                        if (tile.resource_type && tile.resource_amount > best_food_amount) {
                                            best_food_amount = tile.resource_amount; food_x = check_x; food_y = check_y;
                                        }
                                    }
                                }
                            }
                            if (best_food_amount > 0) {
                                data.state[i] = AIState::SEEKING_FOOD; data.target_x[i] = food_x; data.target_y[i] = food_y; continue;
                            }
                        }
                        auto nearby_carnivores_for_hunt = world.getAnimalsNear(data, data.x[i], data.y[i], data.current_sight_radius[i], AnimalType::CARNIVORE);
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

        for (size_t i = 0; i < num_entities; ++i) {
             if (!data.is_alive[i]) continue;

             AIState current_state = data.state[i];

             // --- Combat Action (High Priority if in combat state) ---
             if (current_state == AIState::CHASING || current_state == AIState::PACK_HUNTING) {
                 size_t target_entity_id = data.target_id[i];

                 // Check if target is valid, alive, and adjacent
                 if (target_entity_id != (size_t)-1 && target_entity_id < data.getEntityCount() && data.is_alive[target_entity_id]) {
                      int dx = std::abs(data.x[i] - data.x[target_entity_id]);
                      int dy = std::abs(data.y[i] - data.y[target_entity_id]);

                      // If adjacent (including diagonally)
                      if (dx <= 1 && dy <= 1 && (dx > 0 || dy > 0))
                      {
                          // --- Attack ---
                          int damage_to_deal = data.current_damage[i];
                          MetabolismSystem::applyDamage(data, target_entity_id, damage_to_deal);

                          // --- Energy Gain from Kill (if target died) ---
                          // Check target's health *after* damage is applied this turn
                          if (!data.is_alive[target_entity_id]) {
                              // Calculate nutritional value of the killed entity using its ID
                              int target_age = data.age[target_entity_id];
                              int target_base_nut = data.base_nutritional_value[target_entity_id];
                              int target_prime_age = data.prime_age[target_entity_id];
                              int target_penalty = data.penalty_per_year[target_entity_id];
                              int target_min_value = data.minimum_nutritional_value[target_entity_id];

                              int age_penalty = (target_age > target_prime_age) ? (target_age - target_prime_age) * target_penalty : 0;
                              int energy_gained = std::max(target_min_value, target_base_nut - age_penalty);

                              // Add energy to the attacker, clamping at max
                              data.energy[i] = std::min(data.energy[i] + energy_gained, data.max_energy[i]);

                              // Note: AI system will handle clearing the target ID next turn when it sees the target is dead.
                          }
                      }
                 }
             }
             // --- End Combat Action ---


             // --- Passive Resource Consumption (Happens if on food tile and hungry, regardless of most states) ---
             // Only exclude fleeing, as fleeing animals shouldn't stop to eat.
             if (current_state != AIState::FLEEING && data.energy[i] < data.max_energy[i]) {
                 // Get the tile at the entity's current location using World accessors
                 Tile& current_tile = world.getTile(data.x[i], data.y[i]);

                 // Check if the tile has a resource and if it's consumable
                 if (current_tile.resource_type && current_tile.resource_amount > 0) {
                     // Consume the resource from the tile
                     const int RESOURCE_CONSUMPTION_AMOUNT_REQUESTED = 5; // Tunable
                     int amount_consumed = current_tile.consume(RESOURCE_CONSUMPTION_AMOUNT_REQUESTED);

                     // Calculate energy gained
                     int energy_gained = amount_consumed * current_tile.resource_type->nutritional_value;

                     // Add energy, clamping to max_energy
                     data.energy[i] = std::min(data.energy[i] + energy_gained, data.max_energy[i]);

                     // --- State Change Logic (if SEEKING_FOOD and reached target TILE) ---
                     // If the entity was seeking food and is now on the target tile AND just ate
                     // This logic is here because it depends on both the state AND the consumption event.
                     if (current_state == AIState::SEEKING_FOOD && energy_gained > 0 && // Only if food was actually consumed
                         data.target_x[i] != -1 && data.target_y[i] != -1 &&
                         data.x[i] == data.target_x[i] && data.y[i] == data.target_y[i])
                     {
                         // Reached target food tile and consumed - decide next state (wander/herd)
                         // To decide HERDING, need to check nearby friends again.
                         // AISystem will handle this check next turn based on the entity's new position.
                         // For now, just switch to wandering and clear the coordinate target.
                         data.state[i] = AIState::WANDERING; // Switch to wander state
                         data.target_x[i] = -1; // Clear coordinate target
                         data.target_y[i] = -1;
                         data.target_id[i] = (size_t)-1; // Ensure animal target is also clear
                     }
                 }
             }
             // --- End Passive Consumption ---

             // Other states like FLEEING, WANDERING, HERDING only involve movement,
             // which is handled by the Movement System. No action logic needed here.
             // SEEKING_FOOD involves movement + consumption, consumption handled above.

             // No state-based switch needed for the rest, as actions (combat/consume) are handled above.
             // The state primarily dictates movement (handled by MovementSystem).
             // A default case is not strictly needed if all states are implicitly handled by the logic above.
             // However, if you want to be explicit or print errors for unknown states, you could add one.
        } // End loop over entities
    } // End run function

} // End namespace ActionSystem

namespace ReproductionSystem {

    void run(EntityManager& data) {
        size_t num_entities_before_reproduction = data.getEntityCount(); // Process based on count *before* adding

        // We cannot add entities while iterating with index i directly
        // because it invalidates indices.
        // Reproduction creates new entities at the end of vectors.
        // A common pattern is to collect reproduction requests and then process them.

        // This simple loop processes the CURRENT state of entities up to the count *before* reproduction.
        // New entities created this turn will be processed in the *next* turn.
        for (size_t i = 0; i < num_entities_before_reproduction; ++i) {
            if (!data.is_alive[i]) continue;

            // Check if the entity meets reproduction criteria based on its type
            bool can_reproduce = false;
            int energy_cost = 0;
            int required_energy = 0;
            int required_age = 0;

            switch (data.type[i]) {
                case AnimalType::HERBIVORE:
                    required_energy = static_cast<int>(data.max_energy[i] * HERBIVORE_REPRODUCE_ENERGY_PERCENTAGE);
                    energy_cost = HERBIVORE_REPRODUCE_ENERGY_COST;
                    required_age = HERBIVORE_MIN_REPRODUCE_AGE;
                    if (data.energy[i] > required_energy && data.age[i] > required_age) {
                        can_reproduce = true;
                    }
                    break;
                case AnimalType::CARNIVORE:
                    required_energy = static_cast<int>(data.max_energy[i] * CARNIVORE_REPRODUCE_ENERGY_PERCENTAGE);
                    energy_cost = CARNIVORE_REPRODUCE_ENERGY_COST;
                    required_age = CARNIVORE_MIN_REPRODUCE_AGE;
                    if (data.energy[i] > required_energy && data.age[i] > required_age) {
                        can_reproduce = true;
                    }
                    break;
                case AnimalType::OMNIVORE:
                    required_energy = static_cast<int>(data.max_energy[i] * OMNIVORE_REPRODUCE_ENERGY_PERCENTAGE);
                    energy_cost = OMNIVORE_REPRODUCE_ENERGY_COST;
                    required_age = OMNIVORE_MIN_REPRODUCE_AGE;
                    if (data.energy[i] > required_energy && data.age[i] > required_age) {
                        can_reproduce = true;
                    }
                    break;
                default:
                    // Unknown type, cannot reproduce
                    break;
            }

             // If capable, reproduce and deduct energy
            if (can_reproduce) {
                // Create the new entity using the appropriate helper function
                // They are born at the parent's location.
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
                default:
                    // Should not happen due to can_reproduce check
                    break;
                }

                // Deduct energy cost from the parent AFTER the child is created
                data.energy[i] = std::max(0, data.energy[i] - energy_cost); // Ensure energy doesn't go negative
            }
        } // End loop over entities for reproduction
    } // End run function

} // End namespace ReproductionSystem