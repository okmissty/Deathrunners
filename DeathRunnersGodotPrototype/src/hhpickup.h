#ifndef HH_PICKUP_H
#define HH_PICKUP_H

#include <godot_cpp/classes/node.hpp>

namespace godot {

class HHPickup : public Node {
    GDCLASS(HHPickup, Node)

protected:
    static void _bind_methods();

public:
    HHPickup();
    ~HHPickup();

    void _ready() override;
};

} // namespace godot

#endif // HH_PICKUP_H
