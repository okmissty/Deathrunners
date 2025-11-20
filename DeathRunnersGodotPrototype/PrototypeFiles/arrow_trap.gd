extends Node2D

@export var arrow_scene: PackedScene
@export var fire_direction: Vector2 = Vector2.RIGHT

var used: bool = false

func _ready() -> void:
	add_to_group("trap_arrow")

func activate() -> void:
	if used:
		return
	used = true
	print("Arrow trap firing at: ", global_position)

	if arrow_scene == null:
		push_error("ArrowTrap has no arrow_scene assigned")
		return

	var arrow = arrow_scene.instantiate()
	get_tree().current_scene.add_child(arrow)

	# Spawn at muzzle position and set direction
	arrow.global_position = $Muzzle.global_position
	arrow.direction = fire_direction
