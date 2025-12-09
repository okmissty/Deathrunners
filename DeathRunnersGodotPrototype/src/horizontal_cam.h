#ifndef HORIZONTAL_CAM_H
#define HORIZONTAL_CAM_H


#include <godot_cpp/classes/node2d.hpp>

namespace godot {

/** Simple horizontal camera helper node. */
class HorizontalCam : public Node2D {
    GDCLASS(HorizontalCam, Node2D)

protected:
    /** Bind methods with Godot. */
    static void _bind_methods();

public:
    HorizontalCam();
    ~HorizontalCam();

    /** Called when the camera node is added to the scene. */
    void _ready() override;
    /** Update camera every frame. */
    void _process(double delta) override;
};

} // namespace godot

#endif // HORIZONTAL_CAM_H
