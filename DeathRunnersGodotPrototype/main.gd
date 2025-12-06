# main_multiplayer.gd - Complete replacement for main.gd
extends Node2D

@export var survivor_scene: PackedScene = preload("res://survivor.tscn")
@onready var death_controller = $DeathController
@onready var end_label = $UI/EndLabel
@onready var horizontal_cam = $HorizontalCam

var game_over: bool = false
var survivor_instance = null

func _ready() -> void:
	end_label.text = ""
	
	# Setup multiplayer spawning
	if multiplayer.has_multiplayer_peer():
		setup_multiplayer()
	else:
		# Single player mode - load menu
		get_tree().change_scene_to_file("res://menu.tscn")

func setup_multiplayer():
	print("Setting up multiplayer. Is server: ", multiplayer.is_server())
	
	if multiplayer.is_server():
		# Host controls survivor
		spawn_survivor()
		# Disable death controller for host
		if death_controller:
			death_controller.set_process(false)
			death_controller.enabled = false
		
		# Listen for client connections
		multiplayer.peer_connected.connect(_on_peer_connected)
	else:
		# Client controls death
		if death_controller:
			death_controller.set_process(true)
			death_controller.enabled = true
		
		# Request survivor spawn from server
		rpc_id(1, "request_survivor_spawn")

func spawn_survivor():
	if survivor_instance != null:
		return
		
	survivor_instance = survivor_scene.instantiate()
	survivor_instance.name = "Survivor"
	survivor_instance.position = Vector2(352, 501)
	survivor_instance.scale = Vector2(3, 3)
	
	# Set network authority
	if multiplayer.has_multiplayer_peer():
		survivor_instance.set_multiplayer_authority(1)  # Host always ID 1
	
	add_child(survivor_instance)
	
	# Update camera reference
	if horizontal_cam:
		horizontal_cam.survivor = survivor_instance

func _on_peer_connected(id):
	print("Peer connected to game: ", id)
	if multiplayer.is_server():
		# Sync survivor to new client
		if survivor_instance:
			rpc_id(id, "sync_survivor", survivor_instance.position)

@rpc("any_peer", "reliable")
func request_survivor_spawn():
	if multiplayer.is_server():
		spawn_survivor()

@rpc("reliable")
func sync_survivor(pos):
	if survivor_instance:
		survivor_instance.position = pos

func _process(delta: float) -> void:
	if game_over or survivor_instance == null:
		return

	if survivor_instance:
		var survivor_alive = survivor_instance.get("alive")
		var survivor_reached_goal = survivor_instance.get("reached_goal")
		
		if survivor_alive != null and not survivor_alive:
			_show_game_over("Death wins!")
		elif survivor_reached_goal != null and survivor_reached_goal:
			_show_game_over("Survivor wins!")

func _show_game_over(text: String) -> void:
	game_over = true
	end_label.text = text
	# Don't pause in multiplayer
	if not multiplayer.has_multiplayer_peer():
		get_tree().paused = true
