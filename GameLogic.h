#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include <vector>

// Minimal input state used by GameLogic for both Survivor and Death players.
struct InputState {
    bool left = false;
    bool right = false;
    bool jump = false;
    bool attack = false;
};

struct Player {
    float x = 0.0f;
    float y = 0.0f;
    float vx = 0.0f;
    float vy = 0.0f;
    bool isDeath = false;
    int health = 100;
    bool alive = true;
};

struct Hazard {
    float x = 0.0f;
    float y = 0.0f;
    float vx = 0.0f;
    float vy = 0.0f;
    bool active = true;
};

class GameLogic {
public:
    GameLogic();
    ~GameLogic();

    // Initialize game state (players/hazards)
    void init();

    // Advance simulation by dt, applying inputs for survivor and death players
    void step(float dt, const InputState &survivorInput, const InputState &deathInput);

    const std::vector<Player>& get_players() const { return players; }
    const std::vector<Hazard>& get_hazards() const { return hazards; }

private:
    std::vector<Player> players;
    std::vector<Hazard> hazards;

    // simple physics constants
    static constexpr float GRAVITY = 980.0f;
    static constexpr float MOVE_SPEED = 200.0f;
    static constexpr float JUMP_VELOCITY = -350.0f;
};

#endif // GAMELOGIC_H
