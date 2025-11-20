extends Node2D

@export var survivor_path: NodePath

var survivor: Node2D

func _ready() -> void:
	if survivor_path != NodePath():
		survivor = get_node(survivor_path)
	else:
		survivor = null
	print("DeathController ready, survivor = ", survivor)

func _process(_delta: float) -> void:
	if survivor == null:
		return

	if Input.is_action_just_pressed("death_spawn_boulder"):
		_activate_next_trap_ahead("trap_boulder")

	if Input.is_action_just_pressed("death_aoe"):
		_activate_next_trap_ahead("trap_aoe")

	if Input.is_action_just_pressed("death_spawn_falling"):
		_activate_next_trap_ahead("trap_falling")

	if Input.is_action_just_pressed("death_arrow"):
		_activate_next_trap_ahead("trap_arrow")


func _activate_next_trap_ahead(group_name: String) -> void:
	var traps: Array = get_tree().get_nodes_in_group(group_name)
	print("Trying to activate group '", group_name, "', found ", traps.size(), " traps")

	if traps.is_empty():
		print("No traps of type ", group_name, " in the scene")
		return

	var survivor_x: float = survivor.global_position.x
	var best_trap: Node = null
	var best_x: float = INF

	for trap in traps:
		if trap == null:
			continue
		if not trap.has_method("activate") or not trap.has_method("can_activate"):
			continue
		if not trap.can_activate():
			continue

		var x: float = trap.global_position.x
		if x > survivor_x and x < best_x:
			best_x = x
			best_trap = trap

	if best_trap != null:
		print("Activating trap: ", best_trap.name, " at x=", best_trap.global_position.x)
		best_trap.activate()
	else:
		print("No available ", group_name, " trap ahead of survivor")
