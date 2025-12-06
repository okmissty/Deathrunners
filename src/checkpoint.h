#ifndef CHECKPOINT_H
#define CHECKPOINT_H

#include <godot_cpp/classes/node2d.hpp>

namespace godot {

class Checkpoint : public Node2D {
    GDCLASS(Checkpoint, Node2D)

protected:
    static void _bind_methods();

public:
    Checkpoint();
    ~Checkpoint();

    void _ready() override;
};

} // namespace godot

#endif // CHECKPOINT_H
