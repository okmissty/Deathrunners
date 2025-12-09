#include "menu.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/multiplayer_api.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/timer.hpp>

using namespace godot;

void Menu::_bind_methods() {
    // Button callbacks
    ClassDB::bind_method(D_METHOD("_on_host_pressed"), &Menu::_on_host_pressed);
    ClassDB::bind_method(D_METHOD("_on_join_pressed"), &Menu::_on_join_pressed);
    ClassDB::bind_method(D_METHOD("_on_start_pressed"), &Menu::_on_start_pressed);
    
    // Multiplayer callbacks
    ClassDB::bind_method(D_METHOD("_on_peer_connected", "id"), &Menu::_on_peer_connected);
    ClassDB::bind_method(D_METHOD("_on_peer_disconnected", "id"), &Menu::_on_peer_disconnected);
    ClassDB::bind_method(D_METHOD("_on_connected_to_server"), &Menu::_on_connected_to_server);
    ClassDB::bind_method(D_METHOD("_on_connection_failed"), &Menu::_on_connection_failed);
    
    // RPC methods
    ClassDB::bind_method(D_METHOD("request_player_list"), &Menu::request_player_list);
    ClassDB::bind_method(D_METHOD("update_player_list", "players"), &Menu::update_player_list);
    ClassDB::bind_method(D_METHOD("finalize_roles", "players", "death_id"), &Menu::finalize_roles);
    ClassDB::bind_method(D_METHOD("load_game"), &Menu::load_game);
}

Menu::Menu() {
    death_player_id = -1;
    status_label = nullptr;
    ip_input = nullptr;
    host_button = nullptr;
    join_button = nullptr;
    start_button = nullptr;
    player_list = nullptr;
}

Menu::~Menu() {}

void Menu::_ready() {
    // Get UI node references
    status_label = get_node<Label>("MenuContainer/StatusLabel");
    ip_input = get_node<LineEdit>("MenuContainer/IPInput");
    host_button = get_node<Button>("MenuContainer/ButtonContainer/HostButton");
    join_button = get_node<Button>("MenuContainer/ButtonContainer/JoinButton");
    start_button = get_node<Button>("MenuContainer/StartButton");
    player_list = get_node<Label>("MenuContainer/PlayerList");
    
    // Connect button signals
    if (host_button) {
        host_button->connect("pressed", Callable(this, "_on_host_pressed"));
    }
    if (join_button) {
        join_button->connect("pressed", Callable(this, "_on_join_pressed"));
    }
    if (start_button) {
        start_button->connect("pressed", Callable(this, "_on_start_pressed"));
        start_button->set_visible(false);
    }
    
    // Connect multiplayer signals
    MultiplayerAPI* mp = get_multiplayer();
    if (mp) {
        mp->connect("peer_connected", Callable(this, "_on_peer_connected"));
        mp->connect("peer_disconnected", Callable(this, "_on_peer_disconnected"));
        mp->connect("connected_to_server", Callable(this, "_on_connected_to_server"));
        mp->connect("connection_failed", Callable(this, "_on_connection_failed"));
    }
    
    peer.instantiate();
}

void Menu::_on_host_pressed() {
    peer->create_server(PORT, MAX_PLAYERS);
    
    MultiplayerAPI* mp = get_multiplayer();
    if (mp) {
        mp->set_multiplayer_peer(peer);
    }
    
    // Host is always ID 1, initially a survivor
    connected_players[1] = "Survivor";
    
    if (status_label) {
        status_label->set_text("Hosting... Waiting for players (1/" + String::num_int64(MAX_PLAYERS) + ")");
        status_label->set_modulate(Color(0, 1, 0));
    }
    
    if (host_button) host_button->set_disabled(true);
    if (join_button) join_button->set_disabled(true);
    if (start_button) {
        start_button->set_visible(true);
        start_button->set_disabled(true); // Disabled until minimum players
    }
    
    _update_player_list();
}

void Menu::_on_join_pressed() {
    String ip = "127.0.0.1";
    if (ip_input) {
        String input_text = ip_input->get_text();
        if (!input_text.is_empty()) {
            ip = input_text;
        }
    }
    
    peer->create_client(ip, PORT);
    
    MultiplayerAPI* mp = get_multiplayer();
    if (mp) {
        mp->set_multiplayer_peer(peer);
    }
    
    if (status_label) {
        status_label->set_text("Connecting to " + ip + "...");
        status_label->set_modulate(Color(1, 1, 0));
    }
    
    if (host_button) host_button->set_disabled(true);
    if (join_button) join_button->set_disabled(true);
}

void Menu::_on_connected_to_server() {
    if (status_label) {
        status_label->set_text("Connected! Waiting for host to start");
        status_label->set_modulate(Color(0, 1, 0));
    }
    
    // Request current player list from host
    Array args;
    rpc_id(1, "request_player_list", args);
}

void Menu::_on_peer_connected(int id) {
    UtilityFunctions::print("Player connected: ", id);
    
    MultiplayerAPI* mp = get_multiplayer();
    if (!mp || !mp->is_server()) return;
    
    // Assign new player as survivor initially
    connected_players[id] = "Survivor";
    
    int player_count = connected_players.size();
    if (status_label) {
        status_label->set_text("Players connected: " + String::num_int64(player_count) + "/" + String::num_int64(MAX_PLAYERS));
    }
    
    // Enable start button if we have minimum players
    if (player_count >= MIN_PLAYERS) {
        if (start_button) {
            start_button->set_disabled(false);
        }
        if (status_label) {
            status_label->set_modulate(Color(0, 1, 0));
        }
    }
    
    // Randomly assign one player as Death when we have enough players
    if (player_count >= MIN_PLAYERS && death_player_id == -1) {
        _assign_death_player();
    }
    
    // Sync player list to all clients
    Array args;
    args.push_back(connected_players);
    rpc("update_player_list", args);
    _update_player_list();
}

void Menu::_on_peer_disconnected(int id) {
    UtilityFunctions::print("Player disconnected: ", id);
    
    MultiplayerAPI* mp = get_multiplayer();
    if (!mp || !mp->is_server()) return;
    
    connected_players.erase(id);
    
    // If Death player left, reassign
    if (id == death_player_id && connected_players.size() >= MIN_PLAYERS) {
        _assign_death_player();
    } else if (id == death_player_id) {
        death_player_id = -1;
    }
    
    int player_count = connected_players.size();
    if (status_label) {
        status_label->set_text("Player disconnected. Players: " + String::num_int64(player_count) + "/" + String::num_int64(MAX_PLAYERS));
    }
    
    // Disable start if below minimum
    if (player_count < MIN_PLAYERS) {
        if (start_button) {
            start_button->set_disabled(true);
        }
        if (status_label) {
            status_label->set_modulate(Color(1, 1, 0));
        }
    }
    
    // Sync to all clients
    Array args;
    args.push_back(connected_players);
    rpc("update_player_list", args);
    _update_player_list();
}

void Menu::_on_connection_failed() {
    if (status_label) {
        status_label->set_text("Connection failed!");
        status_label->set_modulate(Color(1, 0, 0));
    }
    
    if (host_button) host_button->set_disabled(false);
    if (join_button) join_button->set_disabled(false);
}

void Menu::_assign_death_player() {
    Array player_ids = connected_players.keys();
    if (player_ids.size() == 0) return;
    
    // Randomly select one player to be Death
    int random_index = Math::randi() % player_ids.size();
    death_player_id = player_ids[random_index];
    
    // Update roles
    for (int i = 0; i < player_ids.size(); i++) {
        int id = player_ids[i];
        if (id == death_player_id) {
            connected_players[id] = "Death";
        } else {
            connected_players[id] = "Survivor";
        }
    }
    
    UtilityFunctions::print("Death player assigned: ", death_player_id);
}

void Menu::_on_start_pressed() {
    MultiplayerAPI* mp = get_multiplayer();
    if (!mp || !mp->is_server()) return;
    
    // Ensure we have a Death player
    if (death_player_id == -1) {
        _assign_death_player();
    }
    
    // Send final role assignments
    Array args1;
    args1.push_back(connected_players);
    args1.push_back(death_player_id);
    rpc("finalize_roles", args1);
    
    // Start the game
    Array args2;
    rpc("load_game", args2);
}

void Menu::request_player_list() {
    MultiplayerAPI* mp = get_multiplayer();
    if (!mp || !mp->is_server()) return;
    
    int sender_id = mp->get_remote_sender_id();
    Array args;
    args.push_back(connected_players);
    rpc_id(sender_id, "update_player_list", args);
}

void Menu::update_player_list(const Dictionary& players) {
    connected_players = players;
    _update_player_list();
}

void Menu::finalize_roles(const Dictionary& players, int death_id) {
    connected_players = players;
    death_player_id = death_id;
    
    // Store in metadata on the scene tree
    SceneTree* tree = get_tree();
    if (tree) {
        tree->set_meta("player_roles", connected_players);
        tree->set_meta("death_player_id", death_player_id);
    }
    
    MultiplayerAPI* mp = get_multiplayer();
    if (mp) {
        int my_id = mp->get_unique_id();
        String my_role = connected_players.has(my_id) ? String(connected_players[my_id]) : "Unknown";
        UtilityFunctions::print("My ID: ", my_id);
        UtilityFunctions::print("My Role: ", my_role);
    }
}

void Menu::load_game() {
    SceneTree* tree = get_tree();
    if (tree) {
        // Wait a bit to ensure RPC sends
        call_deferred("change_scene_to_file", "res://main.tscn");
    }
}

void Menu::_update_player_list() {
    if (!player_list) return;
    
    String text = "Players:\n";
    Array player_ids = connected_players.keys();
    
    MultiplayerAPI* mp = get_multiplayer();
    int my_id = mp ? mp->get_unique_id() : -1;
    
    for (int i = 0; i < player_ids.size(); i++) {
        int id = player_ids[i];
        String role = connected_players[id];
        String display_name = "Player " + String::num_int64(id);
        
        if (id == 1) {
            display_name += " (Host)";
        }
        if (id == my_id) {
            display_name += " (You)";
        }
        
        text += display_name + " - " + role + "\n";
    }
    
    player_list->set_text(text);
}
