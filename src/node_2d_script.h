#ifndef NODE_2D_SCRIPT_H
#define NODE_2D_SCRIPT_H

#include <godot_cpp/classes/node2d.hpp>

namespace godot {

class Node2DScript : public Node2D {
    GDCLASS(Node2DScript, Node2D)

protected:
    static void _bind_methods();

public:
    Node2DScript();
    ~Node2DScript();

    void _ready() override;
};

} // namespace godot

#endif // NODE_2D_SCRIPT_H
