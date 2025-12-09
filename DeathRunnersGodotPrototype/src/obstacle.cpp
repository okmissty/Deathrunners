#include "obstacle.h"
#include "player.h"
#include <godot_cpp/classes/node.hpp>

using namespace godot;

void Obstacle::_bind_methods() {
    ClassDB::bind_method(D_METHOD("_on_body_entered", "body"), 
                        &Obstacle::_on_body_entered);
    
    ClassDB::bind_method(D_METHOD("set_damage", "d"), &Obstacle::set_damage);
    ClassDB::bind_method(D_METHOD("get_damage"), &Obstacle::get_damage);
    
    ADD_PROPERTY(PropertyInfo(Variant::INT, "damage"), "set_damage", "get_damage");
    
    ADD_SIGNAL(MethodInfo("player_hit", PropertyInfo(Variant::OBJECT, "player")));
}

Obstacle::Obstacle() {
    damage = 10;
    one_time_use = false;
    has_been_used = false;
}

Obstacle::~Obstacle() {
}

void Obstacle::_ready() {
    // Connect body_entered signal
    connect("body_entered", Callable(this, "_on_body_entered"));
}

void Obstacle::_on_body_entered(Node *body) {
    if (has_been_used && one_time_use) {
        return;
    }
    
    // Check if it's a player
    Player *player = Object::cast_to<Player>(body);
    if (player) {
        player->take_damage(damage);
        has_been_used = true;
        
        emit_signal("player_hit", player);
        
        // Optional: remove obstacle after use
        if (one_time_use) {
            queue_free();
        }
    }
}