extends Area2D

@export var speed: float = 500.0
@export var damage: float = 15.0 # Set this to 15
@export var direction: Vector2 = Vector2.RIGHT

var active: bool = true

func _ready() -> void:
	monitoring = true
	# CRITICAL: Ensure arrow detects Layer 2 (Player Layer)
	set_collision_mask_value(2, true)

func _process(delta: float) -> void:
	if not active:
		return

	global_position += direction.normalized() * speed * delta

	# Despawn if very far away
	if global_position.x < -2000 or global_position.x > 20000:
		queue_free()

func _on_body_entered(body: Node2D) -> void:
	if not active:
		return

	if body.is_in_group("player"):
		# FIX: Only the Server (Host) triggers the damage.
		# This prevents the "Double Damage" bug.
		if multiplayer.is_server() and body.has_method("apply_damage"):
			body.apply_damage(damage)
		
		# Visual cleanup happens for EVERYONE so the arrow disappears instantly
		active = false
		queue_free()
