#ifndef ANIMAL_H
#define ANIMAL_H

#include <memory>
#include <algorithm>

class World; // Forward declaration

enum class AIState {
    WANDERING,
    FLEEING,
    CHASING,
    PACK_HUNTING
};

class Animal {
    protected:
        int x, y;
        int age;
        bool is_alive;
        char symbol;

        // --- Health System ---
        int health;
        int max_health;
        int turns_since_damage; // For health regeneration

        // --- Hunger/Stat System ---
        // Base stats are the animal's natural attributes
        int base_damage;
        int base_sight_radius;
        int base_speed;

        // Current stats can be modified by hunger, etc.
        int current_damage;
        int current_sight_radius;
        int current_speed;

        int energy;
        int max_energy; // New: For hunger system percentages

        AIState current_state;
        Animal* target;

    public:
        // Constructor updated for max_energy and base stats
        Animal(int start_x, int start_y, char sym,
           int m_hp, int base_dmg, int base_sight, int base_spd, int m_energy, int start_nrg);

        virtual ~Animal() = default;

        virtual void updateAI(World& world) = 0;
        virtual void act(World& world) = 0;
        virtual std::unique_ptr<Animal> reproduce() = 0;

        void takeDamage(int amount);
        void postTurnUpdate(); // Now needs world for boundary checks if hunger causes death
        bool isDead() const;
        void kill();

        void moveTowards(const World& world, int target_x, int target_y);
        void moveAwayFrom(const World& world, int target_x, int target_y);
        void moveRandom(const World& world);

        // Getters now return CURRENT stats
        int getX() const { return x; }
        int getY() const { return y; }
        int getCurrentSightRadius() const { return current_sight_radius; }
        int getCurrentDamage() const { return current_damage; }
        int getCurrentSpeed() const { return current_speed; }
        char getSymbol() const { return symbol; }
        int getHealth() const { return health; } // Useful for debugging
        int getEnergy() const { return energy; } // Useful for debugging

};

#endif // ANIMAL_H