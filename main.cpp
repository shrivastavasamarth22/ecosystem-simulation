#include "includes/World.h"
#include "includes/GraphicsRenderer.h" // <-- Include our new renderer
#include "includes/AnimalConfig.h"     // Needed for constants
#include "includes/EntityManager.h"    // Needed for EntityManager access (later)

#include <iostream>
#include <thread>
#include <chrono>
#include <string> // Needed for window title
#include <SFML/System.hpp> // Needed for sf::Clock and sf::Time

int main() {
    // ... (simulation parameters) ...
    const int WORLD_WIDTH = 50;
    const int WORLD_HEIGHT = 25;
    const int SPATIAL_GRID_CELL_SIZE = 15;
    const int INITIAL_HERBIVORES = 25;
    const int INITIAL_OMNIVORES = 10;
    const int INITIAL_CARNIVORES = 8;
    const int SIMULATION_SPEED_MS = 500;
    const int MAX_TURNS = 2000;
    const int TILE_SIZE_PIXELS = 20;
    const std::string WINDOW_TITLE = "Ecosystem Simulation";


    // --- Simulation Setup ---
    World world(WORLD_WIDTH, WORLD_HEIGHT, SPATIAL_GRID_CELL_SIZE);
    world.init(INITIAL_HERBIVORES, INITIAL_CARNIVORES, INITIAL_OMNIVORES);

    // --- Graphics Setup ---
    GraphicsRenderer renderer;
    renderer.init(WORLD_WIDTH, WORLD_HEIGHT, TILE_SIZE_PIXELS, WINDOW_TITLE);

    // --- Simulation Timing & Control ---
    sf::Clock simulationClock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;
    sf::Time timePerUpdate = sf::milliseconds(SIMULATION_SPEED_MS);
    bool is_paused = false; // <-- Pause state flag
    bool was_p_pressed_last_frame = false; // <-- To detect rising edge of P key


    std::cout << "Starting Intelligent Agent Simulation..." << std::endl;

    // --- Main SFML Window Loop ---
    while (renderer.isOpen()) {
        // Handle window events (closing) - handleEvents no longer needs to process pause
        renderer.handleEvents();

        // --- Handle Pause Input (Polling) ---
        bool is_p_currently_pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::P);
        if (is_p_currently_pressed && !was_p_pressed_last_frame) {
            is_paused = !is_paused; // Toggle pause state
            // Optional: Print pause state to console
            if (is_paused) { std::cout << "Simulation Paused." << std::endl; }
            else { std::cout << "Simulation Resumed." << std::endl; }
        }
        was_p_pressed_last_frame = is_p_currently_pressed;


        // Update simulation based on elapsed time ONLY if NOT paused
        timeSinceLastUpdate += simulationClock.restart();
        while (timeSinceLastUpdate > timePerUpdate) {
            timeSinceLastUpdate -= timePerUpdate;

            // --- Run one simulation turn ONLY if NOT paused ---
                if (!is_paused && !world.isEcosystemCollapsed() && world.getEntityManager().getEntityCount() > 0 && world.getEntityManager().is_alive.size() > 0 )
                {
                    world.update();
                }

            // Check for end conditions
            if (world.isEcosystemCollapsed() || world.getEntityManager().getEntityCount() == 0) {
                std::cout << "Ecosystem collapsed or empty. Simulation finished." << std::endl;
                break; // Exit the simulation update loop
            }
                if (world.getTurnCount() >= MAX_TURNS) {
                    std::cout << "Simulation reached max turns. Finished." << std::endl;
                    break; // Exit the simulation update loop
                }

        } // End while (timeSinceLastUpdate > timePerUpdate)

        // --- Drawing Phase ---
        renderer.clear(sf::Color(100, 149, 237));

        renderer.drawWorld(world);
        renderer.drawEntities(world.getEntityManager());
        // --- MODIFIED: Pass pause state to drawUI ---
        renderer.drawUI(world, is_paused);

        renderer.display();

    } // End while (renderer.isOpen())

    std::cout << "SFML Window closed. Exiting simulation." << std::endl;

    return 0;
}