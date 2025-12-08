extends Camera2D

@export var smooth_speed: float = 5.0

func _process(delta: float) -> void:
	# 1. Get all player nodes in the scene
	var all_nodes = get_tree().get_nodes_in_group("player")
	var alive_survivors = []
	
	# 2. Filter for only ALIVE survivors
	for node in all_nodes:
		# Ensure node is valid and check its 'alive' property
		if is_instance_valid(node) and node.get("alive"):
			alive_survivors.append(node)
			
	# If everyone is dead, stop moving the camera
	if alive_survivors.is_empty():
		return
		
	# 3. Calculate Centroid (Average Position)
	var average_pos = Vector2.ZERO
	for s in alive_survivors:
		average_pos += s.global_position
		
	average_pos /= alive_survivors.size()
	
	# 4. Smoothly move camera to the average position
	# This keeps the group centered on screen
	global_position = global_position.lerp(average_pos, smooth_speed * delta)
