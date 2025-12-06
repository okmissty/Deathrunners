#ifndef ARROW_TRAP_H
#define ARROW_TRAP_H

#include <godot_cpp/classes/node2d.hpp>

namespace godot {

class ArrowTrap : public Node2D {
    GDCLASS(ArrowTrap, Node2D)

protected:
    static void _bind_methods();

public:
    ArrowTrap();
    ~ArrowTrap();

    void _ready() override;
    void _process(double delta) override;
};

} // namespace godot

#endif // ARROW_TRAP_H
