# menu.gd - Improved with proper role assignment for 2-4 players
extends Control

const PORT = 9999
const MIN_PLAYERS = 2
const MAX_PLAYERS = 4

@onready var status_label = $MenuContainer/StatusLabel
@onready var ip_input = $MenuContainer/IPInput
@onready var host_button = $MenuContainer/ButtonContainer/HostButton
@onready var join_button = $MenuContainer/ButtonContainer/JoinButton
@onready var start_button = $MenuContainer/StartButton
@onready var player_list = $MenuContainer/PlayerList

var peer = ENetMultiplayerPeer.new()
var connected_players = {}  # Dictionary to track players and their roles
var death_player_id = -1  # Track who is Death

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
    
    # Host is always a survivor initially
    connected_players[1] = "Survivor"  # Host is always ID 1
    
    status_label.text = "Hosting... Waiting for players (1/" + str(MAX_PLAYERS) + ")"
    status_label.modulate = Color.GREEN
    
    host_button.disabled = true
    join_button.disabled = true
    start_button.visible = true
    start_button.disabled = true  # Disabled until minimum players
    
    _update_player_list()

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
    # Request current player list from host
    rpc_id(1, "request_player_list")

func _on_peer_connected(id):
    print("Player connected: ", id)
    if multiplayer.is_server():
        # Assign new player as survivor initially
        connected_players[id] = "Survivor"
        
        var player_count = connected_players.size()
        status_label.text = "Players connected: " + str(player_count) + "/" + str(MAX_PLAYERS)
        
        # Enable start button if we have minimum players
        if player_count >= MIN_PLAYERS:
            start_button.disabled = false
            status_label.modulate = Color.GREEN
        
        # Randomly assign one player as Death when we have enough players
        if player_count >= MIN_PLAYERS and death_player_id == -1:
            _assign_death_player()
        
        # Sync player list to all clients
        rpc("update_player_list", connected_players)
        _update_player_list()

func _on_peer_disconnected(id):
    print("Player disconnected: ", id)
    if multiplayer.is_server():
        connected_players.erase(id)
        
        # If Death player left, reassign
        if id == death_player_id and connected_players.size() >= MIN_PLAYERS:
            _assign_death_player()
        elif id == death_player_id:
            death_player_id = -1
        
        var player_count = connected_players.size()
        status_label.text = "Player disconnected. Players: " + str(player_count) + "/" + str(MAX_PLAYERS)
        
        # Disable start if below minimum
        if player_count < MIN_PLAYERS:
            start_button.disabled = true
            status_label.modulate = Color.YELLOW
        
        # Sync to all clients
        rpc("update_player_list", connected_players)
        _update_player_list()

func _on_connection_failed():
    status_label.text = "Connection failed!"
    status_label.modulate = Color.RED
    
    host_button.disabled = false
    join_button.disabled = false

func _assign_death_player():
    # Randomly select one player to be Death
    var player_ids = connected_players.keys()
    death_player_id = player_ids[randi() % player_ids.size()]
    
    # Update roles
    for id in connected_players:
        if id == death_player_id:
            connected_players[id] = "Death"
        else:
            connected_players[id] = "Survivor"
    
    print("Death player assigned: ", death_player_id)

func _on_start_pressed():
    if multiplayer.is_server():
        # Ensure we have a Death player
        if death_player_id == -1:
            _assign_death_player()
        
        # Send final role assignments
        rpc("finalize_roles", connected_players, death_player_id)
        # Start the game
        rpc("load_game")

@rpc("any_peer", "reliable")
func request_player_list():
    if multiplayer.is_server():
        var sender_id = multiplayer.get_remote_sender_id()
        rpc_id(sender_id, "update_player_list", connected_players)

@rpc("call_local", "reliable")
func update_player_list(players):
    connected_players = players
    _update_player_list()

@rpc("call_local", "reliable")
func finalize_roles(players, death_id):
    connected_players = players
    death_player_id = death_id
    
    # Store in a global singleton or pass to game scene
    # For now, we'll use metadata on the scene tree
    get_tree().set_meta("player_roles", connected_players)
    get_tree().set_meta("death_player_id", death_player_id)
    
    print("My ID: ", multiplayer.get_unique_id())
    print("My Role: ", connected_players.get(multiplayer.get_unique_id(), "Unknown"))

@rpc("call_local", "reliable")
func load_game():
    get_tree().change_scene_to_file("res://main.tscn")

func _update_player_list():
    if player_list:
        var text = "Players:\n"
        for id in connected_players:
            var role = connected_players[id]
            var name = "Player " + str(id)
            if id == 1:
                name += " (Host)"
            if id == multiplayer.get_unique_id():
                name += " (You)"
            text += name + " - " + role + "\n"
        player_list.text = text
