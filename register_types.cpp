#include "register_types.h"
#include "core/class_db.h"
#include "aurora.h"
#include "world/aurora_world.h"
#include "render/aurora_world_renderer.h"

void register_aurora_types() {

		ClassDB::register_class<aurora::AuroraGame>();
		ClassDB::register_class<aurora::AuroraWorld>();
		ClassDB::register_class<aurora::AuroraWorldRenderer>();
}

void unregister_aurora_types() {
   //nothing to do here
}
