extends Area2D

func _on_body_entered(body: Node) -> void:
	if body.is_in_group("player"):
		if body.has_method("mark_goal_reached"):
			body.mark_goal_reached()
