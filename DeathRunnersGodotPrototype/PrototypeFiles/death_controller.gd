extends Node2D

@export var aoe_scene: PackedScene
@export var falling_block_scene: PackedScene

var preplaced_traps: Array = []
var selected_trap_index: int = -1

var players: Array = []
var selected_player_index: int = 0

@onready var trap_indicator: Node2D = $TrapIndicator
@onready var player_indicator: Node2D = $PlayerIndicator

@onready var trap_label: Label = $"../UI/DeathHUD/TrapLabel"
@onready var target_label: Label = $"../UI/DeathHUD/TargetLabel"

var trap_indicator_base_scale: Vector2 = Vector2.ONE
var player_indicator_base_scale: Vector2 = Vector2.ONE


func _ready() -> void:
	_refresh_preplaced_traps()
	_refresh_players()

	if trap_indicator != null:
		trap_indicator_base_scale = trap_indicator.scale
		trap_indicator.visible = false
	if player_indicator != null:
		player_indicator_base_scale = player_indicator.scale
		player_indicator.visible = false

	_update_trap_highlight()
	_update_player_highlight()


func _process(_delta: float) -> void:
	# --- PREPLACED TRAPS ---
	if Input.is_action_just_pressed("death_trap_prev"):
		_select_prev_trap()
	if Input.is_action_just_pressed("death_trap_next"):
		_select_next_trap()
	if Input.is_action_just_pressed("death_trap_activate"):
		_activate_selected_trap()

	# --- PLAYER SELECTION ---
	if Input.is_action_just_pressed("death_player_prev"):
		_select_prev_player()
	if Input.is_action_just_pressed("death_player_next"):
		_select_next_player()

	# --- SPAWNED TRAPS ---
	if Input.is_action_just_pressed("death_spawn_aoe"):
		_spawn_aoe_on_selected_player()
	if Input.is_action_just_pressed("death_spawn_falling"):
		_spawn_falling_on_selected_player()

	# keep player indicator following selected player
	_update_player_indicator_follow()


# -------------------------------------------------------------------
# PREPLACED TRAPS (boulders + arrows)
# -------------------------------------------------------------------

func _refresh_preplaced_traps() -> void:
	preplaced_traps = []
	preplaced_traps += get_tree().get_nodes_in_group("trap_boulder")
	preplaced_traps += get_tree().get_nodes_in_group("trap_arrow")

	preplaced_traps.sort_custom(Callable(self, "_compare_traps_by_x"))

	if preplaced_traps.size() > 0:
		if selected_trap_index < 0 or selected_trap_index >= preplaced_traps.size():
			selected_trap_index = 0
	else:
		selected_trap_index = -1


func _compare_traps_by_x(a: Node, b: Node) -> bool:
	if not (a is Node2D) or not (b is Node2D):
		return false
	var na := a as Node2D
	var nb := b as Node2D
	return na.global_position.x < nb.global_position.x


func _select_prev_trap() -> void:
	if preplaced_traps.is_empty():
		return
	selected_trap_index = (selected_trap_index - 1 + preplaced_traps.size()) % preplaced_traps.size()
	_update_trap_highlight()


func _select_next_trap() -> void:
	if preplaced_traps.is_empty():
		return
	selected_trap_index = (selected_trap_index + 1) % preplaced_traps.size()
	_update_trap_highlight()


func _current_trap() -> Node2D:
	if selected_trap_index < 0 or selected_trap_index >= preplaced_traps.size():
		return null
	var t = preplaced_traps[selected_trap_index]
	return t as Node2D


func _update_trap_highlight() -> void:
	var trap := _current_trap()

	# Indicator
	if trap_indicator != null:
		if trap != null:
			trap_indicator.visible = true
			trap_indicator.global_position = trap.global_position + Vector2(0, -24)
			_pulse_indicator(trap_indicator, trap_indicator_base_scale)
		else:
			trap_indicator.visible = false

	# HUD
	_update_trap_hud(trap)


func _update_trap_hud(trap: Node) -> void:
	if trap_label == null:
		return

	if trap == null:
		trap_label.text = "Trap: (none)"
		return

	var base_name := "Trap"
	if trap.is_in_group("trap_boulder"):
		base_name = "Boulder Trap"
	elif trap.is_in_group("trap_arrow"):
		base_name = "Arrow Trap"

	var uses_text := ""
	var max_uses = trap.get("max_uses")
	var times_used = trap.get("times_used")
	if max_uses != null and times_used != null:
		uses_text = " (" + str(times_used) + "/" + str(max_uses) + ")"

	trap_label.text = "Trap: %s%s" % [base_name, uses_text]


func _activate_selected_trap() -> void:
	var trap = _current_trap()
	if trap == null:
		return

	if trap.has_method("can_activate") and not trap.can_activate():
		print("Trap cannot activate: ", trap.name)
		return

	print("Activating trap: ", trap.name)
	trap.activate()


# -------------------------------------------------------------------
# PLAYER SELECTION
# -------------------------------------------------------------------

func _refresh_players() -> void:
	players = get_tree().get_nodes_in_group("player")
	if players.size() > 0:
		if selected_player_index < 0 or selected_player_index >= players.size():
			selected_player_index = 0
	else:
		selected_player_index = -1


func _current_player() -> Node2D:
	if selected_player_index < 0 or selected_player_index >= players.size():
		return null
	var p = players[selected_player_index]
	if p is Node2D:
		return p as Node2D
	return null


func _select_prev_player() -> void:
	if players.is_empty():
		return
	selected_player_index = (selected_player_index - 1 + players.size()) % players.size()
	_update_player_highlight()


func _select_next_player() -> void:
	if players.is_empty():
		return
	selected_player_index = (selected_player_index + 1) % players.size()
	_update_player_highlight()


func _update_player_highlight() -> void:
	var player := _current_player()

	# pulse only when selection changes; position is handled every frame
	if player_indicator != null and player != null:
		_pulse_indicator(player_indicator, player_indicator_base_scale)

	_update_player_hud(player)


func _update_player_hud(player: Node) -> void:
	if target_label == null:
		return

	if player == null:
		target_label.text = "Target: (none)"
		return

	target_label.text = "Target: %s" % player.name


func _update_player_indicator_follow() -> void:
	var player := _current_player()
	if player_indicator == null:
		return

	if player != null:
		player_indicator.visible = true
		player_indicator.global_position = player.global_position + Vector2(0, -40)
	else:
		player_indicator.visible = false


# -------------------------------------------------------------------
# SPAWNED TRAPS (AoE + falling block on selected player)
# -------------------------------------------------------------------

func _spawn_aoe_on_selected_player() -> void:
	var player := _current_player()
	if player == null:
		print("No selected player for AoE")
		return
	if aoe_scene == null:
		print("aoe_scene missing on DeathController")
		return

	var aoe = aoe_scene.instantiate()
	get_tree().current_scene.add_child(aoe)
	aoe.global_position = player.global_position + Vector2(0, 16)
	print("Spawned AoE at: ", aoe.global_position)


func _spawn_falling_on_selected_player() -> void:
	var player := _current_player()
	if player == null:
		print("No selected player for falling block")
		return
	if falling_block_scene == null:
		print("falling_block_scene missing on DeathController")
		return

	var block = falling_block_scene.instantiate()
	get_tree().current_scene.add_child(block)
	block.global_position = player.global_position + Vector2(0, -200)
	print("Spawned falling block at: ", block.global_position)


# -------------------------------------------------------------------
# SHARED: indicator pulse animation (uses base scale)
# -------------------------------------------------------------------

func _pulse_indicator(ind: Node2D, base_scale: Vector2) -> void:
	if ind == null:
		return

	ind.scale = base_scale

	var tween := ind.create_tween()
	tween.tween_property(ind, "scale", base_scale * 1.2, 0.15)\
		.set_trans(Tween.TRANS_SINE).set_ease(Tween.EASE_OUT)
	tween.tween_property(ind, "scale", base_scale, 0.15)\
		.set_trans(Tween.TRANS_SINE).set_ease(Tween.EASE_IN)
