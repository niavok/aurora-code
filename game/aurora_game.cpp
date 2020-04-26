#include "aurora_game.h"

namespace aurora {

AuroraGame::AuroraGame()
	: m_isPaused(true)
{
	printf("new AuroraGame %p\n", this);
	m_world.instance();
	m_worldEditor.instance();
	m_worldEditor->SetWorld(m_world);
}

AuroraGame::~AuroraGame()
{
	printf("delete AuroraGame %p\n", this);
}

void AuroraGame::_bind_methods() {

	ClassDB::bind_method(D_METHOD("save"), &AuroraGame::Save);

	ClassDB::bind_method(D_METHOD("get_world"), &AuroraGame::GetWorld);
	ClassDB::bind_method(D_METHOD("get_world_editor"), &AuroraGame::GetWorldEditor);

    // ClassDB::bind_method(D_METHOD("set_pause", "pause"), &AuroraWorld::SetPause);
    // ClassDB::bind_method(D_METHOD("is_paused"), &AuroraWorld::IsPaused);
    // ClassDB::bind_method(D_METHOD("step"), &AuroraWorld::Step);

    // ClassDB::bind_method(D_METHOD("get_last_update_duration"), &AuroraWorld::get_last_update_duration);
}


Ref<AuroraWorld> AuroraGame::GetWorld()
{
	return m_world;
}

Ref<AuroraWorldEditor> AuroraGame::GetWorldEditor()
{
	return m_worldEditor;
}


void AuroraGame::Save()
{
	printf("AuroraGame::Save\n");
}

}
