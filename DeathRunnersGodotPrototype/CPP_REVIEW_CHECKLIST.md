# C++ Implementation Review Checklist

This document tracks which C++ files need their logic verified against the original GDScript implementations.

## Status Legend
- ✅ **Fully Ported** - All logic from GDScript implemented in C++
- ⚠️ **Needs Review** - File exists but needs comparison with GDScript
- ❌ **Missing Logic** - Stub/skeleton only, needs implementation
- 🔧 **In Progress** - Currently being worked on

---

## Core Game Logic

### ✅ MainScript (`src/main_script.cpp/.h`)
**GDScript:** `main.gd`
**Status:** Fully ported
**Implements:**
- Multiplayer spawning
- Player role management
- Win condition checking
- Game over system
- UI management
- RPC communication

### ✅ Menu (`src/menu.cpp/.h`)
**GDScript:** `menu.gd`
**Status:** Fully ported
**Implements:**
- Host/Join lobby
- Player list
- Death role assignment
- Scene transitions
- Network peer management

---

## Player & Controller

### ⚠️ Player (`src/player.cpp/.h`)
**GDScript:** `PrototypeFiles/survivor.gd`
**Status:** Partially implemented
**Review Needed:**
- [ ] Compare movement logic (lines 1-100 in survivor.gd)
- [ ] Check hunger system implementation
- [ ] Verify health/damage system
- [ ] Check lives/respawn logic
- [ ] Verify checkpoint system
- [ ] Check goal reached detection
- [ ] Verify multiplayer authority
- [ ] Check UI bar updates
- [ ] Verify animation states

**Key GDScript Functions to Port:**
```gdscript
func _setup_ui_bars()
func take_damage(amount: float)
func heal(amount: float)
func eat(amount: float)
func die()
func respawn()
func _on_checkpoint_reached(checkpoint_pos: Vector2)
func _on_goal_reached()
```

### ⚠️ DeathController (`src/death_controller.cpp/.h`)
**GDScript:** `PrototypeFiles/death_controller.gd`
**Review Needed:**
- [ ] Trap spawning system
- [ ] Cooldown management
- [ ] Input handling for trap placement
- [ ] UI updates (trap icons, cooldowns)
- [ ] Resource management
- [ ] Multiplayer synchronization

---

## Game Objects

### ⚠️ Checkpoint (`src/checkpoint.cpp/.h`)
**GDScript:** `PrototypeFiles/checkpoint.gd`
**Review Needed:**
- [ ] Area2D body_entered signal
- [ ] Checkpoint registration with player
- [ ] Visual feedback (animation?)
- [ ] Sound effects
- [ ] Multiplayer sync

**Key Logic:**
```gdscript
func _on_body_entered(body):
    if body.is_in_group("player"):
        body._on_checkpoint_reached(global_position)
```

### ⚠️ Goal (`src/goal.cpp/.h`)
**GDScript:** `PrototypeFiles/goal.gd`
**Review Needed:**
- [ ] Win condition trigger
- [ ] Player detection
- [ ] Goal reached callback
- [ ] Visual effects
- [ ] Multiplayer sync

### ⚠️ Hazard (`src/hazard.cpp/.h`)
**GDScript:** `PrototypeFiles/hazard.gd`
**Review Needed:**
- [ ] Damage on contact
- [ ] Damage amount property
- [ ] Area2D collision detection
- [ ] Damage cooldown (to prevent instant death)

---

## Traps

### ⚠️ AoeTrap (`src/aoe_trap.cpp/.h`)
**GDScript:** `PrototypeFiles/aoe_trap.gd`
**Review Needed:**
- [ ] Area of effect damage
- [ ] Damage over time
- [ ] Visual effects (fire, particles)
- [ ] Lifetime/duration
- [ ] Fade out animation
- [ ] Multiplayer spawn sync

### ⚠️ ArrowTrap (`src/arrow_trap.cpp/.h`)
**GDScript:** `PrototypeFiles/arrow_trap.gd`
**Review Needed:**
- [ ] Periodic arrow firing
- [ ] Arrow projectile spawning
- [ ] Fire direction
- [ ] Cooldown timer
- [ ] Muzzle position
- [ ] Multiplayer sync

### ⚠️ ArrowProjectile (`src/arrow_projectile.cpp/.h`)
**GDScript:** `PrototypeFiles/arrow_projectile.gd`
**Review Needed:**
- [ ] Movement/velocity
- [ ] Damage on hit
- [ ] Lifetime/despawn
- [ ] Collision detection
- [ ] Visual rotation

### ⚠️ BoulderTrap (`src/boulder_trap.cpp/.h`)
**GDScript:** `PrototypeFiles/boulder_trap.gd`
**Review Needed:**
- [ ] Falling physics
- [ ] Damage calculation
- [ ] Collision detection
- [ ] Despawn on ground/timeout
- [ ] Multiplayer sync

---

## Pickups

### ⚠️ HealthPickup (`src/health_pickup.cpp/.h`)
**GDScript:** `health_pickup.gd`
**Review Needed:**
- [ ] Health restore amount
- [ ] Collision detection
- [ ] Pickup effect
- [ ] Despawn after use
- [ ] Visual/audio feedback

**Key Logic:**
```gdscript
func _on_body_entered(body):
    if body.has_method("heal"):
        body.heal(health_amount)
        queue_free()
```

### ⚠️ HungerPickup (`src/hunger_pickup.cpp/.h`)
**GDScript:** `hunger_pickup.gd`
**Review Needed:**
- [ ] Hunger restore amount
- [ ] Collision detection
- [ ] Pickup effect
- [ ] Despawn after use

### ⚠️ HHPickup (`src/hhpickup.cpp/.h`)
**GDScript:** `PrototypeFiles/HHpickup.gd`
**Review Needed:**
- [ ] Combined health AND hunger restore
- [ ] Collision detection
- [ ] Pickup effect
- [ ] Despawn after use

---

## Utilities

### ⚠️ HorizontalCam (`src/horizontal_cam.cpp/.h`)
**GDScript:** `PrototypeFiles/horizontal_cam.gd`
**Review Needed:**
- [ ] Camera following logic
- [ ] Smooth movement
- [ ] Boundary constraints
- [ ] Zoom control
- [ ] Target following (player)

---

## How to Review

For each file marked ⚠️:

1. **Open both files side by side:**
   - GDScript: `PrototypeFiles/<name>.gd` or `<name>.gd`
   - C++: `src/<name>.cpp` and `src/<name>.h`

2. **Check the GDScript for:**
   - `func _ready()` → Should be in `_ready()` C++ method
   - `func _process(delta)` → Should be in `_process(double delta)` C++
   - `func _physics_process(delta)` → Should be in `_physics_process(double delta)` C++
   - Signal connections → Should be in C++ with `connect()` or `_bind_methods()`
   - Exported variables → Should be properties with `ADD_PROPERTY` in C++
   - Custom functions → Should be C++ methods

3. **Port missing logic:**
   - Add any missing member variables to the `.h` file
   - Implement missing methods in the `.cpp` file
   - Ensure `_bind_methods()` exposes necessary methods/properties

4. **Test:**
   - Rebuild the extension
   - Test in Godot
   - Verify behavior matches original

---

## Priority Order

**High Priority (Core Gameplay):**
1. Player (survivor.gd → player.cpp) - CRITICAL
2. DeathController - CRITICAL  
3. Goal - Win condition
4. Hazard - Main danger

**Medium Priority (Traps):**
5. AoeTrap
6. ArrowTrap + ArrowProjectile
7. BoulderTrap

**Lower Priority (Polish):**
8. Pickups (Health, Hunger, HH)
9. Checkpoint
10. HorizontalCam

---

## Example: How to Port a Simple Function

### GDScript:
```gdscript
@export var damage: float = 10.0

func _on_body_entered(body):
    if body.has_method("take_damage"):
        body.take_damage(damage)
        queue_free()
```

### C++ Header:
```cpp
class Hazard : public Area2D {
    GDCLASS(Hazard, Area2D)
private:
    float damage;
public:
    void _ready() override;
    void _on_body_entered(Node* body);
    
    void set_damage(float d);
    float get_damage() const;
};
```

### C++ Implementation:
```cpp
void Hazard::_bind_methods() {
    ClassDB::bind_method(D_METHOD("_on_body_entered", "body"), &Hazard::_on_body_entered);
    ClassDB::bind_method(D_METHOD("set_damage", "d"), &Hazard::set_damage);
    ClassDB::bind_method(D_METHOD("get_damage"), &Hazard::get_damage);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "damage"), "set_damage", "get_damage");
}

void Hazard::_ready() {
    connect("body_entered", Callable(this, "_on_body_entered"));
}

void Hazard::_on_body_entered(Node* body) {
    if (body && body->has_method("take_damage")) {
        body->call("take_damage", damage);
        queue_free();
    }
}

void Hazard::set_damage(float d) { damage = d; }
float Hazard::get_damage() const { return damage; }
```

---

## Completion Tracking

- [ ] Player logic fully ported
- [ ] All traps fully ported  
- [ ] All pickups fully ported
- [ ] Camera system working
- [ ] All classes tested individually
- [ ] Multiplayer tested end-to-end
- [ ] Game fully playable in C++

Once all checkboxes are complete, the refactoring is done!
