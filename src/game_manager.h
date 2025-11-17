#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/multiplayer_api.hpp>
#include <godot_cpp/classes/enet_multiplayer_peer.hpp>
#include <godot_cpp/variant/dictionary.hpp>

namespace godot {

/**
 * @brief Main game manager handling multiplayer logic
 * 
 * Manages player connections, role assignment, and game state
 */
class GameManager : public Node {
    GDCLASS(GameManager, Node)

private:
    const int PORT = 7777;
    const int MAX_PLAYERS = 4;
    
    Dictionary players; // player_id -> player_data
    int death_player_id;
    bool game_started;
    
    Ref<ENetMultiplayerPeer> peer;

protected:
    static void _bind_methods();

public:
    GameManager();
    ~GameManager();

    void _ready() override;
    
    /**
     * @brief Start game as server/host
     */
    void host_game();
    
    /**
     * @brief Join game as client
     * @param address Server IP address
     */
    void join_game(const String &address);
    
    /**
     * @brief Handle new player connection
     */
    void _on_player_connected(int id);
    
    /**
     * @brief Handle player disconnection
     */
    void _on_player_disconnected(int id);
    
    /**
     * @brief Start the game (assign roles, spawn players)
     */
    void start_game();
    
    /**
     * @brief Register a player
     */
    void register_player(int id, const Dictionary &player_info);
};

} // namespace godot

#endif // GAME_MANAGER_H