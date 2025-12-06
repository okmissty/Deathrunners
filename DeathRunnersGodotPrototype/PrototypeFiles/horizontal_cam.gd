# horizontal_cam_multiplayer.gd - Complete replacement for horizontal_cam.gd
extends Camera2D

@export var survivor_path: NodePath
var survivor: Node2D

func _ready():
	make_current()
	# Try to get survivor from path
	if survivor_path != NodePath(""):
		survivor = get_node_or_null(survivor_path)
	
	# If no survivor found, we'll look for it dynamically

func _process(delta: float) -> void:
	# If we don't have a survivor reference, try to find it
	if survivor == null:
		var survivors = get_tree().get_nodes_in_group("player")
		if survivors.size() > 0:
			survivor = survivors[0]
	
	# If we still don't have a survivor, return
	if survivor == null:
		return

	# Follow the survivor's X position
	var pos = global_position
	pos.x = survivor.global_position.x
	# pos.y stays constant so camera doesn't bounce up/down
	global_position = pos
