#ifndef HEALTH_PICKUP_H
#define HEALTH_PICKUP_H


#include <godot_cpp/classes/node.hpp>

namespace godot {

/** Health-only pickup node. */
class HealthPickup : public Node {
    GDCLASS(HealthPickup, Node)

protected:
    /** Register API with Godot. */
    static void _bind_methods();

public:
    /** Create the health pickup. */
    HealthPickup();
    /** Destroy the health pickup. */
    ~HealthPickup();

    /** Called when added to the scene. */
    void _ready() override;
};

} // namespace godot

#endif // HEALTH_PICKUP_H
