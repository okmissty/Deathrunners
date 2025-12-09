#ifndef MENU_H
#define MENU_H

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/line_edit.hpp>
#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/e_net_multiplayer_peer.hpp>
#include <godot_cpp/variant/dictionary.hpp>

namespace godot {

/** Multiplayer menu for hosting/joining games with role assignment. */
class Menu : public Control {
    GDCLASS(Menu, Control)

private:
    static const int PORT = 9999;
    static const int MIN_PLAYERS = 2;
    static const int MAX_PLAYERS = 4;
    
    // UI Node references
    Label* status_label;
    LineEdit* ip_input;
    Button* host_button;
    Button* join_button;
    Button* start_button;
    Label* player_list;
    
    // Network state
    Ref<ENetMultiplayerPeer> peer;
    Dictionary connected_players;
    int death_player_id;

protected:
    static void _bind_methods();

public:
    Menu();
    ~Menu();

    void _ready() override;
    
    // Button callbacks
    void _on_host_pressed();
    void _on_join_pressed();
    void _on_start_pressed();
    
    // Multiplayer callbacks
    void _on_peer_connected(int id);
    void _on_peer_disconnected(int id);
    void _on_connected_to_server();
    void _on_connection_failed();
    
    // Helper methods
    void _assign_death_player();
    void _update_player_list();
    
    // RPC methods
    void request_player_list();
    void update_player_list(const Dictionary& players);
    void finalize_roles(const Dictionary& players, int death_id);
    void load_game();
};

} // namespace godot

#endif // MENU_H
