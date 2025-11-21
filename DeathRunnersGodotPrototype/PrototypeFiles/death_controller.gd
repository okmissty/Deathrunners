extends Node2D

@export var survivor_path: NodePath
@export var hazard_scene: PackedScene  # Assign hazard.tscn in inspector

var survivor: Node2D
var falling_block_cooldown: float = 0.0
var falling_block_max_cooldown: float = 2.0  # 2 second cooldown

func _ready() -> void:
	if survivor_path != NodePath():
		survivor = get_node(survivor_path)
	else:
		survivor = null
	
	# Load hazard scene if not assigned
	if hazard_scene == null:
		hazard_scene = load("res://PrototypeFiles/hazard.tscn")
	
	print("DeathController ready, survivor = ", survivor)

func _process(delta: float) -> void:
	if survivor == null:
		return
	
	# Update cooldown
	if falling_block_cooldown > 0:
		falling_block_cooldown -= delta

	# Spawn falling block above player
	if Input.is_action_just_pressed("death_spawn_falling"):
		_spawn_falling_block()

	if Input.is_action_just_pressed("death_spawn_boulder"):
		_activate_next_trap_ahead("trap_boulder")

	if Input.is_action_just_pressed("death_aoe"):
		_activate_next_trap_ahead("trap_aoe")

	if Input.is_action_just_pressed("death_arrow"):
		_activate_next_trap_ahead("trap_arrow")

func _spawn_falling_block() -> void:
	if falling_block_cooldown > 0:
		print("Falling block on cooldown: ", falling_block_cooldown)
		return
	
	if hazard_scene == null:
		print("ERROR: hazard_scene not loaded!")
		return
	
	# Spawn above player
	var hazard = hazard_scene.instantiate()
	get_tree().current_scene.add_child(hazard)
	
	# Position it above the player
	var spawn_height = 200  # pixels above player
	hazard.global_position = Vector2(
		survivor.global_position.x,
		survivor.global_position.y - spawn_height
	)
	
	falling_block_cooldown = falling_block_max_cooldown
	print("Falling block spawned at: ", hazard.global_position)

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
