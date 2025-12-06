#ifndef ARROW_PROJECTILE_H
#define ARROW_PROJECTILE_H

#include <godot_cpp/classes/node2d.hpp>

namespace godot {

class ArrowProjectile : public Node2D {
    GDCLASS(ArrowProjectile, Node2D)

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
