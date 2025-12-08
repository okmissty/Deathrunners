/**
 * @brief Area-of-effect trap node (skeleton)
 *
 * This is a minimal C++ skeleton converted from the GDScript `aoe_trap.gd`.
 * Methods are intentionally small and documented for further porting.
 */
#ifndef AOE_TRAP_H
#define AOE_TRAP_H

#include <godot_cpp/classes/node2d.hpp>

namespace godot {

/**
 * @brief Area-of-effect trap node (skeleton)
 *
 * This is a minimal C++ skeleton converted from the GDScript `aoe_trap.gd`.
 * Methods are intentionally small and documented for further porting.
 */
class AoeTrap : public Node2D {
    GDCLASS(AoeTrap, Node2D)

protected:
    // Bind methods to Godot (exposed to GDScript/rpc). 
    static void _bind_methods();

public:
    // Construct an AoE trap instance. 
    AoeTrap();
    // Destructor. 
    ~AoeTrap();

    // Called when node enters the scene tree.
    void _ready() override;
    // Per-frame processing (delta seconds).
    void _process(double delta) override;
};

} 

#endif
