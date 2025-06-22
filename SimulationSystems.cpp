#include "includes/SimulationSystems.h"
#include "includes/AnimalConfig.h" // Need constants
#include <algorithm>
#include <cmath>

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