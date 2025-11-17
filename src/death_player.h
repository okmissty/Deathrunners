#ifndef DEATH_PLAYER_H
#define DEATH_PLAYER_H

#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/variant/vector2.hpp>

namespace godot {

/**
 * @brief Death player controller
 * 
 * Handles spawning obstacles and enemies to eliminate survivors
 */
class DeathPlayer : public Node2D {
    GDCLASS(DeathPlayer, Node2D)

private:
    float spawn_cooldown;
    float cooldown_timer;
    int spawn_limit;
    int spawned_count;
    
    // Obstacle prefabs
    Ref<PackedScene> spike_scene;
    Ref<PackedScene> pit_scene;
    Ref<PackedScene> enemy_scene;

protected:
    static void _bind_methods();

public:
    DeathPlayer();
    ~DeathPlayer();

    void _ready() override;
    void _process(double delta) override;
    void _input(const Ref<InputEvent> &event) override;

    /**
     * @brief Spawn an obstacle at position
     * @param position World position to spawn
     * @param type Obstacle type (0=spike, 1=pit, 2=enemy)
     */
    void spawn_obstacle(Vector2 position, int type);
    
    /**
     * @brief RPC call to spawn obstacle on all clients
     */
    void rpc_spawn_obstacle(Vector2 position, int type);
    
    bool can_spawn() const;
};

} // namespace godot

#endif // DEATH_PLAYER_H