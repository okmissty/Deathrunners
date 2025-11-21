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
# Hunger drain only while moving
@export var hunger_decrease_rate: float = 2.0    # hunger points per second while moving
@export var hunger_damage_per_second: float = 5.0  # HP per second at 0 hunger

var lives: int
var _lives_label: Label  # NEW
@export var max_lives: int = 3  # Total lives (die 3 times = game over)

var alive: bool = true
var reached_goal: bool = false
var checkpoint_position: Vector2
var is_respawning: bool = false

@onready var sprite = $AnimatedSprite2D
var was_on_floor = true

func _ready() -> void:
	health = max_health
	hunger = max_hunger
	lives = max_lives
	checkpoint_position = global_position  # Start position is first checkpoint

	if sprite:
		sprite.play("idle")
	
	if health_bar_path != NodePath():
		_health_bar = get_node(health_bar_path)
		_health_bar.max_value = max_health
		_health_bar.value = health

	if hunger_bar_path != NodePath():
		_hunger_bar = get_node(hunger_bar_path)
		_hunger_bar.max_value = max_hunger
		_hunger_bar.value = hunger

func _physics_process(delta: float) -> void:
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
	move_and_slide()

	# --- Hunger: only deplete while moving ---
	var is_moving: bool = dir != 0.0
	if is_moving:
		hunger -= hunger_decrease_rate * delta
		if hunger < 0.0:
			hunger = 0.0

	# If starving, take damage over time
	if hunger <= 0.0:
		apply_damage(hunger_damage_per_second * delta)

	_update_ui()

func apply_damage(amount: float) -> void:
	if not alive:
		return
	health -= amount
	if health <= 0.0:
		health = 0.0
		alive = false
		print("Survivor died")
	_update_ui()

func heal(amount: float) -> void:
	if not alive:
		return
	health = min(max_health, health + amount)
	_update_ui()

func restore_hunger(amount: float) -> void:
	hunger = min(max_hunger, hunger + amount)
	_update_ui()

func mark_goal_reached() -> void:
	reached_goal = true
	print("Survivor reached the goal!")

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
