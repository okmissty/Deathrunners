extends CharacterBody2D

const SPEED := 200.0
const JUMP_VELOCITY := -400.0
const GRAVITY := 900.0

var health: float
@export var max_health: int = 100
var _health_bar: ProgressBar
@export var health_bar_path: NodePath

var hunger: float
@export var max_hunger: int = 100
var _hunger_bar: ProgressBar
@export var hunger_bar_path: NodePath

@export var hunger_decrease_rate: float = 15.0
@export var hunger_damage_per_second: float = 2.0

var lives: int
@export var max_lives: int = 3

var alive: bool = true
var reached_goal: bool = false
var checkpoint_position: Vector2
var is_respawning: bool = false

@export var death_y: float = 2000.0

@onready var sprite = $AnimatedSprite2D

func _ready() -> void:
	health = max_health
	hunger = max_hunger
	lives = max_lives
	checkpoint_position = global_position
	
	add_to_group("player")
	
	if sprite:
		sprite.play("idle")
	
	await get_tree().process_frame
	_setup_ui_bars()

func _setup_ui_bars():
	if not multiplayer.has_multiplayer_peer() or is_multiplayer_authority():
		var main_scene = get_tree().current_scene
		if main_scene:
			var health_bar = main_scene.get_node_or_null("UI/BarsContainer/HealthRow/HealthBar")
			if health_bar:
				_health_bar = health_bar
				_health_bar.max_value = max_health
				_health_bar.value = health
				
			var hunger_bar = main_scene.get_node_or_null("UI/BarsContainer/HungerRow/HungerBar")
			if hunger_bar:
				_hunger_bar = hunger_bar
				_hunger_bar.max_value = max_hunger
				_hunger_bar.value = hunger

func _physics_process(delta: float) -> void:
	var process_input = true
	if multiplayer.has_multiplayer_peer():
		process_input = is_multiplayer_authority()
		
	if not alive or reached_goal:
		return

	if not is_on_floor():
		velocity.y += GRAVITY * delta

	if process_input:
		var dir := 0.0
		if Input.is_action_pressed("ui_left"):
			dir -= 1.0
		if Input.is_action_pressed("ui_right"):
			dir += 1.0
		velocity.x = dir * SPEED

		if Input.is_action_just_pressed("ui_accept") and is_on_floor():
			velocity.y = JUMP_VELOCITY
			
		_handle_animations(dir)
		
		var is_moving: bool = abs(dir) > 0.1
		if is_moving:
			hunger -= hunger_decrease_rate * delta
			if hunger < 0.0:
				hunger = 0.0

		if hunger <= 0.0:
			apply_damage(hunger_damage_per_second * delta)

		if global_position.y > death_y:
			apply_damage(1000.0)
		
		_update_ui()
		
		if multiplayer.has_multiplayer_peer():
			sync_state.rpc(global_position, velocity, health, hunger, lives, sprite.animation if sprite else "idle", sprite.flip_h if sprite else false)
	
	move_and_slide()
	
	# NEW: Limit movement to camera view
	if process_input:
		_clamp_to_camera_bounds()

func _clamp_to_camera_bounds():
	var cam = get_viewport().get_camera_2d()
	if not cam: return
	
	# Get camera view info
	var view_rect = get_viewport_rect()
	var cam_pos = cam.global_position
	var zoom = cam.zoom
	
	# Calculate world boundaries visible to camera
	var half_width = (view_rect.size.x / zoom.x) * 0.5
	var left_bound = cam_pos.x - half_width
	var right_bound = cam_pos.x + half_width
	
	# Add padding so we don't hug the absolute edge (32 pixels)
	var padding = 32.0
	
	# Clamp player X position
	global_position.x = clamp(global_position.x, left_bound + padding, right_bound - padding)

@rpc("unreliable_ordered")
func sync_state(pos: Vector2, vel: Vector2, h: float, hu: float, l: int, anim: String, flip: bool):
	if not is_multiplayer_authority():
		global_position = pos
		velocity = vel
		health = h
		hunger = hu
		lives = l
		if sprite:
			sprite.play(anim)
			sprite.flip_h = flip

@rpc("any_peer", "call_local", "reliable")
func apply_damage(amount: float) -> void:
	if not is_multiplayer_authority():
		apply_damage.rpc_id(get_multiplayer_authority(), amount)
		return

	if not alive:
		return
	
	health -= amount
	if health <= 0.0:
		health = 0.0
		_handle_death_or_respawn()
			
	_update_ui()

func _handle_death_or_respawn():
	lives -= 1
	print(name, " lost a life. Lives remaining: ", lives)
	
	if lives > 0:
		respawn()
	else:
		alive = false
		print(name, " is permanently dead.")
		if multiplayer.has_multiplayer_peer():
			sync_death.rpc()
		if sprite:
			sprite.play("idle")
			sprite.modulate = Color(0.5, 0.5, 0.5, 0.5)

func respawn():
	print("Respawning at: ", checkpoint_position)
	global_position = checkpoint_position
	velocity = Vector2.ZERO
	health = max_health
	hunger = max_hunger
	if multiplayer.has_multiplayer_peer():
		sync_respawn.rpc(checkpoint_position)

@rpc("call_local", "reliable")
func sync_respawn(pos: Vector2):
	global_position = pos
	velocity = Vector2.ZERO

@rpc("call_local", "reliable")
func sync_death():
	alive = false
	health = 0
	lives = 0
	if sprite:
		sprite.play("idle")
		sprite.modulate = Color(0.5, 0.5, 0.5, 0.5)
	_update_ui()

func heal(amount: float) -> void:
	if not alive: return
	health = min(max_health, health + amount)
	_update_ui()
	if multiplayer.has_multiplayer_peer() and is_multiplayer_authority(): sync_health.rpc(health)

@rpc("call_local", "reliable")
func sync_health(val): health = val; _update_ui()

func restore_hunger(amount: float) -> void:
	hunger = min(max_hunger, hunger + amount)
	_update_ui()
	if multiplayer.has_multiplayer_peer() and is_multiplayer_authority(): sync_hunger.rpc(hunger)

@rpc("call_local", "reliable")
func sync_hunger(val): hunger = val; _update_ui()

func set_checkpoint(pos: Vector2) -> void:
	checkpoint_position = pos
	print(name, " checkpoint set to: ", pos)

# IMPROVED GOAL DETECTION
func mark_goal_reached() -> void:
	print("=== ", name, " REACHED THE GOAL! ===")
	reached_goal = true
	
	# Make sure the value is set
	print("  reached_goal is now: ", reached_goal)
	
	# Sync across network
	if multiplayer.has_multiplayer_peer():
		print("  Syncing goal reached across network...")
		sync_goal_reached.rpc()
		
		# If we're the authority, also notify the main scene directly
		if is_multiplayer_authority():
			var main = get_tree().current_scene
			if main:
				print("  Notifying main scene directly...")
				# Force the main scene to check win conditions
				if main.has_method("_show_game_over"):
					main.call_deferred("_show_game_over", "Survivors win!")

@rpc("call_local", "reliable")
func sync_goal_reached():
	reached_goal = true
	print(name, " goal reached synced! reached_goal = ", reached_goal)
	
	# Force main scene to check win condition on all clients
	var main = get_tree().current_scene
	if main:
		# Trigger a check in the next frame
		if main.has_method("_process"):
			main.set("game_over", false)  # Allow win check
			main.call_deferred("_process", 0.01)

func _update_ui() -> void:
	if not multiplayer.has_multiplayer_peer() or is_multiplayer_authority():
		if _health_bar: _health_bar.value = health
		if _hunger_bar: _hunger_bar.value = hunger

func _handle_animations(direction: float) -> void:
	if not sprite: return
	if direction > 0: sprite.flip_h = false
	elif direction < 0: sprite.flip_h = true
	if not is_on_floor():
		if velocity.y < 0: sprite.play("jump")
		else: sprite.play("fall")
	else:
		if abs(velocity.x) > 0.1: sprite.play("run")
		else: sprite.play("idle")
