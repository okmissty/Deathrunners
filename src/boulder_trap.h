#ifndef BOULDER_TRAP_H
#define BOULDER_TRAP_H

#include <godot_cpp/classes/node2d.hpp>

namespace godot {

class BoulderTrap : public Node2D {
    GDCLASS(BoulderTrap, Node2D)

protected:
    static void _bind_methods();

public:
    BoulderTrap();
    ~BoulderTrap();

    void _ready() override;
    void _process(double delta) override;
};

} // namespace godot

#endif // BOULDER_TRAP_H
