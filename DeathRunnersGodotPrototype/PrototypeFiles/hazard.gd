extends RigidBody2D

func _ready() -> void:
	contact_monitor = true
	max_contacts_reported = 4

func _on_body_entered(body: Node) -> void:
	# Only care if we hit the player
	if body.is_in_group("player"):
		if body.has_method("hit_by_hazard"):
			body.hit_by_hazard()
		queue_free()  # remove this hazard after hit
