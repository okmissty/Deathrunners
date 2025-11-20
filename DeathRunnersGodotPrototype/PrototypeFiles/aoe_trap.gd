extends Node2D

@export var damage: float = 30.0
@export var warning_time: float = 1.0   # time before explosion
@export var active_time: float = 0.5    # time AoE deals damage

var active: bool = false
var exploded: bool = false
var timer: float = 0.0

func _ready() -> void:
	add_to_group("trap_aoe")
	# Initially invisible / inactive
	#$Area.monitoring = false
	#$Area.visible = false

func activate() -> void:
	if exploded:
		return
	active = true
	timer = 0.0
	print("AoE trap armed at: ", global_position)
	# Maybe flash / change sprite color to warning?
	$Area.visible = true

func _process(delta: float) -> void:
	if not active:
		return

	timer += delta

	if timer >= warning_time and not exploded:
		# Now "explode": enable monitoring so overlaps will be detected
		print("AoE trap exploding!")
		$Area.monitoring = true
		exploded = true

	if exploded and timer >= warning_time + active_time:
		# End of AoE effect
		print("AoE trap finished")
		active = false
		$Area.monitoring = false
		# Optionally hide or queue_free()
		queue_free()


func _on_Area_body_entered(body: Node) -> void:
	if not exploded:
		return
	if body.is_in_group("player") and body.has_method("apply_damage"):
		print("AoE hit player for ", damage)
		body.apply_damage(damage)
