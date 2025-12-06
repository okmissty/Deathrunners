#ifndef MAIN_SCRIPT_H
#define MAIN_SCRIPT_H

#include <godot_cpp/classes/node.hpp>

namespace godot {

class MainScript : public Node {
    GDCLASS(MainScript, Node)

protected:
    static void _bind_methods();

public:
    MainScript();
    ~MainScript();

    void _ready() override;
};

} // namespace godot

#endif // MAIN_SCRIPT_H
