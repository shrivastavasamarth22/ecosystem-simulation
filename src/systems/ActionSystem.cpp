#include "systems/ActionSystem.h"
#include "systems/MetabolismSystem.h"
#include "common/AnimalTypes.h"
#include <cmath>
#include <algorithm>

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

                    // Validate target is still alive and valid
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
                    } else {
                        // Target is dead or invalid - switch to wandering and clear target
                        data.state[i] = AIState::WANDERING;
                        data.target_id[i] = (size_t)-1;
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

                            // If food is gone, go back to wandering and clear target
                            if (tile.getConsumableAmount() <= 0.0f) {
                                data.state[i] = AIState::WANDERING;
                                data.target_x[i] = -1;
                                data.target_y[i] = -1;
                            }
                        } else {
                            // Reached target but no food available - clear target and wander
                            data.state[i] = AIState::WANDERING;
                            data.target_x[i] = -1;
                            data.target_y[i] = -1;
                        }
                    }
                }
        } // End loop over entities
    } // End run function

} // End namespace ActionSystem
