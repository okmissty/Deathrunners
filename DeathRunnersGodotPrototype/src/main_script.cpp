#include "main_script.h"
#include "death_controller.h"
#include "horizontal_cam.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/multiplayer_peer.hpp>

using namespace godot;

void MainScript::_bind_methods() {
    // RPC methods
    ClassDB::bind_method(D_METHOD("player_ready", "player_id"), &MainScript::player_ready);
    ClassDB::bind_method(D_METHOD("sync_game_over", "text"), &MainScript::sync_game_over);
    ClassDB::bind_method(D_METHOD("reload_game_scene"), &MainScript::reload_game_scene);
    
    // Button callbacks
    ClassDB::bind_method(D_METHOD("_on_restart_pressed"), &MainScript::_on_restart_pressed);
    ClassDB::bind_method(D_METHOD("_on_menu_pressed"), &MainScript::_on_menu_pressed);
    
    // Network callbacks
    ClassDB::bind_method(D_METHOD("_on_peer_connected", "id"), &MainScript::_on_peer_connected);
    ClassDB::bind_method(D_METHOD("_on_peer_disconnected", "id"), &MainScript::_on_peer_disconnected);
    
    // Properties
    ClassDB::bind_method(D_METHOD("set_survivor_scene", "scene"), &MainScript::set_survivor_scene);
    ClassDB::bind_method(D_METHOD("get_survivor_scene"), &MainScript::get_survivor_scene);
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "survivor_scene", PROPERTY_HINT_RESOURCE_TYPE, "PackedScene"), 
                 "set_survivor_scene", "get_survivor_scene");
}

MainScript::MainScript() {
    game_over = false;
    death_player_id = -1;
    my_role = "";
    death_controller = nullptr;
    end_label = nullptr;
    role_label = nullptr;
    death_hud = nullptr;
    horizontal_cam = nullptr;
    multiplayer_spawner = nullptr;
    game_over_menu = nullptr;
    restart_button = nullptr;
    menu_button = nullptr;
}

MainScript::~MainScript() {}

void MainScript::_ready() {
    Dictionary opts;
    opts["rpc_mode"] = MultiplayerAPI::RPC_MODE_ANY_PEER;
    opts["transfer_mode"] = MultiplayerPeer::TRANSFER_MODE_RELIABLE;
    opts["call_local"] = true;

    rpc_config("player_ready", opts);
    rpc_config("sync_game_over", opts);
    rpc_config("reload_game_scene", opts);

    // Get node references
    death_controller = get_node<DeathController>("DeathController");
    end_label = get_node<Label>("UI/EndLabel");
    role_label = get_node<Label>("UI/RoleLabel");
    death_hud = get_node<Control>("UI/Death HUD");
    horizontal_cam = get_node<HorizontalCam>("HorizontalCam");
    multiplayer_spawner = get_node<MultiplayerSpawner>("MultiplayerSpawner");
    game_over_menu = get_node<Control>("UI/GameOverMenu");
    restart_button = get_node<Button>("UI/GameOverMenu/RestartButton");
    menu_button = get_node<Button>("UI/GameOverMenu/MenuButton");
    
    if (end_label) {
        end_label->set_text("");
    }
    
    if (game_over_menu) {
        game_over_menu->set_visible(false);
    }
    
    if (restart_button) {
        restart_button->connect("pressed", Callable(this, "_on_restart_pressed"));
    }
    
    if (menu_button) {
        menu_button->connect("pressed", Callable(this, "_on_menu_pressed"));
    }
    
    // Get player roles from metadata
    SceneTree* tree = get_tree();
    if (tree) {
        if (tree->has_meta("player_roles")) {
            player_roles = tree->get_meta("player_roles");
        }
        if (tree->has_meta("death_player_id")) {
            death_player_id = (int)tree->get_meta("death_player_id");
        }
    }
    
    Ref<MultiplayerAPI> mp = get_multiplayer();
    if (mp.is_valid() && mp->has_multiplayer_peer()) {
        setup_multiplayer();
        Array args;
        args.push_back(mp->get_unique_id());
        rpc_id(1, "player_ready", args);
    } else {
        if (tree) {
            tree->change_scene_to_file("res://menu.tscn");
        }
    }
}

void MainScript::setup_multiplayer() {
    Ref<MultiplayerAPI> mp = get_multiplayer();
    if (!mp.is_valid()) return;
    
    int my_id = mp->get_unique_id();
    my_role = player_roles.has(my_id) ? String(player_roles[my_id]) : "Unknown";
    
    UtilityFunctions::print("Setting up game. My ID: ", my_id, " My Role: ", my_role);
    
    if (role_label) {
        role_label->set_text("You are: " + my_role);
        if (my_role == "Death") {
            role_label->set_modulate(Color(1, 0.3, 0.3));
        } else {
            role_label->set_modulate(Color(0.3, 1, 0.3));
        }
    }
    
    if (my_role == "Death") {
        if (death_controller) {
            death_controller->set_process(true);
            death_controller->set("enabled", true);
            death_controller->set_visible(true);
        }
        if (death_hud) {
            death_hud->set_visible(true);
        }
    } else {
        if (death_controller) {
            death_controller->set_process(false);
            death_controller->set("enabled", false);
            death_controller->set_visible(false);
        }
        if (death_hud) {
            death_hud->set_visible(false);
        }
    }
    
    if (mp->is_server()) {
        mp->connect("peer_connected", Callable(this, "_on_peer_connected"));
        mp->connect("peer_disconnected", Callable(this, "_on_peer_disconnected"));
    }
}

void MainScript::player_ready(int player_id) {
    Ref<MultiplayerAPI> mp = get_multiplayer();
    if (!mp.is_valid() || !mp->is_server()) return;
    
    players_ready[player_id] = true;
    UtilityFunctions::print("Player ", player_id, " is ready. Total ready: ", 
                           players_ready.size(), "/", player_roles.size());
    
    if (players_ready.size() >= player_roles.size()) {
        UtilityFunctions::print("All players ready! Spawning survivors...");
        // Wait 0.5s then spawn
        SceneTree* tree = get_tree();
        if (tree) {
            call_deferred("spawn_all_survivors");
        }
    }
}

void MainScript::spawn_all_survivors() {
    Ref<MultiplayerAPI> mp = get_multiplayer();
    if (!mp.is_valid() || !mp->is_server()) return;
    
    int spawn_offset = 0;
    Array player_ids = player_roles.keys();
    
    for (int i = 0; i < player_ids.size(); i++) {
        int player_id = player_ids[i];
        String role = player_roles[player_id];
        
        if (role == "Survivor") {
            spawn_survivor_for_player(player_id, spawn_offset);
            spawn_offset += 50;
        }
    }
}

Node* MainScript::_spawn_survivor(const Dictionary& data) {
    if (!survivor_scene.is_valid()) {
        UtilityFunctions::printerr("Survivor scene not set!");
        return nullptr;
    }
    
    int player_id = data.has("id") ? (int)data["id"] : 1;
    int x_offset = data.has("x_offset") ? (int)data["x_offset"] : 0;
    
    Node* survivor_instance = survivor_scene->instantiate();
    if (!survivor_instance) return nullptr;
    
    survivor_instance->set_name("Survivor_" + String::num_int64(player_id));
    
    Node2D* survivor_node2d = Object::cast_to<Node2D>(survivor_instance);
    if (survivor_node2d) {
        survivor_node2d->set_position(Vector2(352 + x_offset, 501));
        survivor_node2d->set_scale(Vector2(3, 3));
    }
    
    survivor_instance->add_to_group("player");
    survivor_instance->call("set_multiplayer_authority", player_id);
    
    survivors[player_id] = survivor_instance;
    
    UtilityFunctions::print("Spawned survivor for player ", player_id);
    return survivor_instance;
}

void MainScript::spawn_survivor_for_player(int player_id, int x_offset) {
    if (survivors.has(player_id)) return;
    
    if (!survivor_scene.is_valid()) {
        UtilityFunctions::printerr("Survivor scene not set!");
        return;
    }
    
    Node* survivor_instance = survivor_scene->instantiate();
    if (!survivor_instance) return;
    
    survivor_instance->set_name("Survivor_" + String::num_int64(player_id));
    
    Node2D* survivor_node2d = Object::cast_to<Node2D>(survivor_instance);
    if (survivor_node2d) {
        survivor_node2d->set_position(Vector2(352 + x_offset, 501));
        survivor_node2d->set_scale(Vector2(3, 3));
    }
    
    survivor_instance->add_to_group("player");
    add_child(survivor_instance);
    survivor_instance->call("set_multiplayer_authority", player_id);
    
    survivors[player_id] = survivor_instance;
    
    UtilityFunctions::print("Spawned survivor for player ", player_id);
}

void MainScript::_on_peer_connected(int id) {
    UtilityFunctions::print("Peer connected to game: ", id);
}

void MainScript::_on_peer_disconnected(int id) {
    UtilityFunctions::print("Peer disconnected from game: ", id);
    
    if (survivors.has(id)) {
        Node* survivor = Object::cast_to<Node>(survivors[id]);
        if (survivor && survivor->is_inside_tree()) {
            survivor->queue_free();
        }
        survivors.erase(id);
    }
}

void MainScript::_process(double delta) {
    if (game_over) return;
    
    // Periodic refresh for death controller
    Engine* engine = Engine::get_singleton();
    if (engine && engine->get_process_frames() % 60 == 0) {
        if (death_controller && death_controller->get("enabled")) {
            death_controller->call("_refresh_players");
        }
    }
    
    // Check win conditions
    bool any_survivor_alive = false;
    bool any_survivor_won = false;
    int survivor_count = 0;
    
    Array survivor_values = survivors.values();
    for (int i = 0; i < survivor_values.size(); i++) {
        Node* survivor = Object::cast_to<Node>(survivor_values[i]);
        if (survivor && survivor->is_inside_tree()) {
            survivor_count++;
            
            bool alive = survivor->get("alive");
            bool reached_goal = survivor->get("reached_goal");
            
            if (alive) any_survivor_alive = true;
            if (reached_goal) any_survivor_won = true;
        }
    }
    
    if (survivor_count > 0) {
        if (any_survivor_won) {
            _show_game_over("Survivors win!");
        } else if (!any_survivor_alive) {
            _show_game_over("Death wins!");
        }
    }
}

void MainScript::_show_game_over(const String& text) {
    if (game_over) return;
    
    game_over = true;
    if (end_label) {
        end_label->set_text(text);
    }
    if (game_over_menu) {
        game_over_menu->set_visible(true);
    }
    
    Ref<MultiplayerAPI> mp = get_multiplayer();
    if (mp.is_valid() && mp->is_server()) {
        if (restart_button) {
            restart_button->set_text("Restart Game (Host)");
        }
        Array args;
        args.push_back(text);
        rpc("sync_game_over", args);
    }
}

void MainScript::sync_game_over(const String& text) {
    game_over = true;
    if (end_label) {
        end_label->set_text(text);
    }
    if (game_over_menu) {
        game_over_menu->set_visible(true);
    }
}

void MainScript::_on_restart_pressed() {
    Ref<MultiplayerAPI> mp = get_multiplayer();
    if (mp.is_valid() && mp->is_server()) {
        rpc("reload_game_scene", Array());
    }
}

void MainScript::_on_menu_pressed() {
    Ref<MultiplayerAPI> mp = get_multiplayer();
    if (mp.is_valid() && mp->has_multiplayer_peer()) {
        Ref<MultiplayerPeer> peer = mp->get_multiplayer_peer();
        if (peer.is_valid()) {
            peer->close();
        }
    }
    
    SceneTree* tree = get_tree();
    if (tree) {
        tree->change_scene_to_file("res://menu.tscn");
    }
}

void MainScript::reload_game_scene() {
    SceneTree* tree = get_tree();
    if (tree) {
        tree->reload_current_scene();
    }
}

void MainScript::set_survivor_scene(const Ref<PackedScene>& scene) {
    survivor_scene = scene;
}

Ref<PackedScene> MainScript::get_survivor_scene() const {
    return survivor_scene;
}
