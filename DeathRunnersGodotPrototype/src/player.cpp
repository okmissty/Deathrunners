#include "player.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/multiplayer_api.hpp>
#include <godot_cpp/classes/multiplayer_peer.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void Player::_bind_methods() {
    ClassDB::bind_method(D_METHOD("take_damage", "amount"), &Player::take_damage);
    ClassDB::bind_method(D_METHOD("heal", "amount"), &Player::heal);
    ClassDB::bind_method(D_METHOD("eat", "amount"), &Player::eat);
    ClassDB::bind_method(D_METHOD("mark_goal_reached"), &Player::mark_goal_reached);
    
    // RPCs
    ClassDB::bind_method(D_METHOD("sync_respawn", "pos"), &Player::sync_respawn);
    ClassDB::bind_method(D_METHOD("sync_death"), &Player::sync_death);
    ClassDB::bind_method(D_METHOD("sync_goal_reached"), &Player::sync_goal_reached);
    
    // Properties
    ClassDB::bind_method(D_METHOD("set_health", "health"), &Player::set_health);
    ClassDB::bind_method(D_METHOD("get_health"), &Player::get_health);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "health"), "set_health", "get_health");

    ClassDB::bind_method(D_METHOD("set_hunger", "hunger"), &Player::set_hunger);
    ClassDB::bind_method(D_METHOD("get_hunger"), &Player::get_hunger);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "hunger"), "set_hunger", "get_hunger");

    ClassDB::bind_method(D_METHOD("set_lives", "lives"), &Player::set_lives);
    ClassDB::bind_method(D_METHOD("get_lives"), &Player::get_lives);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "lives"), "set_lives", "get_lives");
    
    ClassDB::bind_method(D_METHOD("sync_state", "pos", "vel", "h", "hu", "l", "anim", "flip"), &Player::sync_state);
}

Player::Player() {
    speed = 200.0f;
    jump_velocity = -400.0f;
    gravity = 980.0f;
    
    health = 100.0f;
    max_health = 100.0f;
    hunger = 100.0f;
    max_hunger = 100.0f;
    lives = 3;
    max_lives = 3;
    
    is_alive = true;
    reached_goal = false;
    
    sprite = nullptr;
    health_bar = nullptr;
    hunger_bar = nullptr;
}

Player::~Player() {}

void Player::_ready() {
    Dictionary sync_opts;
    sync_opts["rpc_mode"] = MultiplayerAPI::RPC_MODE_ANY_PEER;
    sync_opts["transfer_mode"] = MultiplayerPeer::TRANSFER_MODE_UNRELIABLE_ORDERED;
    sync_opts["call_local"] = false; 
    rpc_config("sync_state", sync_opts);

    Dictionary reliable_opts;
    reliable_opts["rpc_mode"] = MultiplayerAPI::RPC_MODE_ANY_PEER;
    reliable_opts["transfer_mode"] = MultiplayerPeer::TRANSFER_MODE_RELIABLE;
    reliable_opts["call_local"] = true;
    
    rpc_config("apply_damage", reliable_opts);
    rpc_config("sync_respawn", reliable_opts);
    rpc_config("sync_death", reliable_opts);
    rpc_config("sync_goal_reached", reliable_opts);
    rpc_config("sync_health", reliable_opts);
    rpc_config("sync_hunger", reliable_opts);

    // --- FIX 1: Use Object::cast_to instead of template get_node ---
    sprite = Object::cast_to<AnimatedSprite2D>(get_node_or_null("AnimatedSprite2D"));
    
    checkpoint_position = get_global_position();
    add_to_group("player");
    
    call_deferred("_setup_ui_bars");
}

void Player::_setup_ui_bars() {
    Ref<MultiplayerAPI> mp = get_multiplayer();
    if (!mp->has_multiplayer_peer() || is_multiplayer_authority()) {
        SceneTree* tree = get_tree();
        if (tree && tree->get_current_scene()) {
            // --- FIX 2: Use Object::cast_to for UI bars ---
            Node* health_node = tree->get_current_scene()->get_node_or_null("UI/BarsContainer/HealthRow/HealthBar");
            health_bar = Object::cast_to<ProgressBar>(health_node);

            Node* hunger_node = tree->get_current_scene()->get_node_or_null("UI/BarsContainer/HungerRow/HungerBar");
            hunger_bar = Object::cast_to<ProgressBar>(hunger_node);
            
            _update_ui();
        }
    }
}

void Player::_physics_process(double delta) {
    if (!is_alive || reached_goal) return;

    Ref<MultiplayerAPI> mp = get_multiplayer();
    bool is_auth = !mp->has_multiplayer_peer() || is_multiplayer_authority();

    if (!is_auth) {
        return; 
    }

    if (!is_on_floor()) {
        Vector2 vel = get_velocity();
        vel.y += gravity * delta;
        set_velocity(vel);
    }

    handle_input(delta);
    
    // Update hunger
    if (get_velocity().length() > 10.0f) {
        hunger -= 15.0f * delta;
    }
    if (hunger <= 0) {
        hunger = 0;
        take_damage(2.0f * delta);
    }

    // Fall death check
    if (get_global_position().y > 2000.0f) {
        take_damage(1000.0f);
    }

    move_and_slide();
    _clamp_to_camera_bounds();
    _update_ui();

    // Sync state to other clients
    if (mp->has_multiplayer_peer()) {
        String current_anim = "idle";
        bool current_flip = false;
        if (sprite) {
            current_anim = sprite->get_animation();
            current_flip = sprite->is_flipped_h();
        }
        
        Array args;
        args.push_back(get_global_position());
        args.push_back(get_velocity());
        args.push_back(health);
        args.push_back(hunger);
        args.push_back(lives);
        args.push_back(current_anim);
        args.push_back(current_flip);
        
        rpc("sync_state", args);
    }
}

void Player::sync_state(Vector2 pos, Vector2 vel, float h, float hu, int l, String anim, bool flip) {
    if (!is_multiplayer_authority()) {
        set_global_position(pos);
        set_velocity(vel);
        health = h;
        hunger = hu;
        lives = l;
        
        if (sprite) {
            sprite->play(anim);
            sprite->set_flip_h(flip);
        }
    }
}

void Player::_process(double delta) {
}

void Player::handle_input(double delta) {
    Input* input = Input::get_singleton();
    Vector2 velocity = get_velocity();
    float dir = 0.0f;

    if (input->is_action_pressed("ui_left")) dir -= 1.0f;
    if (input->is_action_pressed("ui_right")) dir += 1.0f;
    
    velocity.x = dir * speed;

    if (input->is_action_just_pressed("ui_accept") && is_on_floor()) {
        velocity.y = jump_velocity;
    }
    
    set_velocity(velocity);
    _handle_animations(dir);
}

void Player::_handle_animations(float dir) {
    if (!sprite) return;
    
    if (dir > 0) sprite->set_flip_h(false);
    else if (dir < 0) sprite->set_flip_h(true);
    
    if (!is_on_floor()) {
        if (get_velocity().y < 0) sprite->play("jump");
        else sprite->play("fall");
    } else {
        if (Math::abs(get_velocity().x) > 0.1f) sprite->play("run");
        else sprite->play("idle");
    }
}

void Player::_clamp_to_camera_bounds() {
    Viewport* vp = get_viewport();
    if (!vp) return;
    Camera2D* cam = vp->get_camera_2d();
    if (!cam) return;
    
    float cam_x = cam->get_global_position().x;
    Rect2 visible_rect = vp->get_visible_rect();
    float half_width = (visible_rect.size.x / cam->get_zoom().x) * 0.5f;
    
    float padding = 32.0f;
    float left = cam_x - half_width + padding;
    float right = cam_x + half_width - padding;
    
    Vector2 pos = get_global_position();
    pos.x = CLAMP(pos.x, left, right);
    set_global_position(pos);
}

void Player::take_damage(float amount) {
    if (!is_alive) return;
    
    health -= amount;
    if (health <= 0) {
        health = 0;
        lives--;
        UtilityFunctions::print("Player lost a life. Lives left: ", lives);
        
        if (lives > 0) {
            respawn();
        } else {
            is_alive = false;
            if (sprite) sprite->set_modulate(Color(0.5, 0.5, 0.5, 0.5));
            if (get_multiplayer()->has_multiplayer_peer()) {
                rpc("sync_death");
            }
        }
    }
    _update_ui();
}

void Player::respawn() {
    set_global_position(checkpoint_position);
    set_velocity(Vector2(0, 0));
    health = max_health;
    hunger = max_hunger;
    
    if (get_multiplayer()->has_multiplayer_peer()) {
        Array args;
        args.push_back(checkpoint_position);
        rpc("sync_respawn", args);
    }
}

void Player::sync_respawn(Vector2 pos) {
    set_global_position(pos);
    set_velocity(Vector2(0, 0));
}

void Player::sync_death() {
    is_alive = false;
    if (sprite) sprite->set_modulate(Color(0.5, 0.5, 0.5, 0.5));
}

void Player::mark_goal_reached() {
    reached_goal = true;
    if (get_multiplayer()->has_multiplayer_peer()) {
        rpc("sync_goal_reached");
    }
    
    Node* main = get_tree()->get_current_scene();
    if (main) main->call_deferred("_show_game_over", "Survivors win!");
}

void Player::sync_goal_reached() {
    reached_goal = true;
}

void Player::_update_ui() {
    if (health_bar) health_bar->set_value(health);
    if (hunger_bar) hunger_bar->set_value(hunger);
}

void Player::heal(float amount) {
    health = Math::min(max_health, health + amount);
    _update_ui();
}

void Player::eat(float amount) {
    hunger = Math::min(max_hunger, hunger + amount);
    _update_ui();
}