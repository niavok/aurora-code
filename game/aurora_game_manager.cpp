#include "aurora_game_manager.h"

namespace aurora {

AuroraGameManager::AuroraGameManager()
{
    printf("AuroraGameManager\n");
}

AuroraGameManager::~AuroraGameManager()
{
}

Ref<AuroraGame> AuroraGameManager::GetGame()
{
	return m_game;
}

bool AuroraGameManager::HasGame()
{
	return m_game.is_valid();
}

void AuroraGameManager::CreateGame()
{
	m_game.instance();
}

void AuroraGameManager::_bind_methods() {

	ClassDB::bind_method(D_METHOD("has_game"), &AuroraGameManager::HasGame);
	ClassDB::bind_method(D_METHOD("get_game"), &AuroraGameManager::GetGame);
	ClassDB::bind_method(D_METHOD("create_game"), &AuroraGameManager::CreateGame);

	/*ClassDB::bind_method(D_METHOD("set_pause", "pause"), &AuroraWorld::SetPause);
    ClassDB::bind_method(D_METHOD("is_paused"), &AuroraWorld::IsPaused);
    ClassDB::bind_method(D_METHOD("step"), &AuroraWorld::Step);



    ClassDB::bind_method(D_METHOD("get_last_update_duration"), &AuroraWorld::get_last_update_duration);*/
}


void AuroraGameManager::_notification(int p_what) {
/*
     switch (p_what) {
        case NOTIFICATION_INTERNAL_PHYSICS_PROCESS: {
            Variant delta = get_physics_process_delta_time();
            Update(delta);
        } break;
        case NOTIFICATION_ENTER_TREE: {
            set_physics_process_internal(true);
        } break;
    }*/
}

}

