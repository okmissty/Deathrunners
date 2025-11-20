extends Area2D

@export var heal_amount: float = 30.0

func _on_body_entered(body: Node) -> void:
	if body.is_in_group("player"):
		if body.has_method("heal"):
			body.heal(heal_amount)
		queue_free()
