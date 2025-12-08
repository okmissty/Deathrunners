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
1. Multiplayer networking (2-4 players) — managed by `GameManager`.
2. Death role with obstacle spawning (`DeathPlayer`, `DeathController`).
3. Health and hunger systems (`Player`, `Survivor`).
4. Multiple trap types (arrow, boulder, AoE) and projectiles.
5. Pickup items (health and hunger restoration).

**Tech Stack:**
- Godot Engine 4.3+ (prototype)
- C++ (Godot GDExtension / godot-cpp)
- Make (Makefile) — simple build helper for the C++ sources

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
![alt text](image.png)

![alt text](image-1.png)

- Whoever is hosting the game would pick "Host Game" and the others would pick "Join Game". 

![alt text](image-2.png)
![alt text](image-3.png)

2. Host starts the Game! Everyone will know their roles based on the menu, and it says it again in game.
![alt text](image-4.png)
- The Death player has a different view than the other players.

3. From here the game starts, the Death player switches between traps to try and take out the survivors. While the survivors push through to the end (Heres us just an overview of our map)
![alt text](image-5.png)

4. If the survivors make it to the end goal, then the game ends and the screen says "Survivors Win" otherwise if a survivor uses all 3 lives or dies from hunger it says "Death Wins"
![alt text](image-6.png)
- Dead players become ghosts and can spectate
![alt text](image-7.png)
![alt text](image-8.png)

## Sources:

Art:
Survivor Sprite: https://jesse-m.itch.io/jungle-pack

Asset/Background/Tile credits: https://jesse-m.itch.io/jungle-pack

Menu: (We got it off pinterest dont tell anyone)


Gameplay:
Multiplayer: https://youtu.be/V4a_J38XdHk?si=EstLyZ-nUp4PxfV7








## AI tools:




