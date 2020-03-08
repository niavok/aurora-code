#include "aurora_world.h"
#include "aurora_world_editor.h"
#include "aurora_level.h"

#include "../physics/aurora_physic_gas_gas_transition.h"

#include <assert.h>

namespace aurora {

AuroraWorld::AuroraWorld()
    : m_isPaused(true)
{
    //count = 0;
    printf("plop1\n");
    WorldEditor worldEditor(*this);
    //worldEditor.GenerateTestWorld1();
    worldEditor.GenerateTestWorld2();

    InitPhysics();
}

AuroraWorld::~AuroraWorld()
{
    for(Level* level : m_levels)
    {
        delete level;
    }
}

void AuroraWorld::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_pause", "pause"), &AuroraWorld::SetPause);
    ClassDB::bind_method(D_METHOD("is_paused"), &AuroraWorld::IsPaused);
    ClassDB::bind_method(D_METHOD("step"), &AuroraWorld::Step);
}


void AuroraWorld::_notification(int p_what) {

     switch (p_what) {
        case NOTIFICATION_INTERNAL_PHYSICS_PROCESS: {
            Variant delta = get_physics_process_delta_time();
            Update(delta);
        } break;
        case NOTIFICATION_ENTER_TREE: {
            set_physics_process_internal(true);
        } break;
    }
}


void AuroraWorld::Update(Scalar delta)
{
    if(!m_isPaused)
    {
        //for(int i = 0; i < 20 ; i++)
        {
            m_physicEngine.Step(delta);
        }
    }
}

Level* AuroraWorld::CreateLevel(Meter2 levelBottomLeftPosition, Meter tileSize, Meter depth, int tileHCount, int tileVCount, bool horizontalLoop)
{
    Level* level = new Level(levelBottomLeftPosition, tileSize, depth, tileHCount, tileVCount, horizontalLoop);
    m_levels.push_back(level);

    return level;
}

void AuroraWorld::InitPhysics()
{
    m_physicEngine.Flush();

    for(Level* level : m_levels)
    {
        for(Tile* tile : level->GetTiles())
        {
            m_physicEngine.AddFluidNode(&tile->GetGazNode());
        }


        level->ForEachTransition([this](Tile* tileA, Tile* tileB, Transition::Direction direction)
        {
            // All level tile have the same size, so transition are all at the center altitude


            GasGasTransition::Config config(tileA->GetGazNode(), tileB->GetGazNode());

            if(direction == Transition::Direction::DIRECTION_RIGHT)
            {
                config.relativeAltitudeA = tileA->GetSize() / 2;
                config.relativeAltitudeB = tileB->GetSize() / 2;
                config.relativeLongitudeA = tileA->GetSize();
                config.relativeLongitudeB = 0;
            }
            else if(direction == Transition::Direction::DIRECTION_UP)
            {
                config.relativeAltitudeA = tileA->GetSize();
                config.relativeAltitudeB = 0;
                config.relativeLongitudeA = tileA->GetSize() / 2;
                config.relativeLongitudeB = tileB->GetSize() / 2;
            }
            else
            {
                assert(false); // ForEachTransition should not send other directions
            }

            config.direction = direction;
            config.section = tileA->GetSize();
            ConnectTiles(config);
        });
    }

    // TODO do this sanity check only sometime (debug command line)
    //m_physicEngine.CheckDuplicates();
}

void AuroraWorld::ConnectTiles(GasGasTransition::Config const& config)
{
    Transition* transition = new GasGasTransition(config);
    m_physicEngine.AddTransition(transition);
}

bool AuroraWorld::IsPaused()
{
    return m_isPaused;
}

void AuroraWorld::SetPause(bool pause)
{
    m_isPaused = pause;
}

void AuroraWorld::Step()
{
    m_physicEngine.Step(0.1);
}

//Rect2 AuroraWorld::GetWorldArea() const
//{
//	return m_worldArea;
//}

}

