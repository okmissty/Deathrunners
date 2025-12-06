#ifndef HAZARD_H
#define HAZARD_H

#include <godot_cpp/classes/node2d.hpp>

namespace godot {

class Hazard : public Node2D {
    GDCLASS(Hazard, Node2D)

protected:
    static void _bind_methods();

public:
    Hazard();
    ~Hazard();

    void _ready() override;
};

} // namespace godot

#endif // HAZARD_H
