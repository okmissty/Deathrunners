extends Node2D

@export var survivor_scene: PackedScene = preload("res://survivor.tscn")
@onready var death_controller = $DeathController
@onready var end_label = $UI/EndLabel
@onready var role_label = $UI/RoleLabel
@onready var death_hud = $"UI/Death HUD"
@onready var horizontal_cam = $HorizontalCam
@onready var multiplayer_spawner = $MultiplayerSpawner

# Game Over UI References
@onready var game_over_menu = $UI/GameOverMenu 
@onready var restart_button = $UI/GameOverMenu/RestartButton
@onready var menu_button = $UI/GameOverMenu/MenuButton

var game_over: bool = false
var survivors = {}
var player_roles = {}
var death_player_id = -1
var my_role = ""
var players_ready = {}

func _ready() -> void:
	end_label.text = ""
	
	if game_over_menu: 
		game_over_menu.visible = false
	if restart_button: 
		restart_button.pressed.connect(_on_restart_pressed)
	if menu_button: 
		menu_button.pressed.connect(_on_menu_pressed)
	
	player_roles = get_tree().get_meta("player_roles", {})
	death_player_id = get_tree().get_meta("death_player_id", -1)
	
	if multiplayer.has_multiplayer_peer():
		setup_multiplayer()
		rpc_id(1, "player_ready", multiplayer.get_unique_id())
	else:
		get_tree().change_scene_to_file("res://menu.tscn")

func setup_multiplayer():
	var my_id = multiplayer.get_unique_id()
	my_role = player_roles.get(my_id, "Unknown")
	
	print("Setting up game. My ID: ", my_id, " My Role: ", my_role)
	
	if role_label:
		role_label.text = "You are: " + my_role
		if my_role == "Death":
			role_label.modulate = Color(1, 0.3, 0.3)
		else:
			role_label.modulate = Color(0.3, 1, 0.3)
	
	if my_role == "Death":
		if death_controller:
			death_controller.set_process(true)
			death_controller.enabled = true
			death_controller.visible = true
		if death_hud:
			death_hud.visible = true
	else:
		if death_controller:
			death_controller.set_process(false)
			death_controller.enabled = false
			death_controller.visible = false
		if death_hud:
			death_hud.visible = false
	
	if multiplayer_spawner:
		multiplayer_spawner.spawn_function = _spawn_survivor
	
	if multiplayer.is_server():
		multiplayer.peer_connected.connect(_on_peer_connected)
		multiplayer.peer_disconnected.connect(_on_peer_disconnected)

@rpc("any_peer", "call_local", "reliable")
func player_ready(player_id: int):
	if not multiplayer.is_server():
		return
	players_ready[player_id] = true
	print("Player ", player_id, " is ready. Total ready: ", players_ready.size(), "/", player_roles.size())
	if players_ready.size() >= player_roles.size():
		print("All players ready! Spawning survivors...")
		await get_tree().create_timer(0.5).timeout
		spawn_all_survivors()

func spawn_all_survivors():
	if not multiplayer.is_server():
		return
	var spawn_offset = 0
	for player_id in player_roles:
		if player_roles[player_id] == "Survivor":
			if multiplayer_spawner:
				var data = { "id": player_id, "x_offset": spawn_offset }
				multiplayer_spawner.spawn(data)
			else:
				spawn_survivor_for_player(player_id, spawn_offset)
			spawn_offset += 50

func _spawn_survivor(data: Dictionary) -> Node:
	var player_id = data.get("id", 1)
	var x_offset = data.get("x_offset", 0)
	
	var survivor_instance = survivor_scene.instantiate()
	survivor_instance.name = "Survivor_" + str(player_id)
	survivor_instance.position = Vector2(352 + x_offset, 501)
	survivor_instance.scale = Vector2(3, 3)
	survivor_instance.add_to_group("player")
	
	# --- FIX START ---
	# Set authority IMMEDIATELY, not inside a .ready callback
	survivor_instance.set_multiplayer_authority(player_id)
	# --- FIX END ---
	
	survivors[player_id] = survivor_instance
	
	# (You can remove the survivor_instance.ready.connect block entirely)
	
	print("Spawned survivor for player ", player_id, " at position ", survivor_instance.position)
	return survivor_instance

func spawn_survivor_for_player(player_id: int, x_offset: int):
	if survivors.has(player_id): return
	var survivor_instance = survivor_scene.instantiate()
	survivor_instance.name = "Survivor_" + str(player_id)
	survivor_instance.position = Vector2(352 + x_offset, 501)
	survivor_instance.scale = Vector2(3, 3)
	survivor_instance.add_to_group("player")
	add_child(survivor_instance)
	survivors[player_id] = survivor_instance
	survivor_instance.set_multiplayer_authority(player_id)
	print("Spawned survivor for player ", player_id)

func _on_peer_connected(id):
	print("Peer connected to game: ", id)

func _on_peer_disconnected(id):
	print("Peer disconnected from game: ", id)
	if survivors.has(id):
		if is_instance_valid(survivors[id]):
			survivors[id].queue_free()
		survivors.erase(id)

func _process(_delta: float) -> void:
	if game_over: return
	if Engine.get_process_frames() % 60 == 0:
		if death_controller and death_controller.enabled:
			death_controller._refresh_players()

	var any_survivor_alive = false
	var any_survivor_won = false
	var survivor_count = 0
	
	for survivor in survivors.values():
		if is_instance_valid(survivor):
			survivor_count += 1
			var alive = survivor.get("alive")
			var reached_goal = survivor.get("reached_goal")
			if alive: any_survivor_alive = true
			if reached_goal: any_survivor_won = true
	
	if survivor_count > 0:
		if any_survivor_won:
			_show_game_over("Survivors win!")
		elif not any_survivor_alive:
			_show_game_over("Death wins!")

func _show_game_over(text: String) -> void:
	if game_over: return
	game_over = true
	end_label.text = text
	if game_over_menu: game_over_menu.visible = true
	if multiplayer.is_server():
		if restart_button: restart_button.text = "Restart Game (Host)"
		sync_game_over.rpc(text)

@rpc("call_local", "reliable")
func sync_game_over(text: String):
	game_over = true
	if end_label: end_label.text = text
	if game_over_menu: game_over_menu.visible = true

func _on_restart_pressed():
	if multiplayer.is_server():
		rpc("reload_game_scene")

func _on_menu_pressed():
	if multiplayer.has_multiplayer_peer():
		multiplayer.multiplayer_peer.close()
	get_tree().change_scene_to_file("res://menu.tscn")

@rpc("call_local", "reliable")
func reload_game_scene():
	get_tree().reload_current_scene()
