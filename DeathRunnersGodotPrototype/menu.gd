# menu.gd - Complete script for menu.tscn
extends Control

const PORT = 9999
const MAX_PLAYERS = 2

@onready var status_label = $MenuContainer/StatusLabel
@onready var ip_input = $MenuContainer/IPInput
@onready var host_button = $MenuContainer/ButtonContainer/HostButton
@onready var join_button = $MenuContainer/ButtonContainer/JoinButton
@onready var start_button = $MenuContainer/StartButton

var peer = ENetMultiplayerPeer.new()

func _ready():
    # Connect button signals
    host_button.pressed.connect(_on_host_pressed)
    join_button.pressed.connect(_on_join_pressed)
    start_button.pressed.connect(_on_start_pressed)
    
    # Hide start button initially
    start_button.visible = false
    
    # Connect multiplayer signals
    multiplayer.peer_connected.connect(_on_peer_connected)
    multiplayer.peer_disconnected.connect(_on_peer_disconnected)
    multiplayer.connected_to_server.connect(_on_connected_to_server)
    multiplayer.connection_failed.connect(_on_connection_failed)

func _on_host_pressed():
    peer.create_server(PORT, MAX_PLAYERS)
    multiplayer.multiplayer_peer = peer
    
    status_label.text = "Hosting... Waiting for Death player"
    status_label.modulate = Color.GREEN
    
    host_button.disabled = true
    join_button.disabled = true
    start_button.visible = true

func _on_join_pressed():
    var ip = ip_input.text
    if ip == "":
        ip = "127.0.0.1"
    
    peer.create_client(ip, PORT)
    multiplayer.multiplayer_peer = peer
    
    status_label.text = "Connecting to " + ip + "..."
    status_label.modulate = Color.YELLOW
    
    host_button.disabled = true
    join_button.disabled = true

func _on_connected_to_server():
    status_label.text = "Connected! Waiting for host to start"
    status_label.modulate = Color.GREEN

func _on_peer_connected(id):
    print("Player connected: ", id)
    if multiplayer.is_server():
        status_label.text = "Death player connected! Ready to start"
        status_label.modulate = Color.GREEN

func _on_peer_disconnected(id):
    print("Player disconnected: ", id)
    status_label.text = "Player disconnected"
    status_label.modulate = Color.RED

func _on_connection_failed():
    status_label.text = "Connection failed!"
    status_label.modulate = Color.RED
    
    host_button.disabled = false
    join_button.disabled = false

func _on_start_pressed():
    if multiplayer.is_server():
        rpc("load_game")

@rpc("call_local", "reliable")
func load_game():
    get_tree().change_scene_to_file("res://main.tscn")
