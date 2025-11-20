extends Node2D

@export var boulder_trap_path: NodePath

var boulder_trap: Node2D

func _ready() -> void:
	if boulder_trap_path != NodePath():
		boulder_trap = get_node(boulder_trap_path)
		print("DeathController found boulder_trap: ", boulder_trap.name)

func _process(_delta: float) -> void:
	# Uses your input action from the screenshot: death_spawn_boulder (key 2)
	if Input.is_action_just_pressed("death_spawn_boulder"):
		print("death_spawn_boulder pressed")
		if boulder_trap != null:
			boulder_trap.activate()
		else:
			print("No boulder_trap set on DeathController!")
