#include "systems/AnimationSystem.h"

namespace AnimationSystem {

    void capturePreviousPositions(EntityManager& data) {
        size_t num_entities = data.getEntityCount();

        // This can be parallelized as each operation is independent.
        #pragma omp parallel for
        for (size_t i = 0; i < num_entities; ++i) {
            data.prev_x[i] = data.x[i];
            data.prev_y[i] = data.y[i];
        }
    }

}
