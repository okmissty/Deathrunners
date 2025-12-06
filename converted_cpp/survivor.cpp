// Plain C++ reference translation of the Survivor logic (non-Godot PoC)
#include <iostream>

class SurvivorRef {
public:
    int health = 100;
    int hunger = 100;
    int max_health = 100;
    int max_hunger = 100;
    float y = 0.0f;
    float death_y = 3000.0f;

    void update(float delta) {
        // Example hunger drain
        hunger -= static_cast<int>(2.0f * delta);
        if (hunger < 0) hunger = 0;
        if (y > death_y) take_damage(health);
    }

    void take_damage(int amount) {
        health -= amount;
        if (health <= 0) {
            health = 0;
            std::cout << "Survivor died\n";
        }
    }
};

int main() {
    SurvivorRef s;
    s.y = 3100.0f; // simulate fall
    s.update(0.016f);
    std::cout << "Health after update: " << s.health << "\n";
    return 0;
}
