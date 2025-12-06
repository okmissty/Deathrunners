#ifndef HUNGER_PICKUP_H
#define HUNGER_PICKUP_H

#include <godot_cpp/classes/node.hpp>

namespace godot {

class HungerPickup : public Node {
    GDCLASS(HungerPickup, Node)

protected:
    static void _bind_methods();

public:
    HungerPickup();
    ~HungerPickup();

    void _ready() override;
};

} // namespace godot

#endif // HUNGER_PICKUP_H
