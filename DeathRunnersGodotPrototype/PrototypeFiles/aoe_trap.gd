extends Node2D

@export var damage: float = 30.0
@export var warning_time: float = 1.0   # seconds before explosion
@export var active_time: float = 0.5    # seconds during which it deals damage
@export var max_uses: int = 3

var active: bool = false
var exploded: bool = false
var times_used: int = 0
var timer: float = 0.0


func _ready() -> void:
	add_to_group("trap_aoe")
	$Area.monitoring = false
	$Area.visible = false   # hide until armed
	print("AoE Trap ready, groups: ", get_groups())

func can_activate() -> bool:
	return (not active) and (times_used < max_uses)

func activate() -> void:
	if not can_activate():
		return
	times_used += 1
	active = true
	exploded = false
	timer = 0.0
	$Area.visible = true
	print("AoE trap armed at: ", global_position, " use ", times_used, "/", max_uses)

func _process(delta: float) -> void:
	if not active:
		return

	timer += delta

	if not exploded and timer >= warning_time:
		exploded = true
		$Area.monitoring = true
		print("AoE trap exploding!")

	elif exploded and timer >= warning_time + active_time:
		_deactivate()

func _deactivate() -> void:
	active = false
	exploded = false
	timer = 0.0
	$Area.monitoring = false
	$Area.visible = false
	print("AoE trap finished")

func _on_area_body_entered(body: Node2D) -> void:
	if not exploded:
		return
	if body.is_in_group("player") and body.has_method("apply_damage"):
		print("AoE hit player for ", damage)
		body.apply_damage(damage)
		
