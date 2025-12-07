# main_multiplayer.gd - Fixed with proper UI visibility and trap controls
extends Node2D

@export var survivor_scene: PackedScene = preload("res://survivor.tscn")
@onready var death_controller = $DeathController
@onready var end_label = $UI/EndLabel
@onready var role_label = $UI/RoleLabel
@onready var death_hud = $"UI/Death HUD"  # Death HUD container
@onready var horizontal_cam = $HorizontalCam
@onready var multiplayer_spawner = $MultiplayerSpawner

var game_over: bool = false
var survivors = {}  # Dictionary to track all survivors
var player_roles = {}
var death_player_id = -1
var my_role = ""

func _ready() -> void:
	end_label.text = ""
	
	# Get role information from menu
	player_roles = get_tree().get_meta("player_roles", {})
	death_player_id = get_tree().get_meta("death_player_id", -1)
	
	# Setup multiplayer spawning
	if multiplayer.has_multiplayer_peer():
		setup_multiplayer()
	else:
		# Single player mode - load menu
		get_tree().change_scene_to_file("res://menu.tscn")

func setup_multiplayer():
	var my_id = multiplayer.get_unique_id()
	my_role = player_roles.get(my_id, "Unknown")
	
	print("Setting up game. My ID: ", my_id, " My Role: ", my_role)
	
	# Display role at top of screen
	if role_label:
		role_label.text = "You are: " + my_role
		if my_role == "Death":
			role_label.modulate = Color(1, 0.3, 0.3)
		else:
			role_label.modulate = Color(0.3, 1, 0.3)
	
	# Setup based on role
	if my_role == "Death":
		# Enable death controller only for Death player
		if death_controller:
			death_controller.set_process(true)
			death_controller.enabled = true
			death_controller.visible = true
		# Show Death HUD for Death player
		if death_hud:
			death_hud.visible = true
	else:
		# Disable death controller for survivors
		if death_controller:
			death_controller.set_process(false)
			death_controller.enabled = false
			death_controller.visible = false
		# Hide Death HUD for survivors
		if death_hud:
			death_hud.visible = false
	
	# Configure MultiplayerSpawner
	if multiplayer_spawner:
		multiplayer_spawner.spawn_function = _spawn_survivor
	
	# Host spawns all survivors
	if multiplayer.is_server():
		# Wait a frame to ensure all clients are ready
		await get_tree().process_frame
		spawn_all_survivors()
		
		# Listen for new connections
		multiplayer.peer_connected.connect(_on_peer_connected)
		multiplayer.peer_disconnected.connect(_on_peer_disconnected)

func spawn_all_survivors():
	if not multiplayer.is_server():
		return
	
	var spawn_offset = 0
	for player_id in player_roles:
		if player_roles[player_id] == "Survivor":
			# Use multiplayer spawner if available
			if multiplayer_spawner:
				var data = {
					"id": player_id,
					"x_offset": spawn_offset
				}
				multiplayer_spawner.spawn(data)
			else:
				spawn_survivor_for_player(player_id, spawn_offset)
			spawn_offset += 50  # Space out survivors

func _spawn_survivor(data: Dictionary) -> Node:
	# This is called by MultiplayerSpawner
	var player_id = data.get("id", 1)
	var x_offset = data.get("x_offset", 0)
	
	var survivor_instance = survivor_scene.instantiate()
	survivor_instance.name = "Survivor_" + str(player_id)
	survivor_instance.position = Vector2(352 + x_offset, 501)
	survivor_instance.scale = Vector2(3, 3)
	
	# Add to player group for Death controller to target
	survivor_instance.add_to_group("player")
	
	# Store in our dictionary
	survivors[player_id] = survivor_instance
	
	# Set authority AFTER adding to tree
	survivor_instance.ready.connect(func():
		survivor_instance.set_multiplayer_authority(player_id)
		print("Set authority for survivor ", player_id)
		
		# Update camera for local player
		if player_id == multiplayer.get_unique_id() and horizontal_cam:
			horizontal_cam.survivor = survivor_instance
	)
	
	print("Spawned survivor for player ", player_id, " at position ", survivor_instance.position)
	
	return survivor_instance

func spawn_survivor_for_player(player_id: int, x_offset: int):
	# Fallback method if no MultiplayerSpawner
	if survivors.has(player_id):
		return  # Already spawned
	
	var survivor_instance = survivor_scene.instantiate()
	survivor_instance.name = "Survivor_" + str(player_id)
	survivor_instance.position = Vector2(352 + x_offset, 501)
	survivor_instance.scale = Vector2(3, 3)
	
	# Add to player group for Death controller to target
	survivor_instance.add_to_group("player")
	
	add_child(survivor_instance)
	survivors[player_id] = survivor_instance
	
	# Set authority after adding to tree
	survivor_instance.set_multiplayer_authority(player_id)
	
	print("Spawned survivor for player ", player_id, " at position ", survivor_instance.position)
	
	# Update camera to follow local player's survivor
	if player_id == multiplayer.get_unique_id() and horizontal_cam:
		horizontal_cam.survivor = survivor_instance

func _on_peer_connected(id):
	print("Peer connected to game: ", id)
	# Resync if needed

func _on_peer_disconnected(id):
	print("Peer disconnected from game: ", id)
	# Remove their survivor if they had one
	if survivors.has(id):
		if is_instance_valid(survivors[id]):
			survivors[id].queue_free()
		survivors.erase(id)

func _process(_delta: float) -> void:
	if game_over:
		return
	
	# Refresh player list periodically for death controller
	if Engine.get_process_frames() % 60 == 0:  # Every second
		if death_controller and death_controller.enabled:
			death_controller._refresh_players()

	# Check win conditions
	var any_survivor_alive = false
	var any_survivor_won = false
	
	for survivor in survivors.values():
		if is_instance_valid(survivor):
			var alive = survivor.get("alive")
			var reached_goal = survivor.get("reached_goal")
			
			if alive:
				any_survivor_alive = true
			if reached_goal:
				any_survivor_won = true
	
	if any_survivor_won:
		_show_game_over("Survivors win!")
	elif not any_survivor_alive and survivors.size() > 0:
		_show_game_over("Death wins!")

func _show_game_over(text: String) -> void:
	if game_over:
		return
	
	game_over = true
	end_label.text = text
	
	# Notify all players
	if multiplayer.is_server():
		sync_game_over.rpc(text)

@rpc("call_local", "reliable")
func sync_game_over(text: String):
	game_over = true
	if end_label:
		end_label.text = text
