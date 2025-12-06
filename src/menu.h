#ifndef MENU_H
#define MENU_H

#include <godot_cpp/classes/node.hpp>

namespace godot {

class Menu : public Node {
    GDCLASS(Menu, Node)

protected:
    static void _bind_methods();

public:
    Menu();
    ~Menu();

    void _ready() override;
};

} // namespace godot

#endif // MENU_H
