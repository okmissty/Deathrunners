extends Area2D

@export var fall_speed: float = 400.0
@export var damage: float = 20.0
@export var lifetime: float = 5.0
@export var warning_time: float = 0.5

var time_alive: float = 0.0
var is_falling: bool = false

func _ready() -> void:
	monitoring = false
	# Detect Survivors (Layer 2) and World (Layer 1)
	collision_mask = 3 
	modulate = Color(1, 0.5, 0.5, 0.7)
	print("[Hazard] Spawned at ", global_position)

func _physics_process(delta: float) -> void:
	time_alive += delta
	
	# Phase 1: Warning
	if time_alive < warning_time:
		var wiggle = sin(time_alive * 20.0) * 2.0
		position.x += wiggle * delta
		modulate.a = 0.4 + 0.3 * sin(time_alive * 15)
		return # Stop here, don't fall yet
	
	# Phase 2: Falling
	if not is_falling:
		is_falling = true
		monitoring = true
		modulate = Color(0.7, 0.2, 0.2, 1)
		print("[Hazard] Warning over - Falling now!")
	
	# Gravity Movement
	global_position.y += fall_speed * delta
	
	# Auto-destroy
	if time_alive >= lifetime:
		print("[Hazard] Lifetime expired - Despawning")
		queue_free()

func _on_body_entered(body: Node) -> void:
	if not is_falling:
		return
		
	if body.is_in_group("player"):
		# Server-Only Damage
		if multiplayer.is_server() and body.has_method("apply_damage"):
			print("[Hazard] Hit Survivor: ", body.name, " -> Dealing ", damage, " damage")
			body.apply_damage(damage)
		else:
			print("[Hazard] Hit Survivor: ", body.name, " (Visual only)")
			
		queue_free()
		
	elif body is TileMap or body is StaticBody2D:
		print("[Hazard] Hit Ground - Destroyed")
		queue_free()
