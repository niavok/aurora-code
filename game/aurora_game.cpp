#include "aurora_game.h"

namespace aurora {

AuroraGame::AuroraGame()
{
    printf("new AuroraGame %p\n", this);
	m_world.instance();
}

AuroraGame::~AuroraGame()
{
	printf("delete AuroraGame %p\n", this);
}

void AuroraGame::_bind_methods() {

	ClassDB::bind_method(D_METHOD("save"), &AuroraGame::Save);

	ClassDB::bind_method(D_METHOD("get_world"), &AuroraGame::GetWorld);

    // ClassDB::bind_method(D_METHOD("set_pause", "pause"), &AuroraWorld::SetPause);
    // ClassDB::bind_method(D_METHOD("is_paused"), &AuroraWorld::IsPaused);
    // ClassDB::bind_method(D_METHOD("step"), &AuroraWorld::Step);

    // ClassDB::bind_method(D_METHOD("get_last_update_duration"), &AuroraWorld::get_last_update_duration);
}


Ref<AuroraWorld> AuroraGame::GetWorld()
{
	return m_world;
}


void AuroraGame::Save()
{
	printf("AuroraGame::Save\n");
}

}