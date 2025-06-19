#ifndef ANIMAL_H
#define ANIMAL_H

#include <memory>

class World;

enum class AIState {
  WANDERING, // Moving randomly, grazing
  FLEEING, // Actively running from a threat
  CHASING, // Actively pursuing prey
  PACK_HUNTING, // Grouping up to attack a stronger target
};

class Animal {
  protected:
    int x, y;
    int age;
    bool is_alive;
    char symbol;

    // --- New Attributes ---
    int health;
    int max_health;
    int damage;
    int sight_radius;
    int speed;
    int energy; // Now used for stamina/reproduction, not life
    AIState current_state;
    Animal* target;
  
  public:
    Animal(int start_x, int start_y, char sym, int max_hp, int dmg, int sight, int spd, int nrg);
    virtual ~Animal() = default;

    // --- Core AI Loop (Replaces old 'update' method) ---
    // Pure virtual functions for specific AI logic
    virtual void updateAI(World& world) = 0;
    virtual void act(World& world) = 0;
    virtual std::unique_ptr<Animal> reproduce() = 0;

    // New common functions
    void takeDamage(int amount);
    void postTurnUpdate(); // Handles aging, energy loss
    bool isDead() const;
    void kill(); // Instantly sets health to 0 and marks as not alive

    // --- Helper Functions for Actions ---
    void moveTowards(int target_x, int target_y);
    void moveAwayFrom(int target_x, int target_y);
    void moveRandom(const World& world);

    // --- Getters ---
    int getX() const { return x; }
    int getY() const { return y; }
    int getSightRadius() const { return sight_radius; }
    int getSymbol() const { return symbol; }
};

#endif // ANIMAL_H