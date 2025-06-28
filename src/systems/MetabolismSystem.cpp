#include "systems/MetabolismSystem.h"
#include "common/AnimalConfig.h"
#include <algorithm>

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

            // Check for starvation death - animals die when energy reaches 0 or below
            if (data.energy[i] <= 0.0f) {
                data.is_alive[i] = false; // Mark as dead from starvation
                data.health[i] = 0.0f;    // Set health to 0 for consistency
                data.energy[i] = 0.0f;    // Ensure energy doesn't go negative
                continue; // Skip further processing for this dead entity
            }

            // Calculate aging penalties for stats
            float age_penalty_factor = 1.0f; // Start with no penalty
            if (data.age[i] > data.prime_age[i]) {
                // Calculate how many years past prime age
                int years_past_prime = data.age[i] - data.prime_age[i];
                // Apply penalty per year (convert to percentage reduction)
                float total_penalty_percentage = years_past_prime * (data.penalty_per_year[i] / 100.0f);
                // Cap penalty at 50% to prevent stats from becoming too low
                total_penalty_percentage = std::min(total_penalty_percentage, 0.5f);
                age_penalty_factor = 1.0f - total_penalty_percentage;
            }

            // Apply aging penalties to current stats
            data.current_damage[i] = data.base_damage[i] * age_penalty_factor;
            data.current_speed[i] = std::max(1.0f, data.base_speed[i] * age_penalty_factor); // Minimum speed of 1
            data.current_sight_radius[i] = std::max(1.0f, data.base_sight_radius[i] * age_penalty_factor); // Minimum sight of 1

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
