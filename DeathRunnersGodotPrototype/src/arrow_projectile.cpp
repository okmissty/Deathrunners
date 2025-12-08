#include "arrow_projectile.h"

using namespace godot;

void ArrowProjectile::_bind_methods() {}

ArrowProjectile::ArrowProjectile() {
    speed = 400.0f;
    direction = Vector2(1, 0); // Default fire right
    lifetime = 5.0f;
    set_damage(20); // Inherited from Obstacle
}

ArrowProjectile::~ArrowProjectile() {}

void ArrowProjectile::_ready() {
    Obstacle::_ready(); // Don't forget to call parent ready!
}

void ArrowProjectile::_process(double delta) {
    // Move
    Vector2 pos = get_position();
    pos += direction * speed * delta;
    set_position(pos);
    
    // Cleanup
    lifetime -= delta;
    if (lifetime <= 0) {
        queue_free();
    }
}