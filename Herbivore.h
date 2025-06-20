#ifndef HERBIVORE_H
#define HERBIVORE_H

#include "Animal.h"

class Herbivore : public Animal {

protected:
    void applyAgingPenalties() override;

public:
    // Constructor: Creates a Herbivore at a specific location.
    // All stats (HP, damage, etc.) are set within the .cpp file.
    Herbivore(int x, int y);

    // --- Overridden AI Functions ---

    // Decides what state to be in (FLEEING, WANDERING) based on surroundings.
    void updateAI(World& world) override;

    // Executes the action for the current state (e.g., moves away from a threat).
    void act(World& world) override;

    // Creates a new Herbivore if energy is sufficient.
    std::unique_ptr<Animal> reproduce() override;
};

#endif // HERBIVORE_H