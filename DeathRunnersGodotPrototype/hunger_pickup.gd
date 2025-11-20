extends Area2D

@export var hunger_amount: float = 40.0

func _on_body_entered(body: Node) -> void:
	if body.is_in_group("player"):
		if body.has_method("restore_hunger"):
			body.restore_hunger(hunger_amount)
		queue_free()
