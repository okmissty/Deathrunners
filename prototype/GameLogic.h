#include <vector>

struct GameLogic
{
    void init();
    void step(float dt, const InputState& survivorInput, const InputState& deathInput);
    const std::vector<struct Player>& get_players() const;
    const std::vector<struct Hazard>& get_hazards() const;
};  

struct Player {
    float x, y;
    float vx, vy;
    bool isDeath;
    int health;
    bool alive;
};

struct Hazard {
    float x, y;
    float vx, vy;
    bool active;
};