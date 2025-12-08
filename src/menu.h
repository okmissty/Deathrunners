#ifndef MENU_H
#define MENU_H

#include <godot_cpp/classes/node.hpp>


namespace godot {

/** Simple UI Menu node converted from GDScript. */
class Menu : public Node {
    GDCLASS(Menu, Node)

protected:
    /** Register menu methods and callbacks. */
    static void _bind_methods();

public:
    Menu();
    ~Menu();

    /** Called when menu is added to the scene. */
    void _ready() override;
};

} // namespace godot

#endif // MENU_H
