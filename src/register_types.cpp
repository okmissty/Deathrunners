#include "register_types.h"

#include "player.h"
#include "death_player.h"
#include "obstacle.h"
#include "survivor.h"
#include "death_controller.h"
#include "hhpickup.h"
#include "aoe_trap.h"
#include "boulder_trap.h"
#include "arrow_trap.h"
#include "arrow_projectile.h"
#include "checkpoint.h"
#include "goal.h"
#include "hazard.h"
#include "main_script.h"
#include "health_pickup.h"
#include "hunger_pickup.h"
#include "horizontal_cam.h"
#include "node_2d_script.h"
#include "menu.h"
#include "game_manager.h"

#include <gdextension_interface.h>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void initialize_deathrun_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }

    ClassDB::register_class<Player>();
    ClassDB::register_class<Survivor>();
    ClassDB::register_class<DeathController>();
    ClassDB::register_class<HHPickup>();
    ClassDB::register_class<AoeTrap>();
    ClassDB::register_class<BoulderTrap>();
    ClassDB::register_class<ArrowTrap>();
    ClassDB::register_class<ArrowProjectile>();
    ClassDB::register_class<Checkpoint>();
    ClassDB::register_class<Goal>();
    ClassDB::register_class<Hazard>();
    ClassDB::register_class<MainScript>();
    ClassDB::register_class<HealthPickup>();
    ClassDB::register_class<HungerPickup>();
    ClassDB::register_class<HorizontalCam>();
    ClassDB::register_class<Node2DScript>();
    ClassDB::register_class<Menu>();
    ClassDB::register_class<DeathPlayer>();
    ClassDB::register_class<Obstacle>();
    ClassDB::register_class<GameManager>();
}

/**
 * @brief Called when the module is being removed/unloaded.
 *
 * For this project there is no special cleanup required, but the function
 * exists to satisfy the GDExtension lifecycle contract.
 */
void uninitialize_deathrun_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }
}

extern "C" {
    GDExtensionBool GDE_EXPORT deathrun_library_init(
        GDExtensionInterfaceGetProcAddress p_get_proc_address,
        GDExtensionClassLibraryPtr p_library,
        GDExtensionInitialization *r_initialization
    ) {
        godot::GDExtensionBinding::InitObject init_obj(
            p_get_proc_address, p_library, r_initialization
        );

        init_obj.register_initializer(initialize_deathrun_module);
        init_obj.register_terminator(uninitialize_deathrun_module);
        init_obj.set_minimum_library_initialization_level(
            MODULE_INITIALIZATION_LEVEL_SCENE
        );

        return init_obj.init();
    }
}