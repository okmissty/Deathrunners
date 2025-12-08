#ifndef HAZARD_H
#define HAZARD_H


#include <godot_cpp/classes/node2d.hpp>

namespace godot {

/** A scene hazard (damage, trap or obstacle). */
class Hazard : public Node2D {
    GDCLASS(Hazard, Node2D)

protected:
    /** Register API with Godot. */
    static void _bind_methods();

public:
    /** Construct hazard. */
    Hazard();
    /** Destroy hazard. */
    ~Hazard();

    /** Node ready callback. */
    void _ready() override;
};

} // namespace godot

#endif // HAZARD_H
