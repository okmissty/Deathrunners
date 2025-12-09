# Deathrunners

Software Systems Final Project

**Creators:** Tyeon Ford, Andy Quach, Claire Wu

**Description:**

Deathrunners is a 2D multiplayer side-scroller survival game. One player is assigned the role of "Death" and spawns hazards and obstacles to stop the other players (survivors) from reaching safety. Survivors manage health and hunger while navigating hazards and attempting to reach the level's goal!

**Project layout**:

- `DeathRunnersGodotPrototype/` — Original Godot prototype (GDScript, scenes, assets).
- `src/` — C++ conversion of game nodes and gameplay logic (GDExtension/Godot C++ skeletons).
- `Makefile` — Build script for C++ components (requires Godot headers / `godot-cpp` to be configured).
- `User_surveys.txt` — Collected user feedback and prototype/final surveys.
- `Changlog.txt` — Project changelog generated from commit history.
- `UML.puml` — UML diagram representing classes and relationships.

**Features implemented:**
1. Multiplayer (2-4 players) — managed by `GameManager`.
2. Death role with obstacle spawning (`DeathPlayer`, `DeathController`).
3. Health and hunger systems (`Player`, `Survivor`).
4. Multiple trap types (arrow, boulder, AoE) and projectiles with selection.
5. Pickup items (health and hunger restoration).
6. Checkpoints

**Tech Stack:**
- Godot Engine 4.3+ (prototype)
- C++ (Godot GDExtension / godot-cpp)

## How to build and run:

1. Ensure you have the Godot C++ bindings (`godot-cpp`) built and available. Follow Godot's GDExtension instructions if needed.
2. From the project root, run:

```powershell
make
```

3. Open `DeathRunnersGodotPrototype/project.godot` in Godot 4.3+ and add the compiled GDExtension if you want to run the C++ nodes in the engine.

Notes: the `src/` folder is the C++ logic converted from the Godot prototype to meet the project requirement. Some behavior is still implemented or tested in the Godot gd script scenes.

## Example usage/Screenshots
To play run the Godot prototype in `DeathRunnersGodotPrototype/`


1. Menu Sreen (Then a 4 player example)
![alt text](Screenshots\image.png)

![alt text](Screenshots\image-1.png)

- Whoever is hosting the game would pick "Host Game" and the others would pick "Join Game". 

![alt text](Screenshots\image-2.png)
![alt text](Screenshots\image-3.png)

2. Host starts the Game! Everyone will know their roles based on the menu, and it says it again in game.
![alt text](Screenshots\image-4.png)
- The Death player has a different view than the other players.

3. From here the game starts, the Death player switches between traps to try and take out the survivors. While the survivors push through to the end (Heres us just an overview of our map)
![alt text](Screenshots\image-5.png)

4. If the survivors make it to the end goal, then the game ends and the screen says "Survivors Win" otherwise if a survivor uses all 3 lives or dies from hunger it says "Death Wins"

![alt text](Screenshots\image-6.png)

- Dead players become ghosts and can spectate

![alt text](Screenshots\image-7.png)

![alt text](Screenshots\image-8.png)

## Sources:

**Art:**

- Survivor Sprite: https://jesse-m.itch.io/jungle-pack

- Asset/Background/Tile credits: https://jesse-m.itch.io/jungle-pack

- Menu picture: (We got it off pinterest)

- Music: Tyeon


**Godot Tutorials/Videos:**
- Multiplayer: https://youtu.be/V4a_J38XdHk?si=EstLyZ-nUp4PxfV7
- Multiplayer (helped a lot ): https://docs.godotengine.org/en/stable/tutorials/networking/high_level_multiplayer.html

- How to make a video game Godot Beginner Tutorial: https://youtu.be/LOhfqjmasi0?si=jVWSm0eqk360wgwt
 
- Godot for absolute beginners: https://youtu.be/s0O3a2AgoBA?si=jm-VWv2QUcttzJ1O








## AI tools:




