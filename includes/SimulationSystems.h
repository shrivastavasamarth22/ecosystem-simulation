#ifndef SIMULATION_SYSTEMS_H
#define SIMULATION_SYSTEMS_H

#include "EntityManager.h"
#include "World.h"
#include <cstddef>

// We can organize related logic within namespaces

namespace MetablismSystem {
    // Function to run the metabolic logic for all entities
    void run(EntityManager& data);

    // Helper function (optional, can keep logic in run) - handles damage application
    // Need a way to apply damage in the new system without an Animal object.
    // Damage will be applied directly in the Action System later.
    // Let's add a simple helper function for applying damage to a specific entity ID.
    void applyDamage(EntityManager& data, size_t entity_id, int amount);
}

// Declare other systems here later:
// namespace AISystem { ... }
// namespace MovementSystem { ... }
// namespace ActionSystem { ... }
// namespace ReproductionSystem { ... }

#endif // SIMULATION_SYSTEMS_H