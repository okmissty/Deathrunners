# goal.gd - Fixed to work with your current multiplayer setup
extends Area2D

func _ready() -> void:
	# Make sure collision detection is enabled
	monitoring = true
	monitorable = true
	
	# Connect the signal if not already connected
	if not body_entered.is_connected(_on_body_entered):
		body_entered.connect(_on_body_entered)
	
	print("Goal ready at position: ", global_position)
	print("  Monitoring: ", monitoring)
	print("  Groups: ", get_groups())

func _on_body_entered(body: Node) -> void:
	print("=== GOAL COLLISION DETECTED ===")
	print("  Body name: ", body.name)
	print("  Body groups: ", body.get_groups())
	print("  Body class: ", body.get_class())
	
	# Check if it's a player/survivor
	if body.is_in_group("player"):
		print("  -> Player detected at goal!")
		
		# Try to call mark_goal_reached if it exists
		if body.has_method("mark_goal_reached"):
			body.mark_goal_reached()
			print("  -> Called mark_goal_reached on ", body.name)
		else:
			print("  -> WARNING: Body doesn't have mark_goal_reached method")
			# Fallback: directly set the property
			if "reached_goal" in body:
				body.reached_goal = true
				print("  -> Directly set reached_goal = true")
		
		# Double-check the value was set
		var reached = body.get("reached_goal")
		print("  -> Survivor reached_goal is now: ", reached)
		
		# Notify all players that someone reached the goal
		if multiplayer.has_multiplayer_peer() and is_multiplayer_authority():
			notify_goal_reached.rpc(body.name)
	else:
		print("  -> Not a player, ignoring")

@rpc("call_local", "reliable")
func notify_goal_reached(survivor_name: String):
	print("Goal reached notification: ", survivor_name, " reached the goal!")
	
	# Try to trigger win condition in main scene
	var main = get_tree().current_scene
	if main and main.has_method("_show_game_over"):
		main.call_deferred("_show_game_over", "Survivors win!")
