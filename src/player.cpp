#include "player.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/multiplayer_api.hpp>
#include <godot_cpp/classes/multiplayer_peer.hpp>

using namespace godot;

void Player::_bind_methods() {
    // Bind methods that can be called from GDScript or networked
    ClassDB::bind_method(D_METHOD("take_damage", "amount"), &Player::take_damage);
    ClassDB::bind_method(D_METHOD("heal", "amount"), &Player::heal);
    ClassDB::bind_method(D_METHOD("eat", "amount"), &Player::eat);
    
    ClassDB::bind_method(D_METHOD("get_health"), &Player::get_health);
    ClassDB::bind_method(D_METHOD("set_health", "h"), &Player::set_health);
    
    ClassDB::bind_method(D_METHOD("get_hunger"), &Player::get_hunger);
    ClassDB::bind_method(D_METHOD("set_hunger", "h"), &Player::set_hunger);
    
    ClassDB::bind_method(D_METHOD("get_player_id"), &Player::get_player_id);
    ClassDB::bind_method(D_METHOD("set_player_id", "id"), &Player::set_player_id);
    
    // Properties for network synchronization
    ADD_PROPERTY(PropertyInfo(Variant::INT, "health"), "set_health", "get_health");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "hunger"), "set_hunger", "get_hunger");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "player_id"), "set_player_id", "get_player_id");
}

Player::Player() {
    speed = 200.0f;
    jump_velocity = -400.0f;
    health = 100;
    max_health = 100;
    hunger = 100;
    max_hunger = 100;
    is_alive = true;
    player_id = 0;
}

Player::~Player() {
}

void Player::_ready() {
    // Set up physics properties
    set_floor_stop_on_slope_enabled(true);
    set_floor_snap_length(5.0);
}

void Player::_physics_process(double delta) {
    // Only process input if this is the local player
    MultiplayerAPI *multiplayer = get_multiplayer();
    if (multiplayer && is_multiplayer_authority()) {
        handle_input(delta);
    }
    
    // Apply gravity to all players
    apply_gravity(delta);
    
    // Move the character
    move_and_slide();
    
    // Update position for network sync
    synced_position = get_position();
    synced_velocity = get_velocity();
}

void Player::_process(double delta) {
    // Update hunger over time (only on authority)
    MultiplayerAPI *multiplayer = get_multiplayer();
    if (multiplayer && is_multiplayer_authority()) {
        update_hunger(delta);
    }
}

void Player::handle_input(double delta) {
    Input *input = Input::get_singleton();
    Vector2 velocity = get_velocity();
    
    // Horizontal movement
    if (input->is_action_pressed("move_left")) {
        velocity.x = -speed;
    } else if (input->is_action_pressed("move_right")) {
        velocity.x = speed;
    } else {
        velocity.x = 0;
    }
    
    // Jump
    if (input->is_action_just_pressed("jump") && is_on_floor()) {
        velocity.y = jump_velocity;
    }
    
    set_velocity(velocity);
}

void Player::apply_gravity(double delta) {
    Vector2 velocity = get_velocity();
    
    if (!is_on_floor()) {
        velocity.y += 980.0 * delta; // Gravity
    }
    
    set_velocity(velocity);
}

void Player::update_hunger(double delta) {
    // Decrease hunger over time
    hunger -= static_cast<int>(2.0 * delta); // Lose 2 hunger per second
    
    if (hunger <= 0) {
        hunger = 0;
        // Take damage when hungry
        take_damage(static_cast<int>(5.0 * delta));
    }
}

void Player::take_damage(int amount) {
    health -= amount;
    
    if (health <= 0) {
        health = 0;
        is_alive = false;
        // Trigger death animation/logic
        set_visible(false);
    }
}

void Player::heal(int amount) {
    health += amount;
    if (health > max_health) {
        health = max_health;
    }
}

void Player::eat(int amount) {
    hunger += amount;
    if (hunger > max_hunger) {
        hunger = max_hunger;
    }
}

void Player::set_health(int h) {
    health = h;
    if (health <= 0) {
        is_alive = false;
    }
}

void Player::set_hunger(int h) {
    hunger = h;
}