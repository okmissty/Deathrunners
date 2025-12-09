#include "death_controller.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/multiplayer_peer.hpp>

using namespace godot;

void DeathController::_bind_methods() {
    // RPCs
    ClassDB::bind_method(D_METHOD("activate_trap_networked", "trap_path"), &DeathController::activate_trap_networked);
    ClassDB::bind_method(D_METHOD("spawn_aoe_networked", "pos"), &DeathController::spawn_aoe_networked);
    ClassDB::bind_method(D_METHOD("spawn_falling_networked", "pos"), &DeathController::spawn_falling_networked);
    ClassDB::bind_method(D_METHOD("_sort_traps_method_wrapper", "a", "b"), &DeathController::_sort_traps_method_wrapper);
    
    // Callable for sorting
    ClassDB::bind_method(D_METHOD("_refresh_players"), &DeathController::_refresh_players);

    // Properties
    ClassDB::bind_method(D_METHOD("set_aoe_scene", "scene"), &DeathController::set_aoe_scene);
    ClassDB::bind_method(D_METHOD("get_aoe_scene"), &DeathController::get_aoe_scene);
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "aoe_scene", PROPERTY_HINT_RESOURCE_TYPE, "PackedScene"), "set_aoe_scene", "get_aoe_scene");

    ClassDB::bind_method(D_METHOD("set_falling_block_scene", "scene"), &DeathController::set_falling_block_scene);
    ClassDB::bind_method(D_METHOD("get_falling_block_scene"), &DeathController::get_falling_block_scene);
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "falling_block_scene", PROPERTY_HINT_RESOURCE_TYPE, "PackedScene"), "set_falling_block_scene", "get_falling_block_scene");
}

DeathController::DeathController() {
    selected_trap_index = -1;
    selected_player_index = 0;
    trap_label = nullptr;
    target_label = nullptr;
    trap_indicator = nullptr;
    player_indicator = nullptr;
    trap_indicator_base_scale = Vector2(1, 1);
    player_indicator_base_scale = Vector2(1, 1);
    enabled = false;
}

DeathController::~DeathController() {}

void DeathController::_ready() {
    Dictionary opts;
    opts["rpc_mode"] = MultiplayerAPI::RPC_MODE_ANY_PEER;
    opts["transfer_mode"] = MultiplayerPeer::TRANSFER_MODE_RELIABLE;
    opts["call_local"] = true;

    rpc_config("activate_trap_networked", opts);
    rpc_config("spawn_aoe_networked", opts);
    rpc_config("spawn_falling_networked", opts);

    // Find UI nodes relative to this node
    trap_label = Object::cast_to<Label>(get_node_or_null("../UI/Death HUD/TrapLabel"));
    target_label = Object::cast_to<Label>(get_node_or_null("../UI/Death HUD/TargetLabel"));
    
    trap_indicator = Object::cast_to<Node2D>(get_node_or_null("TrapIndicator"));
    player_indicator = Object::cast_to<Node2D>(get_node_or_null("PlayerIndicator"));
    
    if (trap_indicator) trap_indicator_base_scale = trap_indicator->get_scale();
    if (player_indicator) player_indicator_base_scale = player_indicator->get_scale();

    // Networking check
    if (get_multiplayer()->has_multiplayer_peer()) {
        enabled = !get_multiplayer()->is_server(); // Death is client usually
        set_process(enabled);
    } else {
        enabled = true;
    }

    _refresh_preplaced_traps();
    _refresh_players();
    
    _update_trap_highlight();
    _update_player_highlight();
}

void DeathController::_process(double delta) {
    if (!enabled) return;

    Input* input = Input::get_singleton();

    if (input->is_action_just_pressed("death_trap_prev")) _select_prev_trap();
    if (input->is_action_just_pressed("death_trap_next")) _select_next_trap();
    if (input->is_action_just_pressed("death_trap_activate")) _activate_selected_trap();

    if (input->is_action_just_pressed("death_player_prev")) _select_prev_player();
    if (input->is_action_just_pressed("death_player_next")) _select_next_player();

    if (input->is_action_just_pressed("death_spawn_aoe")) _spawn_aoe_on_selected_player();
    if (input->is_action_just_pressed("death_spawn_falling")) _spawn_falling_on_selected_player();

    _update_player_indicator_follow();
    _update_trap_hud(_current_trap());
    _update_player_hud(_current_player());
}

void DeathController::_refresh_players() {
    players = get_tree()->get_nodes_in_group("player");
    if (players.size() > 0) {
        if (selected_player_index < 0 || selected_player_index >= players.size()) {
            selected_player_index = 0;
        }
    } else {
        selected_player_index = -1;
    }
}

bool DeathController::_sort_traps_by_x(const Node* a, const Node* b) {
    const Node2D* node_a = Object::cast_to<Node2D>(a);
    const Node2D* node_b = Object::cast_to<Node2D>(b);
    
    if (node_a && node_b) {
        return node_a->get_global_position().x < node_b->get_global_position().x;
    }
    return false;
}

bool DeathController::_sort_traps_method_wrapper(Node* a, Node* b) {
    return _sort_traps_by_x(a, b);
}

void DeathController::_refresh_preplaced_traps() {
    preplaced_traps.clear();
    
    // Get nodes
    Array boulders = get_tree()->get_nodes_in_group("trap_boulder");
    Array arrows = get_tree()->get_nodes_in_group("trap_arrow");
    
    preplaced_traps.append_array(boulders);
    preplaced_traps.append_array(arrows);

    // Sort traps
    preplaced_traps.sort_custom(Callable(this, "_sort_traps_method_wrapper"));

    if (preplaced_traps.size() > 0) {
        if (selected_trap_index < 0 || selected_trap_index >= preplaced_traps.size()) 
            selected_trap_index = 0;
    } else {
        selected_trap_index = -1;
    }
}

Node2D* DeathController::_current_player() {
    if (selected_player_index < 0 || selected_player_index >= players.size()) return nullptr;
    return Object::cast_to<Node2D>(players[selected_player_index]);
}

Node2D* DeathController::_current_trap() {
    if (selected_trap_index < 0 || selected_trap_index >= preplaced_traps.size()) return nullptr;
    return Object::cast_to<Node2D>(preplaced_traps[selected_trap_index]);
}

void DeathController::_select_prev_trap() {
    if (preplaced_traps.is_empty()) return;
    selected_trap_index = (selected_trap_index - 1 + preplaced_traps.size()) % preplaced_traps.size();
    _update_trap_highlight();
}

void DeathController::_select_next_trap() {
    if (preplaced_traps.is_empty()) return;
    selected_trap_index = (selected_trap_index + 1) % preplaced_traps.size();
    _update_trap_highlight();
}

void DeathController::_activate_selected_trap() {
    Node2D* trap = _current_trap();
    if (!trap) return;

    if (trap->has_method("can_activate")) {
        if (!(bool)trap->call("can_activate")) return;
    }

    if (get_multiplayer()->has_multiplayer_peer()) {
        Array args;
        args.push_back(trap->get_path());
        rpc("activate_trap_networked", args);
    } else {
        if (trap->has_method("activate")) trap->call("activate");
    }
}

void DeathController::activate_trap_networked(NodePath trap_path) {
    Node* trap = get_node_or_null(trap_path);
    if (trap && trap->has_method("activate")) {
        trap->call("activate");
    }
}

void DeathController::_select_prev_player() {
    if (players.is_empty()) return;
    selected_player_index = (selected_player_index - 1 + players.size()) % players.size();
    _update_player_highlight();
}

void DeathController::_select_next_player() {
    if (players.is_empty()) return;
    selected_player_index = (selected_player_index + 1) % players.size();
    _update_player_highlight();
}

void DeathController::_spawn_aoe_on_selected_player() {
    Node2D* player = _current_player();
    if (!player || aoe_scene.is_null()) return;
    Vector2 pos = player->get_global_position() + Vector2(0, 16);
    
    if (get_multiplayer()->has_multiplayer_peer()) {
        Array args;
        args.push_back(pos);
        rpc("spawn_aoe_networked", args);
    } else {
        _spawn_aoe_at_position(pos);
    }
}

void DeathController::spawn_aoe_networked(Vector2 pos) {
    _spawn_aoe_at_position(pos);
}

void DeathController::_spawn_aoe_at_position(Vector2 pos) {
    if (aoe_scene.is_null()) return;
    Node* aoe = aoe_scene->instantiate();
    get_tree()->get_current_scene()->add_child(aoe);
    Node2D* aoe2d = Object::cast_to<Node2D>(aoe);
    if (aoe2d) aoe2d->set_global_position(pos);
    if (aoe->has_method("activate")) aoe->call("activate");
}

void DeathController::_spawn_falling_on_selected_player() {
    Node2D* player = _current_player();
    if (!player || falling_block_scene.is_null()) return;
    Vector2 pos = player->get_global_position() + Vector2(0, -200);

    if (get_multiplayer()->has_multiplayer_peer()) {
        Array args;
        args.push_back(pos);
        rpc("spawn_falling_networked", args);
    } else {
        _spawn_falling_at_position(pos);
    }
}

void DeathController::spawn_falling_networked(Vector2 pos) {
    _spawn_falling_at_position(pos);
}

void DeathController::_spawn_falling_at_position(Vector2 pos) {
    if (falling_block_scene.is_null()) return;
    Node* block = falling_block_scene->instantiate();
    get_tree()->get_current_scene()->add_child(block);
    Node2D* block2d = Object::cast_to<Node2D>(block);
    if (block2d) block2d->set_global_position(pos);
}

void DeathController::_update_trap_highlight() {
    Node2D* trap = _current_trap();
    if (trap_indicator) {
        if (trap) {
            trap_indicator->set_visible(true);
            trap_indicator->set_global_position(trap->get_global_position() + Vector2(0, -24));
            _pulse_indicator(trap_indicator, trap_indicator_base_scale);
        } else {
            trap_indicator->set_visible(false);
        }
    }
    _update_trap_hud(trap);
}

void DeathController::_update_player_highlight() {
    Node2D* player = _current_player();
    if (player_indicator && player) {
        _pulse_indicator(player_indicator, player_indicator_base_scale);
    }
    _update_player_hud(player);
}

void DeathController::_update_player_indicator_follow() {
    Node2D* player = _current_player();
    if (!player_indicator) return;
    if (player) {
        player_indicator->set_visible(true);
        player_indicator->set_global_position(player->get_global_position() + Vector2(0, -40));
    } else {
        player_indicator->set_visible(false);
    }
}

void DeathController::_update_trap_hud(Node* trap) {
    if (!trap_label) return;
    if (!trap) {
        trap_label->set_text("Trap: (none)");
        return;
    }
    String base_name = "Trap";
    if (trap->is_in_group("trap_boulder")) base_name = "Boulder Trap";
    else if (trap->is_in_group("trap_arrow")) base_name = "Arrow Trap";
    
    trap_label->set_text("Trap: " + base_name);
}

void DeathController::_update_player_hud(Node* player) {
    if (!target_label) return;
    if (!player) {
        target_label->set_text("Target: (none)");
        return;
    }
    target_label->set_text("Target: " + player->get_name());
}

void DeathController::_pulse_indicator(Node2D* ind, Vector2 base_scale) {
    if (!ind) return;
    Ref<Tween> tween = create_tween();
    tween->tween_property(ind, "scale", base_scale * 1.2, 0.15);
    tween->tween_property(ind, "scale", base_scale, 0.15);
}