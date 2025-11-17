#ifndef PLAYER_H
#define PLAYER_H

#include <godot_cpp/classes/character_body2d.hpp>
#include <godot_cpp/classes/sprite2d.hpp>
#include <godot_cpp/classes/collision_shape2d.hpp>
#include <godot_cpp/classes/multiplayer_synchronizer.hpp>

namespace godot {

/**
 * @brief Player class for survivor characters
 * 
 * Handles player movement, health, hunger, and synchronization
 * across the network.
 */
class Player : public CharacterBody2D {
    GDCLASS(Player, CharacterBody2D)

private:
    float speed;
    float jump_velocity;
    int health;
    int max_health;
    int hunger;
    int max_hunger;
    bool is_alive;
    
    // Network synchronization
    int player_id;
    Vector2 synced_position;
    Vector2 synced_velocity;

protected:
    static void _bind_methods();

public:
    Player();
    ~Player();

    // Godot lifecycle methods
    void _ready() override;
    void _physics_process(double delta) override;
    void _process(double delta) override;

    // Game logic methods
    void handle_input(double delta);
    void apply_gravity(double delta);
    void update_hunger(double delta);
    
    /**
     * @brief Apply damage to the player
     * @param amount Damage amount
     */
    void take_damage(int amount);
    
    /**
     * @brief Heal the player
     * @param amount Heal amount
     */
    void heal(int amount);
    
    /**
     * @brief Feed the player
     * @param amount Food amount
     */
    void eat(int amount);
    
    /**
     * @brief Check if player is still alive
     * @return true if health > 0
     */
    bool get_is_alive() const { return is_alive; }
    
    // Getters/Setters for networking
    void set_player_id(int id) { player_id = id; }
    int get_player_id() const { return player_id; }
    
    int get_health() const { return health; }
    void set_health(int h);
    
    int get_hunger() const { return hunger; }
    void set_hunger(int h);
};

} // namespace godot

#endif // PLAYER_H