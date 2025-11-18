extends RigidBody2D

@export var damage: float = 30.0

func _on_body_entered(body: Node) -> void:
	if body.is_in_group("player"):
		if body.has_method("apply_damage"):
			body.apply_damage(damage)
		queue_free()
