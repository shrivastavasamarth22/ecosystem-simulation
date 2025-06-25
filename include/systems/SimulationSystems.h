#ifndef SIMULATION_SYSTEMS_H
#define SIMULATION_SYSTEMS_H

#include "core/EntityManager.h"
#include "core/World.h"
#include <cstddef>

// We can organize related logic within namespaces

namespace MetabolismSystem {
    // Function to run the metabolic logic for all entities
    void run(EntityManager& data);

    // Helper function (optional, can keep logic in run) - handles damage application
    // Need a way to apply damage in the new system without an Animal object.
    // Damage will be applied directly in the Action System later.
    // Let's add a simple helper function for applying damage to a specific entity ID.
    void applyDamage(EntityManager& data, size_t entity_id, float amount);
}

namespace MovementSystem {
    // Function to run the movement logic for all entities
    // Needs World to check boundaries and access the spatial grid
    void run(EntityManager& data, const World& world);

    // Helper functions for movement logic operating directly on EntityManager data
    // We pass entity_id to know which entity to move.
    void moveTowards(EntityManager& data, size_t entity_id, const World& world, int target_x_coord, int target_y_coord);
    void moveAwayFrom(EntityManager& data, size_t entity_id, const World& world, int target_x_coord, int target_y_coord);
    void moveRandom(EntityManager& data, size_t entity_id, const World& world);
}

namespace AISystem {
    void run(EntityManager& data, const World& world);
}

namespace ActionSystem {
    void run(EntityManager& data, World& world);
}

namespace ReproductionSystem {
    void run(EntityManager& data);
}

#endif // SIMULATION_SYSTEMS_H
