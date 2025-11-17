#include "game_manager.h"
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void GameManager::_bind_methods() {
    ClassDB::bind_method(D_METHOD("host_game"), &GameManager::host_game);
    ClassDB::bind_method(D_METHOD("join_game", "address"), &GameManager::join_game);
    ClassDB::bind_method(D_METHOD("start_game"), &GameManager::start_game);
    
    ClassDB::bind_method(D_METHOD("_on_player_connected", "id"), 
                        &GameManager::_on_player_connected);
    ClassDB::bind_method(D_METHOD("_on_player_disconnected", "id"), 
                        &GameManager::_on_player_disconnected);
    
    ClassDB::bind_method(D_METHOD("register_player", "id", "player_info"), 
                        &GameManager::register_player);
}

GameManager::GameManager() {
    death_player_id = -1;
    game_started = false;
}

GameManager::~GameManager() {
}

void GameManager::_ready() {
    // Connect multiplayer signals
    MultiplayerAPI *multiplayer = get_multiplayer();
    if (multiplayer) {
        multiplayer->connect("peer_connected", 
                           Callable(this, "_on_player_connected"));
        multiplayer->connect("peer_disconnected", 
                           Callable(this, "_on_player_disconnected"));
    }
}

void GameManager::host_game() {
    peer = Ref<ENetMultiplayerPeer>(memnew(ENetMultiplayerPeer));
    Error error = peer->create_server(PORT, MAX_PLAYERS);
    
    if (error != OK) {
        UtilityFunctions::print("Failed to create server: ", error);
        return;
    }
    
    get_tree()->get_multiplayer()->set_multiplayer_peer(peer);
    UtilityFunctions::print("Server started on port ", PORT);
    
    // Host is player 1
    register_player(1, Dictionary());
}

void GameManager::join_game(const String &address) {
    peer = Ref<ENetMultiplayerPeer>(memnew(ENetMultiplayerPeer));
    Error error = peer->create_client(address, PORT);
    
    if (error != OK) {
        UtilityFunctions::print("Failed to connect to server: ", error);
        return;
    }
    
    get_tree()->get_multiplayer()->set_multiplayer_peer(peer);
    UtilityFunctions::print("Connecting to ", address, ":", PORT);
}

void GameManager::_on_player_connected(int id) {
    UtilityFunctions::print("Player ", id, " connected");
    
    // Register the new player
    Dictionary player_info;
    player_info["id"] = id;
    player_info["name"] = "Player " + String::num(id);
    
    register_player(id, player_info);
    
    // If we have enough players, can start game
    if (players.size() >= 2 && !game_started) {
        // Auto-start or wait for host command
    }
}

void GameManager::_on_player_disconnected(int id) {
    UtilityFunctions::print("Player ", id, " disconnected");
    players.erase(id);
}

void GameManager::register_player(int id, const Dictionary &player_info) {
    players[id] = player_info;
    
    UtilityFunctions::print("Registered player ", id);
}

void GameManager::start_game() {
    if (game_started) {
        return;
    }
    
    game_started = true;
    
    // Randomly select Death player
    Array player_ids = players.keys();
    int random_index = UtilityFunctions::randi() % player_ids.size();
    death_player_id = player_ids[random_index];
    
    UtilityFunctions::print("Game started! Player ", death_player_id, " is Death");
    
    // TODO: Spawn players, set up game scene
}