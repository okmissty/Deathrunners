#include "death_player.h"
#include "obstacle.h"
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/input_event_mouse_button.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/multiplayer_api.hpp>

using namespace godot;

void DeathPlayer::_bind_methods() {
    ClassDB::bind_method(D_METHOD("spawn_obstacle", "position", "type"), 
                        &DeathPlayer::spawn_obstacle);
    ClassDB::bind_method(D_METHOD("rpc_spawn_obstacle", "position", "type"), 
                        &DeathPlayer::rpc_spawn_obstacle);
    
    // Mark rpc_spawn_obstacle as an RPC method
    Dictionary rpc_config;
    rpc_config["rpc_mode"] = MultiplayerAPI::RPC_MODE_ANY_PEER;
    rpc_config["call_local"] = true;
    rpc_config["transfer_mode"] = MultiplayerPeer::TRANSFER_MODE_RELIABLE;
    
    ClassDB::add_property_group("DeathPlayer", "Spawning", "spawn_");
}

DeathPlayer::DeathPlayer() {
    spawn_cooldown = 1.0f; // 1 second cooldown
    cooldown_timer = 0.0f;
    spawn_limit = 50;
    spawned_count = 0;
}

DeathPlayer::~DeathPlayer() {
}

void DeathPlayer::_ready() {
    // Load obstacle scenes
    ResourceLoader *loader = ResourceLoader::get_singleton();
    spike_scene = loader->load("res://scenes/spike.tscn");
    pit_scene = loader->load("res://scenes/pit.tscn");
    enemy_scene = loader->load("res://scenes/enemy.tscn");
}

void DeathPlayer::_process(double delta) {
    // Update cooldown
    if (cooldown_timer > 0) {
        cooldown_timer -= delta;
    }
}

void DeathPlayer::_input(const Ref<InputEvent> &event) {
    // Only process if this is the Death player
    MultiplayerAPI *multiplayer = get_multiplayer();
    if (!multiplayer || !is_multiplayer_authority()) {
        return;
    }
    
    // Check for mouse click
    Ref<InputEventMouseButton> mouse_event = event;
    if (mouse_event.is_valid() && mouse_event->is_pressed()) {
        if (can_spawn()) {
            Vector2 mouse_pos = mouse_event->get_position();
            
            // Determine obstacle type based on mouse button or key
            int type = 0; // Default to spike
            if (Input::get_singleton()->is_key_pressed(KEY_1)) type = 0;
            if (Input::get_singleton()->is_key_pressed(KEY_2)) type = 1;
            if (Input::get_singleton()->is_key_pressed(KEY_3)) type = 2;
            
            // Call RPC to spawn on all clients
            rpc_spawn_obstacle(mouse_pos, type);
        }
    }
}

void DeathPlayer::spawn_obstacle(Vector2 position, int type) {
    if (spawned_count >= spawn_limit) {
        return;
    }
    
    Ref<PackedScene> scene;
    switch(type) {
        case 0: scene = spike_scene; break;
        case 1: scene = pit_scene; break;
        case 2: scene = enemy_scene; break;
        default: scene = spike_scene;
    }
    
    if (scene.is_valid()) {
        Node *obstacle = scene->instantiate();
        if (obstacle) {
            get_parent()->add_child(obstacle);
            
            // Set position
            Node2D *obstacle_2d = Object::cast_to<Node2D>(obstacle);
            if (obstacle_2d) {
                obstacle_2d->set_position(position);
            }
            
            spawned_count++;
            cooldown_timer = spawn_cooldown;
        }
    }
}

void DeathPlayer::rpc_spawn_obstacle(Vector2 position, int type) {
    spawn_obstacle(position, type);
}

bool DeathPlayer::can_spawn() const {
    return cooldown_timer <= 0 && spawned_count < spawn_limit;
}