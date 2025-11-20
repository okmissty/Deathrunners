extends Node2D

@export var survivor_path: NodePath
@export var end_label_path: NodePath

var survivor
var end_label: Label
var game_over: bool = false

func _ready() -> void:
	survivor = get_node(survivor_path)
	end_label = get_node(end_label_path)
	end_label.text = ""

func _process(delta: float) -> void:
	if game_over or survivor == null:
		return

	if not survivor.alive:
		_show_game_over("Death wins!")
	elif survivor.reached_goal:
		_show_game_over("Survivor wins!")

func _show_game_over(text: String) -> void:
	game_over = true
	end_label.text = text
	get_tree().paused = true
