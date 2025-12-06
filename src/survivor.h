// Minimal Survivor GDExtension header (PoC conversion from survivor.gd)
#ifndef SURVIVOR_H
#define SURVIVOR_H

#include <godot_cpp/classes/character_body2d.hpp>
#include <godot_cpp/classes/multiplayer_api.hpp>

namespace godot {

class Survivor : public CharacterBody2D {
    GDCLASS(Survivor, CharacterBody2D)

private:
    int health;
    int max_health;
    int hunger;
    int max_hunger;
    bool alive;
    float death_y;

protected:
    static void _bind_methods();

public:
    Survivor();
    ~Survivor();

    void _ready() override;
    void _physics_process(double delta) override;
    void _process(double delta) override;

    void take_damage(int amount);
    void heal(int amount);
    void eat(int amount);
    void set_health(int h);
    void set_hunger(int h);
};

} // namespace godot

#endif // SURVIVOR_H
