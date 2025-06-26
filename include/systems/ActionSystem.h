#ifndef ACTION_SYSTEM_H
#define ACTION_SYSTEM_H

#include "core/EntityManager.h"
#include "core/World.h"

namespace ActionSystem {
    // Function to run the action logic for all entities (combat, resource consumption)
    void run(EntityManager& data, World& world);
}

#endif // ACTION_SYSTEM_H
