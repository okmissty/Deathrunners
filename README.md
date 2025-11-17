# Deathrunners

Software Systems Final Project

**Creators:** Claire Wu, Tyeon Ford, Andy Quach

**Description:** 

A 2D multiplayer scroller survival game based in a X themed environment where one player is chosen to be “Death” and tries to take out all of the other players by throwing obstacles/enemies/silly inconveniences in their way to prevent them from getting to safety.

For everyone else the task is to survive till the end!

**Features**
1. Multiplayer networking (2-4 players)
2. Death role with obstacle spawning
3. Health system with damage
4. Hunger system
5. Healing items and food pickups

**Tech Stack**
- Godot Engine 4.3+
- GCC/Clang C++ compiler
- SCons build system
- Python 3.x

**Key Classes**
- **Player**: Survivor character with health/hunger
- **DeathPlayer**: Controls obstacle spawning
- **Obstacle**: Base class for hazards
- **GameManager**: Multiplayer and game state management

# Gameplay



### Survivors
- **A / Left Arrow**: Move left
- **D / Right Arrow**: Move right
- **Space / W**: Jump

### Death Player
- **Mouse Click**: Spawn obstacle
- **1-3 Keys**: Select obstacle type
**Features:**

- Multiplayer
- “Death” spawning in objects in the environment 
- Players have health and hunger bars
- Players can lose health, heal, and be taken out
- Games end when all survivors reach the goal or are dead.

**Project Timeline:**

- Week 1 (Nov 3-7): Decide theme of project, decide user group, finish project proposal
- Week 2 (Nov 8-14): Set up basic game structure and multiplayer connection
- Week 3 (Nov 15-21): Add “Death” abilities and player health system, finish prototype, and collect first round of user feedback
- Week 4 (Nov 22-28): Add hunger bar and healing items
- Week 5 (Nov 29-Dec 8): Polish UI, fix bugs, finalize project, collect second user feedback, prepare presentation
