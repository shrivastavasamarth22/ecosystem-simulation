#include "includes/GraphicsRenderer.h"
#include "includes/AnimalTypes.h"
#include "includes/Resource.h"
#include "includes/Tile.h"
#include "includes/EntityManager.h"

GraphicsRenderer::GraphicsRenderer() : m_window(), m_tile_size(0) {
    // Constructor doesn't create the window yet, init does.
}

GraphicsRenderer::~GraphicsRenderer() {
    // Destructor is called when the object goes out of scope,
    // automatically cleans up SFML resources (like the window).
}

void GraphicsRenderer::init(int world_width, int world_height, int tile_size, const std::string& title) {
    m_tile_size = tile_size;
    unsigned int window_width = world_width * m_tile_size;
    unsigned int window_height = world_height * m_tile_size;

    // Create the window
    m_window.create(sf::VideoMode(window_width, window_height), title);
    m_window.setVerticalSyncEnabled(true); // Enable vertical sync for smoother frame rate
}

bool GraphicsRenderer::isOpen() const {
    return m_window.isOpen();
}

void GraphicsRenderer::handleEvents() {
    sf::Event event;
    // Process events in a loop
    while (m_window.pollEvent(event)) {
        // Close window: exit
        if (event.type == sf::Event::Closed)
            m_window.close();

        // Handle keyboard input here later (Phase 6)
        // if (event.type == sf::Event::KeyPressed) { ... }
    }
}

void GraphicsRenderer::clear(const sf::Color& color) {
    m_window.clear(color); // Clear the window with the specified color
}

void GraphicsRenderer::display() {
    m_window.display(); // Display what has been drawn to the window
}