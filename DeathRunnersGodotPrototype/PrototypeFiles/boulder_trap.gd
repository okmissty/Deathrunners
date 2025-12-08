extends Node2D

@export var speed: float = 300.0
@export var damage: float = 20.0
@export var lifetime: float = 4.0
@export var max_uses: int = 3

var active: bool = false
var times_used: int = 0
var time_active: float = 0.0
var start_position: Vector2

@onready var hit_area = $HitArea

func _ready() -> void:
	add_to_group("trap_boulder")
	start_position = global_position
	
	# Failsafe: Create shape if missing
	var has_shape = false
	for child in hit_area.get_children():
		if child is CollisionShape2D and child.shape != null:
			has_shape = true
			break
	if not has_shape:
		var shape_node = CollisionShape2D.new()
		var circle = CircleShape2D.new()
		circle.radius = 40.0
		shape_node.shape = circle
		hit_area.add_child(shape_node)

	# Connect signal if needed
	if not hit_area.body_entered.is_connected(_on_hit_area_body_entered):
		hit_area.body_entered.connect(_on_hit_area_body_entered)
	
	# Layer 1 + 2
	hit_area.collision_mask = 3
	hit_area.monitoring = false
	
	print("BoulderTrap ready")

func can_activate() -> bool:
	return (not active) and (times_used < max_uses)

func activate() -> void:
	if not can_activate():
		return
	times_used += 1
	active = true
	time_active = 0.0
	global_position = start_position
	
	hit_area.set_deferred("monitoring", true)
	print("Boulder trap activated")

func _physics_process(delta: float) -> void:
	if not active:
		return

	global_position.x += speed * delta
	time_active += delta

	if time_active >= lifetime:
		_deactivate()

func _deactivate() -> void:
	active = false
	time_active = 0.0
	hit_area.set_deferred("monitoring", false)
	global_position = start_position

func _on_hit_area_body_entered(body: Node) -> void:
	if not active:
		return
	
	if body.is_in_group("player"):
		print("Boulder HIT: ", body.name)
		
		# FIX: Only the SERVER triggers the damage logic.
		# This prevents double damage.
		if multiplayer.is_server() and body.has_method("apply_damage"):
			body.apply_damage(damage)
		
		# Everyone deactivates the boulder visually
		_deactivate()
