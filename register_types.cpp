#include "register_types.h"
#include "core/class_db.h"
#include "aurora.h"
#include "world/aurora_world.h"
#include "render/aurora_world_renderer.h"
#include "world/aurora_world_editor.h"
#include "world/aurora_level.h"
#include "game/aurora_game_manager.h"
#include "game/aurora_game.h"

void register_aurora_types() {
		ClassDB::register_class<aurora::AuroraGameManager>();
		ClassDB::register_class<aurora::AuroraGame>();
		ClassDB::register_class<aurora::AuroraWorld>();
		ClassDB::register_class<aurora::AuroraLevel>();
		ClassDB::register_class<aurora::AuroraWorldRenderer>();
		ClassDB::register_class<aurora::AuroraWorldEditor>();
}

void unregister_aurora_types() {
   //nothing to do here
}
