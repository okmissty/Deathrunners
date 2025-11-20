extends Area2D

@export var heal_amount: float = 30.0
@export var hunger_amount: float = 40.0

func _on_body_entered(body: Node) -> void:
	if not body.is_in_group("player"):
		return

	# Safe property reads
	var health = body.get("health")
	var max_health = body.get("max_health")
	var hunger = body.get("hunger")
	var max_hunger = body.get("max_hunger")

	# If any required values are missing, bail safely
	if health == null or max_health == null or hunger == null or max_hunger == null:
		return

	var healed = false

	# Heal HP if not full
	if health < max_health and body.has_method("heal"):
		body.heal(heal_amount)
		healed = true

	# Restore hunger if not full
	if hunger < max_hunger and body.has_method("restore_hunger"):
		body.restore_hunger(hunger_amount)
		healed = true

	# Only consume pickup if something was restored
	if healed:
		queue_free()
