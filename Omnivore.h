#ifndef OMNIVORE_H
#define OMNIVORE_H

#include "Animal.h"

class Omnivore : public Animal {
public:
    // Constructor: Creates an Omnivore at a specific location.
    Omnivore(int x, int y);

    // --- Overridden AI Functions ---

    // Decides its state: WANDERING, CHASING, PACK_HUNTING, or FLEEING.
    void updateAI(World& world) override;

    // Executes the action for the current state.
    void act(World& world) override;

    // Creates a new Omnivore if energy is sufficient.
    std::unique_ptr<Animal> reproduce() override;
};

#endif // OMNIVORE_H