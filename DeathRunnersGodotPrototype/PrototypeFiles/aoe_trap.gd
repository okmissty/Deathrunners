extends CharacterBody2D

@export var damage: float = 10.0
@export var warning_time: float = 0.7
@export var active_time: float = 0.3
@export var animation_name: String = "explode"

@export var gravity: float = 2000.0
@export var initial_horizontal_speed: float = 0.0 
@export var should_roll_on_floor: bool = false

var state: String = "idle" 
var timer: float = 0.0

# FIX: Use an array to track WHO was hit, so we can hit multiple people
var damaged_bodies: Array = [] 

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
		hit_area.collision_mask = 3 # Hit Players (2) + World (1)

	z_index = 50
	print("[AoE] Ready. Damage: ", damage)

func can_activate() -> bool:
	return state == "idle"

func activate() -> void:
	if not can_activate():
		return

	state = "warning"
	timer = 0.0
	
	# FIX: Clear the hit list for the new explosion
	damaged_bodies.clear()
	
	show()
	velocity = Vector2(initial_horizontal_speed, 0.0)

	if bomb_sprite:
		bomb_sprite.visible = true
		bomb_sprite.scale = bomb_base_scale
	if explosion_anim:
		explosion_anim.visible = false
		explosion_anim.stop()
	if hit_area:
		hit_area.monitoring = false
		
	print("[AoE] Armed at ", global_position)

func _physics_process(delta: float) -> void:
	if state == "warning":
		velocity.y += gravity * delta
		move_and_slide()
		if is_on_floor():
			if should_roll_on_floor:
				velocity.y = 0.0
			else:
				velocity = Vector2.ZERO
	elif state == "active" or state == "done":
		velocity = Vector2.ZERO

func _process(delta: float) -> void:
	if state == "idle" or state == "done":
		return

	timer += delta

	if state == "warning":
		if bomb_sprite:
			var t := sin(timer * 10.0) * 0.08
			bomb_sprite.scale = bomb_base_scale * (1.0 + t)
		if timer >= warning_time:
			_start_explosion()

	elif state == "active":
		if timer >= active_time:
			state = "done"
			if hit_area: hit_area.monitoring = false
			print("[AoE] Finished")
			queue_free()

func _start_explosion() -> void:
	state = "active"
	timer = 0.0
	velocity = Vector2.ZERO
	print("[AoE] BOOM! Exploding now.")

	if bomb_sprite: bomb_sprite.visible = false
	if explosion_anim:
		explosion_anim.visible = true
		explosion_anim.scale = explosion_base_scale
		explosion_anim.play(animation_name if animation_name != "" else "default")

	if hit_area:
		hit_area.monitoring = true
		
		# Optional: Force check for bodies already inside the area
		for body in hit_area.get_overlapping_bodies():
			_handle_body_entered(body)

func _on_hit_area_body_entered(body: Node) -> void:
	_handle_body_entered(body)

func _on_area_body_entered(body: Node) -> void:
	_handle_body_entered(body)

func _handle_body_entered(body: Node) -> void:
	if state != "active":
		return
		
	# FIX: check if THIS SPECIFIC body has been hit
	if body in damaged_bodies:
		return

	if body.is_in_group("player"):
		# FIX: Add to list so we don't hit THIS player again in the same frame/explosion
		damaged_bodies.append(body)
		
		if multiplayer.is_server() and body.has_method("apply_damage"):
			print("[AoE] BLASTED Survivor: ", body.name, " -> Dealing ", damage, " damage")
			body.apply_damage(damage)
		else:
			print("[AoE] Hit Survivor: ", body.name, " (Visual only)")
