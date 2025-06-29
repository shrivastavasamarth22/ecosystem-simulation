#ifndef TERRAIN_H
#define TERRAIN_H

#include "common/AnimalTypes.h"
#include <SFML/Graphics/Color.hpp>
#include <string>
#include <set>

// Struct to define a type of terrain
struct TerrainType {
    std::string name;
    float speed_modifier;    // Multiplier for entity speed (0.5 = half speed, 1.0 = normal)
    float sight_modifier;    // Multiplier for entity sight radius
    std::set<AnimalType> allowed_types; // Empty set means all types allowed
    char symbol;            // Symbol for console representation
    sf::Color color;        // Base color for rendering
};

// Terrain type constants
extern const TerrainType TERRAIN_NORMAL;
extern const TerrainType TERRAIN_WATER;
extern const TerrainType TERRAIN_ROCKY;

#endif // TERRAIN_H
