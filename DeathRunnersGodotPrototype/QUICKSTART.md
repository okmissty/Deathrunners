# Quick Start - Building Death Runners C++

## Prerequisites Check
- [ ] Godot 4.3+ installed
- [ ] Python installed
- [ ] SCons installed (`pip install scons`)
- [ ] C++ compiler (Visual Studio on Windows)

## Build Steps

### 1. Get godot-cpp (First Time Only)
```powershell
# From DeathRunnersGodotPrototype directory
git clone https://github.com/godotengine/godot-cpp.git
cd godot-cpp
git checkout 4.3
```

### 2. Build godot-cpp (First Time Only)
```powershell
# Still in godot-cpp directory
scons platform=windows target=template_debug
scons platform=windows target=template_release
cd ..
```
This takes 5-10 minutes. You'll see a lot of compilation output.

### 3. Build Death Runners Extension
```powershell
# Back in DeathRunnersGodotPrototype directory
scons platform=windows target=template_debug
```
This should be faster, ~1-2 minutes.

### 4. Open in Godot
1. Open Godot Engine
2. Import Project → Navigate to `DeathRunnersGodotPrototype`
3. Select `project.godot`
4. Click "Import & Edit"

### 5. Test
- Press F5 or click the Play button
- You should see the menu
- Test Host/Join functionality

## If You Get Errors

### "scons: command not found"
```powershell
pip install scons
# Then restart PowerShell
```

### "Cannot find godot-cpp"
Make sure you're in the right directory and godot-cpp folder exists:
```powershell
cd DeathRunnersGodotPrototype
ls godot-cpp  # Should show files
```

### "No compiler found" (Windows)
Install Visual Studio Community (free) with C++ development tools:
https://visualstudio.microsoft.com/downloads/

### "Class not found" in Godot
The extension didn't build correctly. Check:
```powershell
ls bin  # Should show .dll files (Windows)
```

## Rebuilding After Changes

Whenever you modify `.cpp` or `.h` files:
```powershell
scons platform=windows target=template_debug
```
Then reload/run the game in Godot.

## Directory Structure After Build

```
DeathRunnersGodotPrototype/
├── bin/
│   └── libdeathrunners.windows.template_debug.x86_64.dll  ← Your compiled code
├── godot-cpp/   ← You cloned this
├── src/         ← C++ source files
└── deathrunners.gdextension  ← Tells Godot where the .dll is
```

## Quick Debugging

If game doesn't work:
1. Check Godot console (bottom panel) for errors
2. Make sure the .dll exists in `bin/`
3. Try rebuilding: `scons platform=windows target=template_debug`
4. Check that scene files reference C++ classes (not .gd files)

## Getting Help

- Godot GDExtension Docs: https://docs.godotengine.org/en/stable/tutorials/scripting/gdextension/
- godot-cpp GitHub: https://github.com/godotengine/godot-cpp
- SCons Documentation: https://scons.org/documentation.html

## Success Checklist
- [ ] godot-cpp cloned and built
- [ ] Death Runners extension built (bin/ has .dll)
- [ ] Project opens in Godot without errors
- [ ] Menu screen appears when running
- [ ] Can host/join multiplayer games

Once all checked, you're ready to develop!
