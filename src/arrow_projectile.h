#ifndef ARROW_PROJECTILE_H
#define ARROW_PROJECTILE_H


#include <godot_cpp/classes/node2d.hpp>

namespace godot {

/** Simple arrow projectile skeleton. */
class ArrowProjectile : public Node2D {
    GDCLASS(ArrowProjectile, Node2D)

protected:
    // Bind methods/properties with Godot.
    static void _bind_methods();

public:
    // Create projectile.
    ArrowProjectile();
    // Destroy projectile.
    ~ArrowProjectile();

    // Called when added to the scene.
    void _ready() override;
    // Called once per frame.
    void _process(double delta) override;
};

}

#endif
