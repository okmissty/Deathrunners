extends Node2D

@export var speed: float = 300.0    # horizontal speed
@export var damage: float = 40.0

var active: bool = false
var used: bool = false

func _ready() -> void:
	# So DeathController can find this trap
	add_to_group("trap_boulder")
	#activate()

func activate() -> void:
	if used:
		return
	active = true
	print("Boulder trap activated at: ", global_position)

func _process(delta: float) -> void:
	if not active:
		return
	# Move to the right along the ground. Adjust direction as needed (-speed to go left)
	global_position.x += speed * delta


func _on_HitArea_body_entered(body: Node) -> void:
	if not active or used:
		return

	if body.is_in_group("player") and body.has_method("apply_damage"):
		print("Boulder hit player for ", damage)
		body.apply_damage(damage)
		used = true
		# Optionally stop the boulder or let it keep rolling
		# active = false
