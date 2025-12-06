# survivor_multiplayer.gd - Simplified without MultiplayerSynchronizer conflicts
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

@export var hunger_decrease_rate: float = 0.5
@export var hunger_damage_per_second: float = 2.0

var lives: int
@export var max_lives: int = 3

var alive: bool = true
var reached_goal: bool = false
var checkpoint_position: Vector2
var is_respawning: bool = false

@export var death_y: float = 2000.0

@onready var sprite = $AnimatedSprite2D
var was_on_floor = true

func _ready() -> void:
	# Initialize values
	health = max_health
	hunger = max_hunger
	lives = max_lives
	checkpoint_position = global_position
	
	# Add to player group
	add_to_group("player")
	
	# Setup animations
	if sprite:
		sprite.play("idle")
	
	# Setup UI bars - wait a frame for scene to be ready
	await get_tree().process_frame
	
	if health_bar_path != NodePath(""):
		_health_bar = get_node_or_null(health_bar_path)
		if _health_bar:
			_health_bar.max_value = max_health
			_health_bar.value = health

	if hunger_bar_path != NodePath(""):
		_hunger_bar = get_node_or_null(hunger_bar_path)
		if _hunger_bar:
			_hunger_bar.max_value = max_hunger
			_hunger_bar.value = hunger

func _physics_process(delta: float) -> void:
	# Only process input if we have authority
	var process_input = true
	if multiplayer.has_multiplayer_peer():
		process_input = is_multiplayer_authority()
		
	if not alive or reached_goal:
		return

	# Gravity always applies
	if not is_on_floor():
		velocity.y += GRAVITY * delta

	# Input only if we have authority
	if process_input:
		# Horizontal movement
		var dir := 0.0
		if Input.is_action_pressed("ui_left"):
			dir -= 1.0
		if Input.is_action_pressed("ui_right"):
			dir += 1.0
		velocity.x = dir * SPEED

		# Jump
		if Input.is_action_just_pressed("ui_accept") and is_on_floor():
			velocity.y = JUMP_VELOCITY
			
		# Handle animations
		_handle_animations(dir)
		
		# Hunger system - only deplete while moving
		var is_moving: bool = dir != 0.0
		if is_moving:
			hunger -= hunger_decrease_rate * delta
			if hunger < 0.0:
				hunger = 0.0

		# If starving, take damage over time
		if hunger <= 0.0:
			apply_damage(hunger_damage_per_second * delta)

		# Death by falling off the level
		if global_position.y > death_y:
			apply_damage(health)
		
		# Sync to other players
		if multiplayer.has_multiplayer_peer():
			update_remote_state.rpc(global_position, velocity, sprite.animation if sprite else "idle", sprite.flip_h if sprite else false)
	
	# Always move the character
	move_and_slide()
	
	# Update UI
	_update_ui()

@rpc("unreliable_ordered")
func update_remote_state(pos: Vector2, vel: Vector2, anim: String, flip: bool):
	if not is_multiplayer_authority():
		global_position = pos
		velocity = vel
		if sprite:
			sprite.play(anim)
			sprite.flip_h = flip

func apply_damage(amount: float) -> void:
	if not alive:
		return
	
	health -= amount
	if health <= 0.0:
		health = 0.0
		alive = false
		print(name, " died! Health: ", health)
		
		# Sync death across network
		if multiplayer.has_multiplayer_peer():
			sync_death.rpc()
		
		# Play death animation
		if sprite:
			sprite.play("idle")  # Or "death" if you have one
			
	_update_ui()

@rpc("call_local", "reliable")
func sync_death():
	alive = false
	health = 0
	if sprite:
		sprite.play("idle")
	_update_ui()

func heal(amount: float) -> void:
	if not alive:
		return
	health = min(max_health, health + amount)
	_update_ui()
	print(name, " healed for ", amount, ". Health: ", health)

func restore_hunger(amount: float) -> void:
	hunger = min(max_hunger, hunger + amount)
	_update_ui()
	print(name, " restored hunger for ", amount, ". Hunger: ", hunger)

func set_checkpoint(pos: Vector2) -> void:
	checkpoint_position = pos
	print(name, " checkpoint set at: ", checkpoint_position)

func mark_goal_reached() -> void:
	reached_goal = true
	print(name, " reached the goal!")
	
	# Sync goal reached across network
	if multiplayer.has_multiplayer_peer():
		sync_goal_reached.rpc()

@rpc("call_local", "reliable")
func sync_goal_reached():
	reached_goal = true
	print(name, " goal reached synced!")

func _update_ui() -> void:
	if _health_bar:
		_health_bar.value = health
	if _hunger_bar:
		_hunger_bar.value = hunger

func _handle_animations(direction: float) -> void:
	if not sprite:
		return
	
	# Flip sprite based on direction
	if direction > 0:
		sprite.flip_h = false
	elif direction < 0:
		sprite.flip_h = true
	
	# Determine which animation to play
	if not is_on_floor():
		if velocity.y < 0:
			sprite.play("jump")
		else:
			sprite.play("fall")
	else:
		if abs(velocity.x) > 0.1:
			sprite.play("run")
		else:
			sprite.play("idle")
