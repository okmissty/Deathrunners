#ifndef BOULDER_TRAP_H
#define BOULDER_TRAP_H


#include <godot_cpp/classes/node2d.hpp>

namespace godot {

/** Simple boulder trap skeleton converted from GDScript. */
class BoulderTrap : public Node2D {
    GDCLASS(BoulderTrap, Node2D)

protected:
    /** Bind methods for Godot. */
    static void _bind_methods();

public:
    /** Create a new BoulderTrap. */
    BoulderTrap();
    /** Destroy the BoulderTrap. */
    ~BoulderTrap();

    /** Node entered scene tree. */
    void _ready() override;
    /** Per-frame update. */
    void _process(double delta) override;
};

} // namespace godot

#endif // BOULDER_TRAP_H
