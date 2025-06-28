#ifndef METABOLISM_SYSTEM_H
#define METABOLISM_SYSTEM_H

#include "core/EntityManager.h"
#include "core/World.h"
#include <cstddef>

namespace MetabolismSystem {
    // Function to run the metabolic logic for all entities
    void run(EntityManager& data, const World& world);

    // Helper function for applying damage to a specific entity ID
    void applyDamage(EntityManager& data, size_t entity_id, float amount);
}

#endif // METABOLISM_SYSTEM_H
