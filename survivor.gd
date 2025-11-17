extends CharacterBody2D

const SPEED = 200.0
const JUMP_VELOCITY = -400.0
const GRAVITY = 900.0

func _physics_process(delta: float) -> void:
	# Apply gravity when not standing on something
	if not is_on_floor():
		velocity.y += GRAVITY * delta

	# Horizontal movement with arrow keys
	var dir := 0.0
	if Input.is_action_pressed("ui_left"):
		dir -= 1.0
	if Input.is_action_pressed("ui_right"):
		dir += 1.0

	velocity.x = dir * SPEED

	# Jump with space/enter
	if Input.is_action_just_pressed("ui_accept") and is_on_floor():
		velocity.y = JUMP_VELOCITY

	move_and_slide()
