#include "resources/Biome.h"
#include "resources/Terrain.h"

// --- Biome Definitions ---

// Water: Consists of only water terrain, no resources
const BiomeType BIOME_WATER = {
    "Water", // name
    {        // terrain_distribution
        {&TERRAIN_WATER, 1.0f} // 100% water terrain
    },
    {        // resource_distribution (empty - no resources spawn in water)
    },
    sf::Color(0, 100, 200) // Deep blue water color
};

// Barren: Mostly empty normal terrain, some grass
const BiomeType BIOME_BARREN = {
    "Barren", // name
    {         // terrain_distribution
        {&TERRAIN_NORMAL, 1.0f} // 100% normal terrain
    },
    {         // resource_distribution
        {&RESOURCE_GRASS, 0.10f} // 10% chance of grass
    },
    sf::Color(188, 143, 143) // A dusty, rocky color
};

// Rocky: Only rock terrain, no resources
const BiomeType BIOME_ROCKY = {
    "Rocky", // name
    {        // terrain_distribution
        {&TERRAIN_ROCKY, 1.0f} // 100% rocky terrain
    },
    {        // resource_distribution (empty - no resources spawn on rocks)
    },
    sf::Color(120, 120, 120) // Gray rock color
};

// Grassland: Normal terrain with grass, some bushes, rarely berries
const BiomeType BIOME_GRASSLAND = {
    "Grassland", // name
    {            // terrain_distribution
        {&TERRAIN_NORMAL, 1.0f} // 100% normal terrain
    },
    {            // resource_distribution
        {&RESOURCE_GRASS, 0.70f},   // 70% chance of grass
        {&RESOURCE_BUSH, 0.20f},    // 20% chance of bush
        {&RESOURCE_BERRIES, 0.05f}  // 5% chance of berries
    },
    sf::Color(126, 200, 80) // A lush green color
};

// Forest: Normal terrain with mostly bushes, some grass, few berries (hinders sight)
const BiomeType BIOME_FOREST = {
    "Forest", // name
    {         // terrain_distribution
        {&TERRAIN_NORMAL, 1.0f} // 100% normal terrain (sight modifier applied in systems)
    },
    {         // resource_distribution
        {&RESOURCE_BUSH, 0.50f},    // 50% chance of bush
        {&RESOURCE_GRASS, 0.25f},   // 25% chance of grass
        {&RESOURCE_BERRIES, 0.10f}  // 10% chance of berries
    },
    sf::Color(34, 139, 34) // A deep forest green
};

// Fertile: Normal terrain with mainly berries, some bush and grass
const BiomeType BIOME_FERTILE = {
    "Fertile", // name
    {          // terrain_distribution
        {&TERRAIN_NORMAL, 1.0f} // 100% normal terrain
    },
    {          // resource_distribution
        {&RESOURCE_BERRIES, 0.60f}, // 60% chance of berries
        {&RESOURCE_BUSH, 0.25f},    // 25% chance of bush
        {&RESOURCE_GRASS, 0.15f}    // 15% chance of grass
    },
    sf::Color(255, 215, 0) // Golden fertile color
};
