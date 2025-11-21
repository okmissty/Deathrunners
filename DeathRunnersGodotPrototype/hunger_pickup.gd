extends Area2D

@export var hunger_amount: float = 40.0

func _on_body_entered(body: Node) -> void:
	if not body.is_in_group("player"):
		return

	if not body.has_method("restore_hunger"):
		return

	# Safely read hunger + max_hunger
	var current_hunger = body.get("hunger")
	var max_hunger = body.get("max_hunger")

	if current_hunger == null or max_hunger == null:
		return

	# Already full hunger? Don't consume the pickup.
	if current_hunger >= max_hunger:
		return

	# Restore hunger and consume
	body.restore_hunger(hunger_amount)
	queue_free()
