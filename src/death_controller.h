#ifndef DEATH_CONTROLLER_H
#define DEATH_CONTROLLER_H

#include <godot_cpp/classes/node.hpp>

namespace godot {

class DeathController : public Node {
    GDCLASS(DeathController, Node)

protected:
    static void _bind_methods();

public:
    DeathController();
    ~DeathController();

    void _ready() override;
    void _process(double delta) override;
};

} // namespace godot

#endif // DEATH_CONTROLLER_H
