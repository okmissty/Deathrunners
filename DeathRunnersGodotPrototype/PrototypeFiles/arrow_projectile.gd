extends Node2D

@export var speed: float = 500.0
@export var damage: float = 20.0
@export var direction: Vector2 = Vector2.RIGHT

var active: bool = true

func _process(delta: float) -> void:
	if !active:
		return
	global_position += direction.normalized() * speed * delta

	# Optional: despawn if off-screen / too far
	if global_position.x < -2000 or global_position.x > 20000:
		queue_free()

func _on_HitArea_body_entered(body: Node) -> void:
	if !active:
		return
	if body.is_in_group("player") and body.has_method("apply_damage"):
		print("Arrow hit player for ", damage)
		body.apply_damage(damage)
		active = false
		queue_free()


func _on_hit_area_body_entered(body: Node2D) -> void:
	pass # Replace with function body.
