extends Camera2D

@export var survivor_path: NodePath
var survivor: Node2D

func _ready():
	make_current()
	survivor = get_node(survivor_path)

func _process(delta: float) -> void:
	if survivor == null:
		return

	var pos = global_position
	pos.x = survivor.global_position.x   # follow X
	# pos.y stays constant so camera doesn't bounce up/down
	global_position = pos
