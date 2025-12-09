#ifndef DEATH_CONTROLLER_H
#define DEATH_CONTROLLER_H

#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/multiplayer_api.hpp>
#include <godot_cpp/classes/multiplayer_peer.hpp>
#include <godot_cpp/classes/tween.hpp>
#include <godot_cpp/classes/property_tweener.hpp>

namespace godot {

class DeathController : public Node2D {
    GDCLASS(DeathController, Node2D)

private:
    Ref<PackedScene> aoe_scene;
    Ref<PackedScene> falling_block_scene;
    
    // State
    Array preplaced_traps;
    int selected_trap_index;
    
    Array players;
    int selected_player_index;
    
    // UI & Visuals
    Label* trap_label;
    Label* target_label;
    Node2D* trap_indicator;
    Node2D* player_indicator;
    Vector2 trap_indicator_base_scale;
    Vector2 player_indicator_base_scale;
    
    bool enabled;

protected:
    static void _bind_methods();

public:
    DeathController();
    ~DeathController();

    void _ready() override;
    void _process(double delta) override;

    // Logic Methods
    void _refresh_players();
    void _refresh_preplaced_traps();
    Node2D* _current_player();
    Node2D* _current_trap();
    
    // Input Handlers
    void _select_prev_trap();
    void _select_next_trap();
    void _activate_selected_trap();
    void _select_prev_player();
    void _select_next_player();
    
    // Abilities
    void _spawn_aoe_on_selected_player();
    void _spawn_falling_on_selected_player();
    void _spawn_aoe_at_position(Vector2 pos);
    void _spawn_falling_at_position(Vector2 pos);

    // RPCs
    void activate_trap_networked(NodePath trap_path);
    void spawn_aoe_networked(Vector2 pos);
    void spawn_falling_networked(Vector2 pos);

    // Helper
    void _update_trap_highlight();
    void _update_player_highlight();
    void _update_player_indicator_follow();
    void _update_trap_hud(Node* trap);
    void _update_player_hud(Node* player);
    void _pulse_indicator(Node2D* ind, Vector2 base_scale);
    
    // Sorting helpers
    bool _sort_traps_by_x(const Node* a, const Node* b);
    bool _sort_traps_method_wrapper(Node* a, Node* b);

    // Getters/Setters
    void set_aoe_scene(const Ref<PackedScene>& p_scene) { aoe_scene = p_scene; }
    Ref<PackedScene> get_aoe_scene() const { return aoe_scene; }
    void set_falling_block_scene(const Ref<PackedScene>& p_scene) { falling_block_scene = p_scene; }
    Ref<PackedScene> get_falling_block_scene() const { return falling_block_scene; }
};

} // namespace godot

#endif