#ifndef ANIMATION_SYSTEM_H
#define ANIMATION_SYSTEM_H

#include "core/EntityManager.h"

namespace AnimationSystem {
    // Function to capture the current positions as the previous positions
    void capturePreviousPositions(EntityManager& data);
}

#endif // ANIMATION_SYSTEM_H
