extends Area2D

@export var fall_speed: float = 300.0
@export var damage: float = 30.0

func _process(delta: float) -> void:
	# Simple "fake gravity" so it falls straight down
	global_position.y += fall_speed * delta

func _on_body_entered(body: Node) -> void:
	# Debug: see what we hit
	print("Hazard overlapped with: ", body.name)

	if body.is_in_group("player"):
		print("Hazard hit player, applying damage: ", damage)
		if body.has_method("apply_damage"):
			body.apply_damage(damage)
		queue_free()  # remove hazard so it only hits once
