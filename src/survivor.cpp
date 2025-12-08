#include "survivor.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/input.hpp>

using namespace godot;

void Survivor::_bind_methods() {
    ClassDB::bind_method(D_METHOD("take_damage", "amount"), &Survivor::take_damage);
    ClassDB::bind_method(D_METHOD("heal", "amount"), &Survivor::heal);
    ClassDB::bind_method(D_METHOD("eat", "amount"), &Survivor::eat);

    ADD_PROPERTY(PropertyInfo(Variant::INT, "health"), "set_health", "set_health");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "hunger"), "set_hunger", "set_hunger");
}

Survivor::Survivor() {
    max_health = 100;
    health = max_health;
    max_hunger = 100;
    hunger = max_hunger;
    alive = true;
    death_y = 3000.0f;
}

Survivor::~Survivor() {
}

void Survivor::_ready() {
    // Minimal setup for PoC
}

void Survivor::_physics_process(double delta) {
    // Simple fall-death check similar to GDScript version
    if (get_global_position().y > death_y) {
        take_damage(health);
    }
}

void Survivor::_process(double delta) {
    Ref<MultiplayerAPI> mp = get_multiplayer();
    if (mp.is_valid() && is_multiplayer_authority()) {
        // hunger logic could go here in a fuller conversion
        // For PoC keep it minimal
    }
}

void Survivor::take_damage(int amount) {
    health -= amount;
    if (health <= 0) {
        health = 0;
        alive = false;
        set_visible(false);
    }
}

void Survivor::heal(int amount) {
    health += amount;
    if (health > max_health) health = max_health;
}

void Survivor::eat(int amount) {
    hunger += amount;
    if (hunger > max_hunger) hunger = max_hunger;
}

void Survivor::set_health(int h) {
    health = h;
    if (health <= 0) alive = false;
}

void Survivor::set_hunger(int h) {
    hunger = h;
}
