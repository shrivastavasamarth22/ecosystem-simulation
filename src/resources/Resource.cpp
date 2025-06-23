#include "resources/Resource.h"

const ResourceType RESOURCE_GRASS = {
    "Grass",
    10, // Max amount
    2, // Regrowth rate per turn
    1, // Nutritional value
    ',' // Symbol to draw on the grid
};

const ResourceType RESOURCE_BERRIES = {
    "Berries",
    5, // Max amount
    1, // Regrowth rate per turn
    3, // Nutritional value
    'b' // Symbol to draw on the grid
};