#ifndef MAIN_SCRIPT_H
#define MAIN_SCRIPT_H

#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/multiplayer_spawner.hpp>
#include <godot_cpp/classes/multiplayer_api.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/classes/scene_tree.hpp>

namespace godot {

class DeathController;
class HorizontalCam;

/** Main game script handling multiplayer game state and player management. */
class MainScript : public Node2D {
    GDCLASS(MainScript, Node2D)

private:
    // Scene references
    Ref<PackedScene> survivor_scene;
    
    // Node references (assigned via get_node in _ready)
    DeathController* death_controller;
    Label* end_label;
    Label* role_label;
    Control* death_hud;
    HorizontalCam* horizontal_cam;
    MultiplayerSpawner* multiplayer_spawner;
    Control* game_over_menu;
    Button* restart_button;
    Button* menu_button;
    
    // Game state
    bool game_over;
    Dictionary survivors;
    Dictionary player_roles;
    int death_player_id;
    String my_role;
    Dictionary players_ready;

protected:
    static void _bind_methods();

public:
    MainScript();
    ~MainScript();

    void _ready() override;
    void _process(double delta) override;
    
    // Multiplayer setup
    void setup_multiplayer();
    Node* _spawn_survivor(const Dictionary& data);
    void spawn_survivor_for_player(int player_id, int x_offset);
    void spawn_all_survivors();
    
    // Network callbacks
    void _on_peer_connected(int id);
    void _on_peer_disconnected(int id);
    
    // Game state management
    void _show_game_over(const String& text);
    void _on_restart_pressed();
    void _on_menu_pressed();
    
    // RPC methods
    void player_ready(int player_id);
    void sync_game_over(const String& text);
    void reload_game_scene();
    
    // Property getters/setters
    void set_survivor_scene(const Ref<PackedScene>& scene);
    Ref<PackedScene> get_survivor_scene() const;
};

} // namespace godot

#endif // MAIN_SCRIPT_H
