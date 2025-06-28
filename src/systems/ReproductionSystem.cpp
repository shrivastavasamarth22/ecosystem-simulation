#include "systems/ReproductionSystem.h"
#include "common/AnimalConfig.h"
#include "common/AnimalTypes.h"

namespace ReproductionSystem {

    void run(EntityManager& data) {
        size_t current_entity_count = data.getEntityCount(); // Cache count before adding new ones
        for (size_t i = 0; i < current_entity_count; ++i) {
            if (!data.is_alive[i]) continue;

            float reproduce_energy_percentage = 0.0f;
            int min_reproduce_age = 0;
            float reproduce_cost = 0.0f;

            switch (data.type[i]) {
                case AnimalType::HERBIVORE:
                    reproduce_energy_percentage = HERBIVORE_REPRODUCE_ENERGY_PERCENTAGE;
                    min_reproduce_age = HERBIVORE_MIN_REPRODUCE_AGE;
                    reproduce_cost = HERBIVORE_REPRODUCE_ENERGY_COST;
                    break;
                case AnimalType::CARNIVORE:
                    reproduce_energy_percentage = CARNIVORE_REPRODUCE_ENERGY_PERCENTAGE;
                    min_reproduce_age = CARNIVORE_MIN_REPRODUCE_AGE;
                    reproduce_cost = CARNIVORE_REPRODUCE_ENERGY_COST;
                    break;
                case AnimalType::OMNIVORE:
                    reproduce_energy_percentage = OMNIVORE_REPRODUCE_ENERGY_PERCENTAGE;
                    min_reproduce_age = OMNIVORE_MIN_REPRODUCE_AGE;
                    reproduce_cost = OMNIVORE_REPRODUCE_ENERGY_COST;
                    break;
            }

            if (data.age[i] > min_reproduce_age && data.energy[i] > data.max_energy[i] * reproduce_energy_percentage) {
                data.energy[i] -= reproduce_cost;
                // Create a new entity of the same type
                size_t offspring_id;
                switch (data.type[i]) {
                    case AnimalType::HERBIVORE:
                        offspring_id = data.createHerbivore(data.x[i], data.y[i]);
                        break;
                    case AnimalType::CARNIVORE:
                        offspring_id = data.createCarnivore(data.x[i], data.y[i]);
                        // Set parent tracking for carnivores to prevent family conflicts
                        data.parent_id[offspring_id] = i;
                        break;
                    case AnimalType::OMNIVORE:
                        offspring_id = data.createOmnivore(data.x[i], data.y[i]);
                        break;
                }
            }
        }
    } // End run function

} // End namespace ReproductionSystem
