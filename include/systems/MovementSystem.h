#ifndef MOVEMENT_SYSTEM_H
#define MOVEMENT_SYSTEM_H

#include "core/EntityManager.h"
#include "core/World.h"
#include <cstddef>

namespace MovementSystem {
    // Function to run the movement logic for all entities
    void run(EntityManager& data, const World& world);

    // Helper functions for movement logic operating directly on EntityManager data
    void moveTowards(EntityManager& data, size_t entity_id, const World& world, int target_x_coord, int target_y_coord);
    void moveAwayFrom(EntityManager& data, size_t entity_id, const World& world, int target_x_coord, int target_y_coord);
    void moveRandom(EntityManager& data, size_t entity_id, const World& world);
}

#endif // MOVEMENT_SYSTEM_H
