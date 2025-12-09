#ifndef GOAL_H
#define GOAL_H


#include <godot_cpp/classes/area2d.hpp>

namespace godot {

/** Friendly goal node; represents level end or objective. */
class Goal : public Area2D {
    GDCLASS(Goal, Area2D)

protected:
    /** Register methods with Godot. */
    static void _bind_methods();

public:
    /** Construct goal. */
    Goal();
    /** Destructor. */
    ~Goal();

    /** Called when node enters the scene. */
    void _ready() override;
};

} // namespace godot

#endif // GOAL_H
