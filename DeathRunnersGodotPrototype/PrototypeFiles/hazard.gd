extends RigidBody2D

@export var damage: float = 30.0

func _ready() -> void:
	# Allow this body to report contacts in Godot 4
	contact_monitor = true
	max_contacts_reported = 4

func _on_body_entered(body: Node) -> void:
	print("Hazard collided with: ", body.name)

	if body.is_in_group("player"):
		print("Hazard hit player, applying damage: ", damage)
		if body.has_method("apply_damage"):
			body.apply_damage(damage)
		queue_free()
