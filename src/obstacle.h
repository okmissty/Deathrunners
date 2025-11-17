#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <godot_cpp/classes/area2d.hpp>
#include <godot_cpp/classes/collision_shape2d.hpp>

namespace godot {

/**
 * @brief Base obstacle class
 * 
 * Damages players on contact
 */
class Obstacle : public Area2D {
    GDCLASS(Obstacle, Area2D)

private:
    int damage;
    bool one_time_use;
    bool has_been_used;

protected:
    static void _bind_methods();

public:
    Obstacle();
    ~Obstacle();

    void _ready() override;
    
    /**
     * @brief Called when a body enters the obstacle
     */
    void _on_body_entered(Node *body);
    
    void set_damage(int d) { damage = d; }
    int get_damage() const { return damage; }
};

} // namespace godot

#endif // OBSTACLE_H