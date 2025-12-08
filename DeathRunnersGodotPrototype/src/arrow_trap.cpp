#include "arrow_trap.h"
#include "arrow_projectile.h" // Include the projectile header
#include <godot_cpp/classes/resource_loader.hpp>

using namespace godot;

void ArrowTrap::_bind_methods() {} // No new methods needed for now

ArrowTrap::ArrowTrap() {
    shoot_cooldown = 2.0f; // Shoot every 2 seconds
    timer = 0.0f;
}

ArrowTrap::~ArrowTrap() {}

void ArrowTrap::_ready() {
    // Load the projectile scene
    ResourceLoader *loader = ResourceLoader::get_singleton();
    projectile_scene = loader->load("res://scenes/arrow_projectile.tscn");
}

void ArrowTrap::_process(double delta) {
    if (!is_multiplayer_authority()) return; // Only server spawns arrows

    timer += delta;
    if (timer >= shoot_cooldown) {
        timer = 0.0f;
        
        if (projectile_scene.is_valid()) {
            Node *node = projectile_scene->instantiate();
            ArrowProjectile *arrow = Object::cast_to<ArrowProjectile>(node);
            if (arrow) {
                // Add to parent so it doesn't move with the trap
                get_parent()->add_child(arrow);
                arrow->set_global_position(get_global_position());
            }
        }
    }
}