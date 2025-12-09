# Build Instructions for Death Runners C++ GDExtension

## Overview

This project has been refactored to run entirely on C++ code using Godot's GDExtension system. The original GDScript files in `PrototypeFiles/` have been ported to C++ implementations in the `src/` directory.

## Prerequisites

1. **Godot 4.3+** - Download from [https://godotengine.org/](https://godotengine.org/)
2. **SCons** - Build system for compiling the GDExtension
   - Install via pip: `pip install scons`
   - Or via system package manager
3. **C++ Compiler**
   - Windows: MSVC (Visual Studio) or MinGW-w64
   - Linux: GCC or Clang
   - macOS: Xcode Command Line Tools
4. **godot-cpp** - C++ bindings for Godot (submodule)

## Setup

### 1. Initialize godot-cpp

If you haven't cloned with submodules, you need to get godot-cpp:

```bash
cd DeathRunnersGodotPrototype
git clone https://github.com/godotengine/godot-cpp.git
cd godot-cpp
git checkout 4.3
cd ..
```

### 2. Build godot-cpp

First, build the godot-cpp library:

```bash
cd godot-cpp
scons platform=<platform> target=template_debug
scons platform=<platform> target=template_release
cd ..
```

Replace `<platform>` with:

- `windows` for Windows
- `linux` for Linux
- `macos` for macOS

### 3. Build the GDExtension

From the `DeathRunnersGodotPrototype` directory:

```bash
# Debug build
scons platform=<platform> target=template_debug

# Release build
scons platform=<platform> target=template_release
```

This will create the compiled library in the `bin/` directory.

## Project Structure

```text
DeathRunnersGodotPrototype/
├── src/                       # C++ source files
│   ├── main_script.cpp/.h     # Main game logic
│   ├── menu.cpp/.h            # Multiplayer menu
│   ├── player.cpp/.h          # Survivor player
│   ├── death_controller.cpp/.h
│   ├── checkpoint.cpp/.h
│   ├── goal.cpp/.h
│   ├── hazard.cpp/.h
│   ├── aoe_trap.cpp/.h
│   ├── arrow_trap.cpp/.h
│   ├── boulder_trap.cpp/.h
│   ├── arrow_projectile.cpp/.h
│   ├── health_pickup.cpp/.h
│   ├── hunger_pickup.cpp/.h
│   ├── hhpickup.cpp/.h
│   ├── horizontal_cam.cpp/.h
│   └── register_types.cpp/.h  # GDExtension registration
├── PrototypeFiles/            # Original GDScript (for reference)
├── bin/                       # Compiled GDExtension libraries
├── deathrunners.gdextension   # GDExtension configuration
├── SConstruct                 # Build configuration
└── project.godot              # Godot project file
```

## C++ Classes

All game logic has been ported to C++ and registered as GDExtension classes:

### Core Game Classes

- **MainScript** - Main game coordinator, spawns players, tracks game state
- **Menu** - Multiplayer lobby with role assignment
- **Player** - Survivor character with health/hunger/movement
- **DeathController** - Death player controller for spawning obstacles

### Game Objects

- **Checkpoint** - Respawn points for players
- **Goal** - Win condition for survivors
- **Hazard** - Static danger zones
- **HHPickup** - Combined health/hunger pickup
- **HealthPickup** - Restores player health
- **HungerPickup** - Restores player hunger

### Traps & Obstacles

- **AoeTrap** - Area of effect damage trap
- **ArrowTrap** - Shoots arrow projectiles
- **ArrowProjectile** - Arrow projectile entity
- **BoulderTrap** - Falling boulder obstacle

### Utilities

- **HorizontalCam** - Camera controller

## Scene Files

All `.tscn` files have been updated to reference the C++ classes:

- Script paths changed from `res://PrototypeFiles/*.gd` to `res://ClassName`
- This tells Godot to use the registered GDExtension classes

## Running the Game

1. Open the project in Godot Editor
2. The editor will automatically load the GDExtension
3. Press F5 to run, or use the Play button

If you get errors about missing classes, ensure:

- The GDExtension is compiled (check `bin/` directory)
- The `deathrunners.gdextension` file exists
- The library path in `.gdextension` matches your compiled file

## Development Workflow

1. Make changes to C++ files in `src/`
2. Rebuild: `scons platform=<platform> target=template_debug`
3. Reload the Godot project (or just run it)
4. Test changes

**Note**: Godot 4.3+ supports hot-reloading of GDExtensions when `reloadable = true` is set in the `.gdextension` file.

## Troubleshooting

### "Cannot find library" error

- Make sure you built the GDExtension for your platform
- Check the `bin/` directory for the compiled library
- Verify the path in `deathrunners.gdextension` matches

### "Class not found" errors

- Ensure all classes are registered in `register_types.cpp`
- Check that `ClassDB::register_class<ClassName>()` is called

### Build errors

- Ensure godot-cpp is built first
- Check that your compiler is in the PATH
- Verify SCons is installed correctly

## Testing Multiplayer

1. Build and run the game
2. Click "Host" on one instance
3. Note the IP address (or use localhost/127.0.0.1)
4. Run another instance, enter IP, click "Join"
5. When ready, host clicks "Start Game"
6. One player will be "Death", others are "Survivors"

## Academic Notes

This project demonstrates:

- C++ game development with Godot
- GDExtension system usage
- Multiplayer networking in C++
- Object-oriented game architecture
- Resource management and memory safety
- Build system configuration (SCons)

All original GDScript logic has been faithfully ported to C++ while maintaining the same game behavior and multiplayer functionality.
