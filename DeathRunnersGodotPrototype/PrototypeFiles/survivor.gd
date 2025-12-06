# survivor_multiplayer.gd - Fixed with proper Godot 4 RPC syntax
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
	
	# Only process input if we have authority
	if multiplayer.has_multiplayer_peer():
		set_physics_process(is_multiplayer_authority())
	
	# Setup animations
	if sprite:
		sprite.play("idle")
	
	# Setup UI bars
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
	# Only process if we have authority
	if multiplayer.has_multiplayer_peer() and not is_multiplayer_authority():
		return
		
	if not alive or reached_goal:
		return

	# Gravity
	if not is_on_floor():
		velocity.y += GRAVITY * delta

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
	
	# Move the character
	move_and_slide()

	# Hunger system - only deplete while moving
	var is_moving: bool = dir != 0.0
	if is_moving:
		hunger -= hunger_decrease_rate * delta
		if hunger < 0.0:
			hunger = 0.0

	# If starving, take damage over time
	if hunger <= 0.0:
		apply_damage(hunger_damage_per_second * delta)

	# Update UI
	_update_ui()

	# Death by falling off the level
	if global_position.y > death_y:
		apply_damage(health)
	
	# Sync position for multiplayer
	if multiplayer.has_multiplayer_peer() and is_multiplayer_authority():
		update_remote_position.rpc(global_position, velocity)

@rpc("unreliable_ordered")
func update_remote_position(pos: Vector2, vel: Vector2):
	if not is_multiplayer_authority():
		global_position = pos
		velocity = vel

func apply_damage(amount: float) -> void:
	if not alive:
		return
	
	# Only apply damage if we have authority
	if multiplayer.has_multiplayer_peer() and not is_multiplayer_authority():
		return
		
	health -= amount
	if health <= 0.0:
		health = 0.0
		alive = false
		print("Survivor died")
		
		# Sync death across network
		if multiplayer.has_multiplayer_peer():
			sync_death.rpc()
			
	_update_ui()

@rpc("call_local", "reliable")
func sync_death():
	alive = false
	if sprite:
		sprite.play("idle")

func heal(amount: float) -> void:
	if not alive:
		return
	health = min(max_health, health + amount)
	_update_ui()

func restore_hunger(amount: float) -> void:
	hunger = min(max_hunger, hunger + amount)
	_update_ui()

func set_checkpoint(pos: Vector2) -> void:
	checkpoint_position = pos
	print("Checkpoint set at: ", checkpoint_position)

func mark_goal_reached() -> void:
	reached_goal = true
	print("Survivor reached the goal!")
	
	# Sync goal reached across network
	if multiplayer.has_multiplayer_peer() and is_multiplayer_authority():
		sync_goal_reached.rpc()

@rpc("call_local", "reliable")
func sync_goal_reached():
	reached_goal = true

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
	
	# Sync animation across network
	if multiplayer.has_multiplayer_peer() and is_multiplayer_authority():
		sync_animation.rpc(sprite.animation, sprite.flip_h)

@rpc("unreliable_ordered")
func sync_animation(anim: String, flip: bool):
	if not is_multiplayer_authority() and sprite:
		sprite.play(anim)
		sprite.flip_h = flip
