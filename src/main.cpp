#include "core/World.h"
#include "graphics/GraphicsRenderer.h" // <-- Include our new renderer
#include "common/AnimalConfig.h"     // Needed for constants
#include "core/EntityManager.h"    // Needed for EntityManager access (later)

#include <iostream>
#include <thread>
#include <chrono>
#include <string> // Needed for window title
#include <SFML/System.hpp> // Needed for sf::Clock and sf::Time

int main() {
    // ... (simulation parameters) ...
    const int WORLD_WIDTH = 200;
    const int WORLD_HEIGHT = 100; 
    const int SPATIAL_GRID_CELL_SIZE = 0; // 0 = auto-calculate optimal size
    const int INITIAL_HERBIVORES = 25;
    const int INITIAL_OMNIVORES = 10;
    const int INITIAL_CARNIVORES = 5;
    const int SIMULATION_SPEED_MS = 500;
    const int MAX_TURNS = 2000;
    const int TILE_SIZE_PIXELS = 20;
    const std::string WINDOW_TITLE = "Ecosystem Simulation";

    // --- NEW: Window Dimensions (Fixed) ---
    const unsigned int WINDOW_WIDTH_PIXELS = 1920;
    const unsigned int WINDOW_HEIGHT_PIXELS = 1080;


    // --- Simulation Setup ---
    World world(WORLD_WIDTH, WORLD_HEIGHT, SPATIAL_GRID_CELL_SIZE);
    world.init(INITIAL_HERBIVORES, INITIAL_CARNIVORES, INITIAL_OMNIVORES);

    // --- Graphics Setup ---
    GraphicsRenderer renderer;
    // Pass the fixed window dimensions AND the world dimensions to the renderer
    renderer.init(WINDOW_WIDTH_PIXELS, WINDOW_HEIGHT_PIXELS, WORLD_WIDTH, WORLD_HEIGHT, TILE_SIZE_PIXELS, WINDOW_TITLE);
    // --- Simulation Timing & Control ---
    sf::Clock simulationClock;
    sf::Clock cameraClock; // For smooth camera updates
    sf::Time timeSinceLastUpdate = sf::Time::Zero;
    sf::Time timePerUpdate = sf::milliseconds(SIMULATION_SPEED_MS);
    bool is_paused = false; // <-- Pause state flag

    // --- NEW: Animation Timing ---
    float animation_progress = 0.0f; // 0.0 = start of turn, 1.0 = end of turn

    bool was_p_pressed_last_frame = false; // <-- To detect rising edge of P key
    bool simulation_ended = false; // <-- Flag to indicate if the simulation has ended


    std::cout << "Starting Intelligent Agent Simulation..." << std::endl;

    // --- Main SFML Window Loop ---
    while (renderer.isOpen()) {
        // Handle window events (closing)
        renderer.handleEvents();

        // --- Handle Pause Input ---
        bool is_p_currently_pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::P);
        if (is_p_currently_pressed && !was_p_pressed_last_frame) {
            is_paused = !is_paused; // Toggle pause state
            if (is_paused) { std::cout << "Simulation Paused." << std::endl; }
            else { std::cout << "Simulation Resumed." << std::endl; }
        }
        was_p_pressed_last_frame = is_p_currently_pressed;


        // --- Simulation Update Loop (Fixed Timestep) ---
        timeSinceLastUpdate += simulationClock.restart();
        while (timeSinceLastUpdate > timePerUpdate) {
            timeSinceLastUpdate -= timePerUpdate;

            // Only run the update if the simulation is not paused and has not ended.
            if (!is_paused && !simulation_ended)
            {
                world.update();
                animation_progress = 0.0f; // Reset animation timer each tick

                // Check for end conditions AFTER the update
                if (world.isEcosystemCollapsed() || world.getEntityManager().getEntityCount() == 0) {
                    std::cout << "Ecosystem collapsed or empty. Simulation finished." << std::endl;
                    simulation_ended = true;
                } else if (world.getTurnCount() >= MAX_TURNS) {
                    std::cout << "Simulation reached max turns. Finished." << std::endl;
                    simulation_ended = true;
                }
            }
        }

        // --- NEW: Update Animation Progress ---
        if (!is_paused) {
            animation_progress = std::min(1.0f, timeSinceLastUpdate.asSeconds() / timePerUpdate.asSeconds());
        }

        // --- Update Camera System ---
        float camera_delta_time = cameraClock.restart().asSeconds();
        renderer.update(camera_delta_time);

        // --- Drawing Phase ---
        renderer.clear(sf::Color(100, 149, 237)); // Cornflower Blue

        renderer.drawWorld(world);
        renderer.drawEntities(world.getEntityManager(), animation_progress);
        renderer.drawUI(world, is_paused);

        // Draw simulation ended message if simulation has ended
        if (simulation_ended) {
            renderer.drawSimulationEndedMessage();
        }

        renderer.drawCursor();

        renderer.display();

    } // End while (renderer.isOpen())

    std::cout << "SFML Window closed. Exiting simulation." << std::endl;

    return 0;
}