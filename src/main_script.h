#ifndef MAIN_SCRIPT_H
#define MAIN_SCRIPT_H

#include <godot_cpp/classes/node.hpp>


namespace godot {

/** Top-level script for project-wide behavior (converted to C++ skeleton). */
class MainScript : public Node {
    GDCLASS(MainScript, Node)

protected:
    /** Bind any methods/properties to Godot. */
    static void _bind_methods();

public:
    MainScript();
    ~MainScript();

    /** Called when the node enters the scene tree. */
    void _ready() override;
};

} // namespace godot

#endif // MAIN_SCRIPT_H
