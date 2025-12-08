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

/**
 * @brief Host a game by creating an ENet server and setting the multiplayer peer.
 *
 * This creates an ENet server on PORT and registers the host as player 1.
 */
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

/**
 * @brief Join an existing server as a client.
 * @param address Server IP to connect to.
 */
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

/**
 * @brief Callback when a new peer connects.
 * @param id Peer id assigned by ENet.
 */
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

/**
 * @brief Callback when a peer disconnects; removes them from the registry.
 */
void GameManager::_on_player_disconnected(int id) {
    UtilityFunctions::print("Player ", id, " disconnected");
    players.erase(id);
}

/**
 * @brief Register a player in the local players dictionary.
 */
void GameManager::register_player(int id, const Dictionary &player_info) {
    players[id] = player_info;
    
    UtilityFunctions::print("Registered player ", id);
}

/**
 * @brief Start the game: mark started and choose Death player.
 *
 * This is a simple implementation which randomly chooses a death player
 * from the registered players. Scene setup/spawning is left as a TODO.
 */
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