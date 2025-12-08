#ifndef CHECKPOINT_H
#define CHECKPOINT_H


#include <godot_cpp/classes/node2d.hpp>

namespace godot {

/** Persistent checkpoint node used for respawning players. */
class Checkpoint : public Node2D {
    GDCLASS(Checkpoint, Node2D)

protected:
    /** Register methods/properties to Godot. */
    static void _bind_methods();

public:
    /** Construct a checkpoint. */
    Checkpoint();
    /** Destructor. */
    ~Checkpoint();

    /** Called when the node enters the scene. */
    void _ready() override;
};

} // namespace godot

#endif // CHECKPOINT_H
