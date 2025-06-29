#include "resources/Terrain.h"

// Normal terrain - no restrictions or modifiers
const TerrainType TERRAIN_NORMAL = {
    "Normal",
    1.0f,        // Normal speed
    1.0f,        // Normal sight
    {},          // All animal types allowed (empty set)
    '.',         // Symbol
    sf::Color(139, 69, 19) // Brown earth color
};

// Water terrain - slows down all entities
const TerrainType TERRAIN_WATER = {
    "Water",
    0.5f,        // Half speed
    1.0f,        // Normal sight
    {},          // All animal types allowed
    '~',         // Water symbol
    sf::Color(0, 100, 200) // Blue water color
};

// Rocky terrain - only carnivores and herbivores can traverse
const TerrainType TERRAIN_ROCKY = {
    "Rocky",
    1.0f,        // Normal speed
    1.0f,        // Normal sight
    {AnimalType::CARNIVORE, AnimalType::HERBIVORE}, // Only carnivores and herbivores
    '^',         // Mountain/rock symbol
    sf::Color(120, 120, 120) // Gray rock color
};
