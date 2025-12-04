extends CharacterBody2D

@export var damage: float = 30.0
@export var warning_time: float = 0.7
@export var active_time: float = 0.3
@export var animation_name: String = "explode"

@export var gravity: float = 2000.0
@export var initial_horizontal_speed: float = 0.0  # >0 to roll right, <0 to roll left
@export var should_roll_on_floor: bool = false     # false = stop when hitting floor

var state: String = "idle"   # "idle" -> "warning" -> "active" -> "done"
var timer: float = 0.0
var has_damaged: bool = false

var bomb_sprite: Sprite2D
var explosion_anim: AnimatedSprite2D
var hit_area: Area2D

var bomb_base_scale: Vector2 = Vector2.ONE
var explosion_base_scale: Vector2 = Vector2.ONE

func _ready() -> void:
	add_to_group("trap_aoe")

	bomb_sprite = get_node_or_null("BombSprite")
	explosion_anim = get_node_or_null("AnimatedSprite2D")
	hit_area = get_node_or_null("HitArea")

	if bomb_sprite:
		bomb_base_scale = bomb_sprite.scale
		bomb_sprite.visible = false

	if explosion_anim:
		explosion_base_scale = explosion_anim.scale
		explosion_anim.visible = false
		explosion_anim.stop()

	if hit_area:
		hit_area.monitoring = false

	z_index = 50
	if bomb_sprite:
		bomb_sprite.z_index = 50
	if explosion_anim:
		explosion_anim.z_index = 50

	print("AoETrap ready. bomb =", bomb_sprite, " explosion =", explosion_anim, " hit_area =", hit_area)


func can_activate() -> bool:
	return state == "idle"


func activate() -> void:
	if not can_activate():
		return

	state = "warning"
	timer = 0.0
	has_damaged = false

	show()

	# start falling/rolling
	velocity = Vector2(initial_horizontal_speed, 0.0)

	# show bomb, hide explosion
	if bomb_sprite:
		bomb_sprite.visible = true
		bomb_sprite.scale = bomb_base_scale
		bomb_sprite.modulate = Color(1, 1, 1, 1)

	if explosion_anim:
		explosion_anim.visible = false
		explosion_anim.stop()
		explosion_anim.scale = explosion_base_scale

	if hit_area:
		hit_area.monitoring = false

	print("AoE trap armed at:", global_position)


func _physics_process(delta: float) -> void:
	if state == "warning":
		# apply gravity
		velocity.y += gravity * delta

		# move using built-in velocity
		move_and_slide()

		# when we touch the floor
		if is_on_floor():
			if should_roll_on_floor:
				# keep horizontal speed, stop falling
				velocity.y = 0.0
			else:
				# stop completely where we landed
				velocity = Vector2.ZERO

	elif state == "active" or state == "done":
		# freeze motion once we explode or are done
		velocity = Vector2.ZERO

func _process(delta: float) -> void:
	if state == "idle" or state == "done":
		return

	timer += delta

	if state == "warning":
		# bomb telegraph (pulse)
		if bomb_sprite:
			var t := sin(timer * 10.0) * 0.08
			bomb_sprite.scale = bomb_base_scale * (1.0 + t)

		if timer >= warning_time:
			_start_explosion()

	elif state == "active":
		if timer >= active_time:
			state = "done"
			if hit_area:
				hit_area.monitoring = false
			queue_free()


func _start_explosion() -> void:
	state = "active"
	timer = 0.0

	print("AoE trap EXPLODING at:", global_position)

	# Freeze physics in place when it explodes
	velocity = Vector2.ZERO

	if bomb_sprite:
		bomb_sprite.visible = false

	if explosion_anim:
		explosion_anim.visible = true
		explosion_anim.scale = explosion_base_scale
		#explosion_anim.modulate = Color(1, 0.2, 0.2, 1.0)

		var frames := explosion_anim.sprite_frames
		var anim_name := animation_name

		if anim_name == "" and frames != null and frames.get_animation_names().size() > 0:
			anim_name = frames.get_animation_names()[0]

		if anim_name != "":
			explosion_anim.animation = anim_name
		explosion_anim.play()

	if hit_area:
		hit_area.monitoring = true


func _handle_body_entered(body: Node) -> void:
	if state != "active":
		return
	if has_damaged:
		return

	if body.is_in_group("player") and body.has_method("apply_damage"):
		print("AoE hit player for", damage)
		body.apply_damage(damage)
		has_damaged = true


func _on_area_body_entered(body: Node) -> void:
	_handle_body_entered(body)


func _on_hit_area_body_entered(body: Node) -> void:
	_handle_body_entered(body)
