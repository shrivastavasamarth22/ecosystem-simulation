#ifndef BIOME_H
#define BIOME_H

#include "resources/Resource.h"
#include <SFML/Graphics/Color.hpp>
#include <map>
#include <string>
#include <vector>

// Forward declare TerrainType to avoid include loop
struct TerrainType;

// Defines the properties of a specific biome
struct BiomeType {
    std::string name;

    // Defines which terrains can appear in this biome and their spawn probability.
    std::map<const TerrainType*, float> terrain_distribution;

    // Defines which resources can appear in this biome and their spawn probability.
    std::map<const ResourceType*, float> resource_distribution;

    // Base color for texture tinting or background rendering
    sf::Color color;
};

// Declare the global biome type constants defined in Biome.cpp
extern const BiomeType BIOME_WATER;
extern const BiomeType BIOME_BARREN;
extern const BiomeType BIOME_ROCKY;
extern const BiomeType BIOME_GRASSLAND;
extern const BiomeType BIOME_FOREST;
extern const BiomeType BIOME_FERTILE;

#endif // BIOME_H
