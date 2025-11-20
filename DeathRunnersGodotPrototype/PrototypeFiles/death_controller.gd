extends Node2D

@export var hazard_scene: PackedScene
@export var survivor_path: NodePath

var survivor: Node2D

func _ready() -> void:
	if survivor_path != NodePath():
		survivor = get_node(survivor_path)

func _process(delta: float) -> void:
	# Example: press '1' to drop a rock above survivor
	if Input.is_action_just_pressed("death_spawn_rock"):
		spawn_hazard_over_survivor()

func spawn_hazard_over_survivor() -> void:
	if hazard_scene == null or survivor == null:
		print("No hazard_scene or survivor set on DeathController")
		return

	var h = hazard_scene.instantiate()
	get_tree().current_scene.add_child(h)

	# spawn above the survivor
	h.global_position = survivor.global_position + Vector2(0, -200)
	
