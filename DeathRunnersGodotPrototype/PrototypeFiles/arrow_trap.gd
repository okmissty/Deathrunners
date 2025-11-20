extends Node2D

@export var arrow_scene: PackedScene
@export var fire_direction: Vector2 = Vector2.RIGHT
@export var max_uses: int = 3

var times_used: int = 0

func _ready() -> void:
	add_to_group("trap_arrow")

func can_activate() -> bool:
	return times_used < max_uses

func activate() -> void:
	if not can_activate():
		print("ArrowTrap at ", global_position, " is out of uses")
		return

	times_used += 1
	print("Arrow trap firing at: ", global_position, " use ", times_used, "/", max_uses)

	if arrow_scene == null:
		push_error("ArrowTrap has no arrow_scene assigned")
		return

	var arrow = arrow_scene.instantiate()
	get_tree().current_scene.add_child(arrow)

	# Spawn at muzzle position and set direction
	arrow.global_position = $Muzzle.global_position
	arrow.direction = fire_direction
