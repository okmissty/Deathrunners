extends Node2D

@export var radius: float = 64.0
@export var color: Color = Color(1, 0, 0, 0.4)  # translucent red

func _draw() -> void:
	draw_circle(Vector2.ZERO, radius, color)

func _process(delta: float) -> void:
	queue_redraw()
	
