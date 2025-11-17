extends Node2D

@export var hazard_scene: PackedScene
@export var survivor_path: NodePath

var survivor: Node2D

func _ready():
	survivor = get_node(survivor_path)

func _process(delta):
	# Example: press "1" to drop a hazard above the Survivor
	if Input.is_key_pressed(KEY_1):
		spawn_hazard_over_survivor()

func spawn_hazard_over_survivor():
	if survivor == null or hazard_scene == null:
		return

	var h = hazard_scene.instantiate()
	get_tree().current_scene.add_child(h)

	# Spawn position a bit above survivor
	h.global_position = survivor.global_position + Vector2(0, -200)
