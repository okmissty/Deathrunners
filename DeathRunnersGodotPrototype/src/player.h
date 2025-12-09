#ifndef PLAYER_H
#define PLAYER_H

#include <godot_cpp/classes/character_body2d.hpp>
#include <godot_cpp/classes/progress_bar.hpp>
#include <godot_cpp/classes/animated_sprite2d.hpp>
#include <godot_cpp/classes/camera2d.hpp>
#include <godot_cpp/classes/multiplayer_peer.hpp>

namespace godot {

class Player : public CharacterBody2D {
    GDCLASS(Player, CharacterBody2D)

private:
    float speed;
    float jump_velocity;
    float gravity;
    
    // Health & Hunger
    float health;
    float max_health;
    float hunger;
    float max_hunger;
    
    // Lives & State
    int lives;
    int max_lives;
    bool is_alive;
    bool reached_goal;
    Vector2 checkpoint_position;
    
    // Network Sync
    Vector2 synced_position;
    Vector2 synced_velocity;

    // References
    AnimatedSprite2D* sprite;
    ProgressBar* health_bar;
    ProgressBar* hunger_bar;

protected:
    static void _bind_methods();

public:
    Player();
    ~Player();

    void _ready() override;
    void _physics_process(double delta) override;
    void _process(double delta) override;

    // Actions
    void handle_input(double delta);
    void apply_gravity(double delta);
    void update_hunger(double delta);
    void _clamp_to_camera_bounds();
    void _handle_animations(float dir);
    void _setup_ui_bars();
    void _update_ui();

    // Gameplay
    void take_damage(float amount);
    void heal(float amount);
    void eat(float amount);
    void respawn();
    void mark_goal_reached();

    // RPCs
    void sync_respawn(Vector2 pos);
    void sync_death();
    void sync_goal_reached();

    void sync_state(Vector2 pos, Vector2 vel, float h, float hu, int l, String anim, bool flip);
    // Getters/Setters for properties
    void set_health(float h) { health = h; }
    float get_health() const { return health; }
    void set_hunger(float h) { hunger = h; }
    float get_hunger() const { return hunger; }
    void set_lives(int l) { lives = l; }
    int get_lives() const { return lives; }
};

} // namespace godot

#endif