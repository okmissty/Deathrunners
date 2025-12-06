#ifndef AOE_TRAP_H
#define AOE_TRAP_H

#include <godot_cpp/classes/node2d.hpp>

namespace godot {

class AoeTrap : public Node2D {
    GDCLASS(AoeTrap, Node2D)

protected:
    static void _bind_methods();

public:
    AoeTrap();
    ~AoeTrap();

    void _ready() override;
    void _process(double delta) override;
};

} // namespace godot

#endif // AOE_TRAP_H
