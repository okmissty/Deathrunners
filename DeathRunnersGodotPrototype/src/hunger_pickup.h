#ifndef HUNGER_PICKUP_H
#define HUNGER_PICKUP_H


#include <godot_cpp/classes/area2d.hpp>

namespace godot {

/** Hunger-only pickup node (restores hunger). */
class HungerPickup : public Area2D {
    GDCLASS(HungerPickup, Area2D)

protected:
    /** Bind methods/properties for Godot. */
    static void _bind_methods();

public:
    /** Construct hunger pickup. */
    HungerPickup();
    /** Destructor. */
    ~HungerPickup();

    /** Called when node enters scene. */
    void _ready() override;
};

} // namespace godot

#endif // HUNGER_PICKUP_H
