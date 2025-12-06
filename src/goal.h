#ifndef GOAL_H
#define GOAL_H

#include <godot_cpp/classes/node2d.hpp>

namespace godot {

class Goal : public Node2D {
    GDCLASS(Goal, Node2D)

protected:
    static void _bind_methods();

public:
    Goal();
    ~Goal();

    void _ready() override;
};

} // namespace godot

#endif // GOAL_H
