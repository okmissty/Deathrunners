extends Node2D

@export var hazard_scene: PackedScene      # We'll assign Hazard.tscn in the editor
@export var survivor_path: NodePath        # We'll point this at the Survivor node

var survivor: Node2D

func _ready():
	# Get the survivor node from the path
	if survivor_path != NodePath():
		survivor = get_node(survivor_path)

func _process(delta: float) -> void:
	if Input.is_action_just_pressed("death_spawn_rock"):
		spawn_hazard_over_survivor()

func spawn_hazard_over_survivor() -> void:
	if hazard_scene == null or survivor == null:
		return

	# Instance a new hazard
	var hazard = hazard_scene.instantiate()
	# Add the hazard to the current scene (Main)
	get_tree().current_scene.add_child(hazard)

	# Position it above the survivor
	# Feel free to tweak the offset values
	hazard.global_position = survivor.global_position + Vector2(0, -200)
