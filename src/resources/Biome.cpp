#include "resources/Biome.h"

// --- Biome Definitions ---

// Grassland: Abundant grass, some berries.
const BiomeType BIOME_GRASSLAND = {
    "Grassland", // name
    {            // resource_distribution
        {&RESOURCE_GRASS, 0.80f},   // 80% chance of grass
        {&RESOURCE_BERRIES, 0.15f} // 15% chance of berries
    },
    sf::Color(126, 200, 80) // A lush green color
};

// Forest: Abundant berries, some grass. Rarest biome.
const BiomeType BIOME_FOREST = {
    "Forest",    // name
    {            // resource_distribution
        {&RESOURCE_GRASS, 0.25f},   // 25% chance of grass
        {&RESOURCE_BERRIES, 0.70f} // 70% chance of berries
    },
    sf::Color(34, 139, 34) // A deep forest green
};

// Barren: Mostly empty, sparse grass, very few berries. Most common biome.
const BiomeType BIOME_BARREN = {
    "Barren",    // name
    {            // resource_distribution
        {&RESOURCE_GRASS, 0.10f},   // 10% chance of grass
        {&RESOURCE_BERRIES, 0.02f} // 2% chance of berries
    },
    sf::Color(188, 143, 143) // A dusty, rocky color
};
