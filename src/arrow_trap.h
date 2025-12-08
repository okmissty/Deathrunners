#ifndef ARROW_TRAP_H
#define ARROW_TRAP_H


#include <godot_cpp/classes/node2d.hpp>

namespace godot {

/** Arrow trap skeleton (fires projectiles). */
class ArrowTrap : public Node2D {
    GDCLASS(ArrowTrap, Node2D)

protected:
    /** Register methods/properties with Godot. */
    static void _bind_methods();

public:
    /** Construct ArrowTrap. */
    ArrowTrap();
    /** Destroy ArrowTrap. */
    ~ArrowTrap();

    /** Called when node enters scene. */
    void _ready() override;
    /** Called each frame with delta seconds. */
    void _process(double delta) override;
};

} // namespace godot

#endif // ARROW_TRAP_H
