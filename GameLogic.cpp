// Implementations for GameLogic. The public API is declared in GameLogic.h.
#include "GameLogic.h"
#include <algorithm>

GameLogic::GameLogic() {
}

GameLogic::~GameLogic() {
}

void GameLogic::init() {
    players.clear();
    hazards.clear();

    // Default: two survivors and one death player (index 0 = survivor, 1 = death)
    Player p1;
    p1.x = 100.0f; p1.y = 100.0f; p1.isDeath = false;
    players.push_back(p1);

    Player p2;
    p2.x = 200.0f; p2.y = 100.0f; p2.isDeath = true;
    p2.health = 100;
    players.push_back(p2);
}

void GameLogic::step(float dt, const InputState &survivorInput, const InputState &deathInput) {
    // Apply input to the survivor (assume players[0] is survivor) if present
    if (!players.empty()) {
        Player &surv = players[0];
        if (surv.alive) {
            float ax = 0.0f;
            if (survivorInput.left) ax -= MOVE_SPEED;
            if (survivorInput.right) ax += MOVE_SPEED;

            surv.vx = ax;
            if (survivorInput.jump) {
                // simple ground check: y >= 0 considered on ground in this PoC
                if (surv.y >= 0.0f) surv.vy = JUMP_VELOCITY;
            }

            // Integrate
            surv.vy += GRAVITY * dt;
            surv.x += surv.vx * dt;
            surv.y += surv.vy * dt;

            // Keep above floor
            if (surv.y > 1000.0f) { // arbitrary fatal fall threshold for PoC
                surv.alive = false;
                surv.health = 0;
            }
        }
    }

    // Apply input to the Death player (assume players[1] is death) if present
    if (players.size() > 1) {
        Player &death = players[1];
        if (death.alive) {
            float ax = 0.0f;
            if (deathInput.left) ax -= MOVE_SPEED;
            if (deathInput.right) ax += MOVE_SPEED;
            death.vx = ax;
            if (deathInput.jump) {
                if (death.y >= 0.0f) death.vy = JUMP_VELOCITY;
            }
            death.vy += GRAVITY * dt;
            death.x += death.vx * dt;
            death.y += death.vy * dt;
        }
    }

    // Simple hazard update
    for (auto &h : hazards) {
        if (!h.active) continue;
        h.vy += GRAVITY * dt;
        h.x += h.vx * dt;
        h.y += h.vy * dt;
    }

    // Basic collision: if any hazard is near a player, reduce health
    for (auto &pl : players) {
        if (!pl.alive) continue;
        for (auto &h : hazards) {
            if (!h.active) continue;
            float dx = pl.x - h.x;
            float dy = pl.y - h.y;
            float dist2 = dx*dx + dy*dy;
            if (dist2 < 25.0f) { // hit threshold
                pl.health -= 10;
                h.active = false; // one-time hit
                if (pl.health <= 0) {
                    pl.alive = false;
                    pl.health = 0;
                }
            }
        }
    }
}
