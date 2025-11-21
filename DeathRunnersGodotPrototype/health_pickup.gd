extends Area2D

@export var heal_amount: float = 30.0

func _on_body_entered(body: Node) -> void:
	if not body.is_in_group("player"):
		return

	if not body.has_method("heal"):
		return

	# Safely read health + max_health
	var current_health = body.get("health")
	var max_health = body.get("max_health")

	# If survivor script doesn't have these, bail out harmlessly
	if current_health == null or max_health == null:
		return

	# Already full HP? Don't consume the pickup.
	if current_health >= max_health:
		return

	# Heal and consume
	body.heal(heal_amount)
	queue_free()
