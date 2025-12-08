#ifndef ARROW_PROJECTILE_H
#define ARROW_PROJECTILE_H

#include "obstacle.h" // Inherit from Obstacle instead of Node2D!

namespace godot {

class ArrowProjectile : public Obstacle { // Changed from Node2D
    GDCLASS(ArrowProjectile, Obstacle)

private:
    float speed;
    Vector2 direction;
    float lifetime;

protected:
    static void _bind_methods();

public:
    ArrowProjectile();
    ~ArrowProjectile();

    void _ready() override;
    void _process(double delta) override;
};

} // namespace godot

#endif // ARROW_PROJECTILE_H