#ifndef HEALTH_PICKUP_H
#define HEALTH_PICKUP_H

#include <godot_cpp/classes/node.hpp>

namespace godot {

class HealthPickup : public Node {
    GDCLASS(HealthPickup, Node)

protected:
    static void _bind_methods();

public:
    HealthPickup();
    ~HealthPickup();

    void _ready() override;
};

} // namespace godot

#endif // HEALTH_PICKUP_H
