#include "GameLogic.h"
#include <vector>






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

class GameLogic {
public:
    void init();
    void step(float dt, const InputState& survivorInput, const InputState& deathInput);
    const std::vector<Player>& get_players() const;
    const std::vector<Hazard>& get_hazards() const;
};
