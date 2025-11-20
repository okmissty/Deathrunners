extends Node2D

@export var speed: float = 300.0
@export var damage: float = 40.0
@export var lifetime: float = 4.0       # seconds boulder stays active
@export var max_uses: int = 3           # how many times Death can use this trap

var active: bool = false
var times_used: int = 0
var time_active: float = 0.0
var start_position: Vector2

func _ready() -> void:
	add_to_group("trap_boulder")
	start_position = global_position
	$HitArea.monitoring = false  # only when rolling
	print("BoulderTrap ready, groups: ", get_groups())


func can_activate() -> bool:
	return (not active) and (times_used < max_uses)

func activate() -> void:
	if not can_activate():
		return
	times_used += 1
	active = true
	time_active = 0.0
	global_position = start_position
	$HitArea.monitoring = true
	print("Boulder trap activated at: ", global_position, " use ", times_used, "/", max_uses)

func _process(delta: float) -> void:
	if not active:
		return

	global_position.x += speed * delta
	time_active += delta

	if time_active >= lifetime:
		_deactivate()

func _deactivate() -> void:
	active = false
	time_active = 0.0
	$HitArea.monitoring = false
	global_position = start_position
	print("Boulder trap deactivated")

func _on_hit_area_body_entered(body: Node2D) -> void:
	if not active:
		return
	if body.is_in_group("player") and body.has_method("apply_damage"):
		print("Boulder hit player for ", damage)
		body.apply_damage(damage)
		_deactivate()
