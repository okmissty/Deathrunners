extends Area2D

func _ready() -> void:
	monitoring = true
	monitorable = true

func _on_body_entered(body: Node) -> void:
	if body.is_in_group("player"):
		if body.has_method("set_checkpoint"):
			# Set checkpoint slightly above the checkpoint so player doesn't spawn inside the ground
			body.set_checkpoint(global_position + Vector2(0, -10))
			print("Checkpoint updated at: ", global_position)
