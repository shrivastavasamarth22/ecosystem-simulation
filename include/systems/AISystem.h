#ifndef AI_SYSTEM_H
#define AI_SYSTEM_H

#include "core/EntityManager.h"
#include "core/World.h"

namespace AISystem {
    // Function to run the AI decision-making logic for all entities
    void run(EntityManager& data, const World& world);
}

#endif // AI_SYSTEM_H
