extends Area2D

@export var fall_speed: float = 400.0
@export var damage: float = 30.0
@export var lifetime: float = 5.0
@export var warning_time: float = 0.5  # Time before it starts falling

var time_alive: float = 0.0
var is_falling: bool = false

func _ready() -> void:
	monitoring = false
	# Warning visual
	modulate = Color(1, 0.5, 0.5, 0.7)

func _process(delta: float) -> void:
	time_alive += delta
	
	# Warning phase - blink
	if time_alive < warning_time:
		modulate.a = 0.4 + 0.3 * sin(time_alive * 15)
		return
	
	# Start falling after warning
	if not is_falling:
		is_falling = true
		monitoring = true
		modulate = Color(0.7, 0.2, 0.2, 1)
	
	# Fall down
	global_position.y += fall_speed * delta
	
	# Auto-destroy after lifetime
	if time_alive >= lifetime:
		queue_free()

func _on_body_entered(body: Node) -> void:
	if not is_falling:
		return
		
	print("Hazard hit: ", body.name)
	
	if body.is_in_group("player") and body.has_method("apply_damage"):
		print("Falling block hit player for ", damage)
		body.apply_damage(damage)
		queue_free()
