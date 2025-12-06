#ifndef HORIZONTAL_CAM_H
#define HORIZONTAL_CAM_H

#include <godot_cpp/classes/node2d.hpp>

namespace godot {

class HorizontalCam : public Node2D {
    GDCLASS(HorizontalCam, Node2D)

protected:
    static void _bind_methods();

public:
    HorizontalCam();
    ~HorizontalCam();

    void _ready() override;
    void _process(double delta) override;
};

} // namespace godot

#endif // HORIZONTAL_CAM_H
