#ifndef WFC_GENERATOR_H
#define WFC_GENERATOR_H

#include "resources/Biome.h"
#include <vector>
#include <set>
#include <random>
#include <map>

struct WFCCell {
    std::set<const BiomeType*> possible_biomes;
    const BiomeType* collapsed_biome = nullptr;
    bool is_collapsed = false;
};

class WFCGenerator {
public:
    WFCGenerator(int width, int height, std::mt19937& rng);
    
    // Main WFC algorithm
    bool generate();
    
    // Get the final biome assignment for a tile
    const BiomeType* getBiome(int x, int y) const;
    
    // Set initial constraints (seed some cells with specific biomes)
    void setSeed(int x, int y, const BiomeType* biome);
    
    // Get adjacency rules for external use
    const std::map<const BiomeType*, std::set<const BiomeType*>>& getAdjacencyRules() const;
    
private:
    int m_width, m_height;
    std::mt19937& m_rng;
    std::vector<std::vector<WFCCell>> m_grid;
    
    // Adjacency rules - which biomes can be next to each other
    std::map<const BiomeType*, std::set<const BiomeType*>> m_adjacency_rules;
    
    // WFC algorithm steps
    void initializeGrid();
    void setupAdjacencyRules();
    bool findLowestEntropyCell(int& out_x, int& out_y);
    bool collapseCell(int x, int y);
    bool propagateConstraints(int x, int y);
    void updateNeighborConstraints(int x, int y, int nx, int ny);
    bool isValidNeighbor(const BiomeType* current, const BiomeType* neighbor);
    
    // Utility functions
    bool isValidCoordinate(int x, int y) const;
    std::vector<std::pair<int, int>> getNeighbors(int x, int y) const;
};

#endif // WFC_GENERATOR_H
