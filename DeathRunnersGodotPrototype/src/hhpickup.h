#ifndef HH_PICKUP_H
#define HH_PICKUP_H

#include <godot_cpp/classes/area2d.hpp>

namespace godot {

class HHPickup : public Area2D {
    GDCLASS(HHPickup, Area2D)

protected:
    static void _bind_methods();

public:
    HHPickup();
    ~HHPickup();

    void _ready() override;
};

} // namespace godot

#endif // HH_PICKUP_H
