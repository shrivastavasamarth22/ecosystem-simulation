#include "core/WFCGenerator.h"
#include "resources/Biome.h"
#include <algorithm>
#include <limits>

WFCGenerator::WFCGenerator(int width, int height, std::mt19937& rng) 
    : m_width(width), m_height(height), m_rng(rng) {
    m_grid.resize(height, std::vector<WFCCell>(width));
    setupAdjacencyRules();
}

void WFCGenerator::setupAdjacencyRules() {
    // Define which biomes can be adjacent to each other
    // More permissive rules to avoid WFC getting stuck and create natural transitions
    
    // Water can be next to: Water, Barren, Grassland, Forest (rivers through forests)
    m_adjacency_rules[&BIOME_WATER] = {&BIOME_WATER, &BIOME_BARREN, &BIOME_GRASSLAND, &BIOME_FOREST};
    
    // Barren can be next to: Water, Barren, Rocky, Grassland, Forest (transition zones)
    m_adjacency_rules[&BIOME_BARREN] = {&BIOME_WATER, &BIOME_BARREN, &BIOME_ROCKY, &BIOME_GRASSLAND, &BIOME_FOREST};
    
    // Rocky can be next to: Barren, Rocky, Grassland, Forest (mountains near various biomes)
    m_adjacency_rules[&BIOME_ROCKY] = {&BIOME_BARREN, &BIOME_ROCKY, &BIOME_GRASSLAND, &BIOME_FOREST};
    
    // Grassland can be next to: Water, Barren, Rocky, Grassland, Forest, Fertile (central transition biome)
    m_adjacency_rules[&BIOME_GRASSLAND] = {&BIOME_WATER, &BIOME_BARREN, &BIOME_ROCKY, &BIOME_GRASSLAND, &BIOME_FOREST, &BIOME_FERTILE};
    
    // Forest can be next to: Water, Barren, Rocky, Grassland, Forest, Fertile (forests are versatile)
    m_adjacency_rules[&BIOME_FOREST] = {&BIOME_WATER, &BIOME_BARREN, &BIOME_ROCKY, &BIOME_GRASSLAND, &BIOME_FOREST, &BIOME_FERTILE};
    
    // Fertile can be next to: Grassland, Forest, Fertile (rich areas, but can connect to most green areas)
    m_adjacency_rules[&BIOME_FERTILE] = {&BIOME_GRASSLAND, &BIOME_FOREST, &BIOME_FERTILE};
}

void WFCGenerator::initializeGrid() {
    // Initialize all cells with all possible biomes
    std::set<const BiomeType*> all_biomes = {
        &BIOME_WATER, &BIOME_BARREN, &BIOME_ROCKY, 
        &BIOME_GRASSLAND, &BIOME_FOREST, &BIOME_FERTILE
    };
    
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            if (!m_grid[y][x].is_collapsed) {
                m_grid[y][x].possible_biomes = all_biomes;
            }
        }
    }
}

bool WFCGenerator::generate() {
    initializeGrid();
    
    // WFC main loop
    while (true) {
        int min_x, min_y;
        if (!findLowestEntropyCell(min_x, min_y)) {
            // All cells are collapsed - generation complete!
            return true;
        }
        
        if (!collapseCell(min_x, min_y)) {
            // Contradiction occurred - generation failed
            return false;
        }
        
        if (!propagateConstraints(min_x, min_y)) {
            // Propagation failed - contradiction
            return false;
        }
    }
}

bool WFCGenerator::findLowestEntropyCell(int& out_x, int& out_y) {
    int min_entropy = std::numeric_limits<int>::max();
    std::vector<std::pair<int, int>> candidates;
    
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            if (!m_grid[y][x].is_collapsed) {
                int entropy = m_grid[y][x].possible_biomes.size();
                
                if (entropy == 0) {
                    // Contradiction - no possible biomes
                    return false;
                }
                
                if (entropy < min_entropy) {
                    min_entropy = entropy;
                    candidates.clear();
                    candidates.push_back({x, y});
                } else if (entropy == min_entropy) {
                    candidates.push_back({x, y});
                }
            }
        }
    }
    
    if (candidates.empty()) {
        // All cells are collapsed
        return false;
    }
    
    // Randomly select from candidates with lowest entropy
    std::uniform_int_distribution<int> dist(0, candidates.size() - 1);
    auto selected = candidates[dist(m_rng)];
    out_x = selected.first;
    out_y = selected.second;
    
    return true;
}

bool WFCGenerator::collapseCell(int x, int y) {
    WFCCell& cell = m_grid[y][x];
    
    if (cell.is_collapsed || cell.possible_biomes.empty()) {
        return false;
    }
    
    // Randomly select a biome from possible options
    std::vector<const BiomeType*> options(cell.possible_biomes.begin(), cell.possible_biomes.end());
    std::uniform_int_distribution<int> dist(0, options.size() - 1);
    
    cell.collapsed_biome = options[dist(m_rng)];
    cell.is_collapsed = true;
    cell.possible_biomes.clear();
    
    return true;
}

bool WFCGenerator::propagateConstraints(int start_x, int start_y) {
    // Use a queue to propagate constraints
    std::vector<std::pair<int, int>> to_process;
    to_process.push_back({start_x, start_y});
    
    while (!to_process.empty()) {
        auto [x, y] = to_process.back();
        to_process.pop_back();
        
        auto neighbors = getNeighbors(x, y);
        for (auto [nx, ny] : neighbors) {
            if (m_grid[ny][nx].is_collapsed) {
                continue; // Already collapsed, skip
            }
            
            // Update neighbor's constraints based on collapsed cell
            size_t old_size = m_grid[ny][nx].possible_biomes.size();
            updateNeighborConstraints(x, y, nx, ny);
            
            if (m_grid[ny][nx].possible_biomes.empty()) {
                // Contradiction
                return false;
            }
            
            // If constraints changed, add to processing queue
            if (m_grid[ny][nx].possible_biomes.size() != old_size) {
                to_process.push_back({nx, ny});
            }
        }
    }
    
    return true;
}

void WFCGenerator::updateNeighborConstraints(int x, int y, int nx, int ny) {
    const BiomeType* collapsed_biome = m_grid[y][x].collapsed_biome;
    if (!collapsed_biome) return;
    
    WFCCell& neighbor = m_grid[ny][nx];
    
    // Remove biomes from neighbor that are not compatible with collapsed_biome
    auto it = neighbor.possible_biomes.begin();
    while (it != neighbor.possible_biomes.end()) {
        if (!isValidNeighbor(collapsed_biome, *it)) {
            it = neighbor.possible_biomes.erase(it);
        } else {
            ++it;
        }
    }
}

bool WFCGenerator::isValidNeighbor(const BiomeType* current, const BiomeType* neighbor) {
    auto it = m_adjacency_rules.find(current);
    if (it == m_adjacency_rules.end()) {
        return true; // No rules defined, allow any neighbor
    }
    
    return it->second.count(neighbor) > 0;
}

bool WFCGenerator::isValidCoordinate(int x, int y) const {
    return x >= 0 && x < m_width && y >= 0 && y < m_height;
}

std::vector<std::pair<int, int>> WFCGenerator::getNeighbors(int x, int y) const {
    std::vector<std::pair<int, int>> neighbors;
    
    // 4-directional neighbors (up, down, left, right)
    std::vector<std::pair<int, int>> offsets = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};
    
    for (auto [dx, dy] : offsets) {
        int nx = x + dx;
        int ny = y + dy;
        
        if (isValidCoordinate(nx, ny)) {
            neighbors.push_back({nx, ny});
        }
    }
    
    return neighbors;
}

const BiomeType* WFCGenerator::getBiome(int x, int y) const {
    if (!isValidCoordinate(x, y)) {
        return nullptr;
    }
    
    return m_grid[y][x].collapsed_biome;
}

void WFCGenerator::setSeed(int x, int y, const BiomeType* biome) {
    if (!isValidCoordinate(x, y)) {
        return;
    }
    
    WFCCell& cell = m_grid[y][x];
    cell.collapsed_biome = biome;
    cell.is_collapsed = true;
    cell.possible_biomes.clear();
}

const std::map<const BiomeType*, std::set<const BiomeType*>>& WFCGenerator::getAdjacencyRules() const {
    return m_adjacency_rules;
}
