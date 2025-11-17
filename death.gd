extends CharacterBody2D

const SPEED = 250.0
const JUMP_VELOCITY = -450.0
const GRAVITY = 1000.0

func _physics_process(delta):
	if not is_on_floor():
		velocity.y += GRAVITY * delta

	var dir := 0.0
	# Use A/D for Death
	if Input.is_key_pressed(KEY_A):
		dir -= 1.0
	if Input.is_key_pressed(KEY_D):
		dir += 1.0
	velocity.x = dir * SPEED

	if Input.is_key_pressed(KEY_W) and is_on_floor():
		velocity.y = JUMP_VELOCITY

	move_and_slide()
