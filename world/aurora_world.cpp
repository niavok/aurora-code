#include "aurora_world.h"
#include "aurora_world_editor.h"
#include "aurora_level.h"

#include "../physics/aurora_physic_gas_gas_transition.h"


namespace aurora {

AuroraWorld::AuroraWorld()
    : m_isPaused(true)
{
    //count = 0;
    printf("plop1\n");
    WorldEditor worldEditor(*this);
    worldEditor.GenerateHelloWord();

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



Level* AuroraWorld::CreateLevel(Mm minTileSize, int maxTileSubdivision, int rootTileHCount, int rootTileVCount)
{
    Level* level = new Level(minTileSize, maxTileSubdivision, rootTileHCount, rootTileVCount);
    m_levels.push_back(level);

    return level;
}

void AuroraWorld::InitPhysics()
{
    m_physicEngine.Flush();

    for(Level* level : m_levels)
    {
        for(Tile* tile : level->GetRootTiles())
        {
            m_physicEngine.AddFluidNode(&tile->GetContent()->GetGazNode());


            {
                MmRect tileLeft;

                tileLeft.position.y = tile->GetPosition().y;
                if(tile->GetPosition().x == 0)
                {
                    // Make world circular
                    tileLeft.position.x = level->GetSize().x - 1;
                }
                else
                {
                    tileLeft.position.x = tile->GetPosition().x - 1;
                }

                tileLeft.size.x = 1;
                tileLeft.size.y = tile->GetSize();


                std::vector<Tile*> tilesToConnect;
                level->FindTileAt(tilesToConnect, tileLeft);
                for(Tile* tileToConnect : tilesToConnect)
                {
                    Meter relativeAltitudeB;
                    Meter relativeAltitudeA;
                    if(tileToConnect->GetSize() < tile->GetSize())
                    {
                        relativeAltitudeB = tileToConnect->GetSize() / 2;
                        relativeAltitudeA = tileToConnect->GetPosition().y - tile->GetPosition().y + relativeAltitudeB;
                    }
                    else
                    {
                        relativeAltitudeA = tile->GetSize() / 2;
                        relativeAltitudeB = tile->GetPosition().y - tileToConnect->GetPosition().y + relativeAltitudeA;
                    }
                    
                    Meter relativeLongitudeA = 0;
                    Meter relativeLongitudeB = tileToConnect->GetSize();

                    Meter section = MIN(tileToConnect->GetSize(), tile->GetSize());
                    ConnectTiles(tile, tileToConnect, Transition::Direction(Transition::Direction::DIRECTION_LEFT), relativeAltitudeA, relativeAltitudeB, relativeLongitudeA, relativeLongitudeB, section);
                }
            }



            {
                MmRect tileBottom;
                tileBottom.position.x = tile->GetPosition().x;
                tileBottom.position.y = tile->GetPosition().y + tile->GetSize();
                tileBottom.size.x = tile->GetSize();
                tileBottom.size.y = 1;

                std::vector<Tile*> tilesToConnect;
                level->FindTileAt(tilesToConnect, tileBottom);
                for(Tile* tileToConnect : tilesToConnect)
                {
                    Meter section = MIN(tileToConnect->GetSize(), tile->GetSize());
                    
                    Meter relativeAltitudeA = tile->GetSize();
                    Meter relativeAltitudeB = 0;
                    Meter relativeLongitudeA;
                    Meter relativeLongitudeB;

                    if(tileToConnect->GetSize() < tile->GetSize())
                    {
                        relativeLongitudeB = tileToConnect->GetSize() / 2;
                        relativeLongitudeA = tileToConnect->GetPosition().x - tile->GetPosition().x + relativeLongitudeB;
                    }
                    else
                    {
                        relativeLongitudeA = tile->GetSize() / 2;
                        relativeLongitudeB = tile->GetPosition().x - tileToConnect->GetPosition().x + relativeLongitudeA;
                    }



                    ConnectTiles(tile, tileToConnect, Transition::Direction(Transition::Direction::DIRECTION_DOWN), relativeAltitudeA, relativeAltitudeB, relativeLongitudeA, relativeLongitudeB, section);
                }
            }
        }
    }

    m_physicEngine.CheckDuplicates();
}

void AuroraWorld::ConnectTiles(Tile* tileA, Tile* tileB, Transition::Direction direction, Meter relativeAltitudeA, Meter relativeAltitudeB, Meter relativeLongitudeA, Meter relativeLongitudeB, Meter section)
{
    GasGasTransition::Config config(tileA->GetContent()->GetGazNode(), tileB->GetContent()->GetGazNode());
    config.relativeAltitudeA = relativeAltitudeA;
    config.relativeAltitudeB = relativeAltitudeB;
    config.relativeLongitudeA = relativeLongitudeA;
    config.relativeLongitudeB = relativeLongitudeB;
    config.direction = direction;
    config.section = section;

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
    //for(int i = 0; i < 20 ; i++)
    {
        m_physicEngine.Step(0.1);
    }
}

//void AuroraWorld::Repack()
//{
//	for(AuroraTile* tile : m_rootTiles)
//	{
//		tile->Repack();
//	}
//}

//Rect2 AuroraWorld::GetWorldArea() const
//{
//	return m_worldArea;
//}

}

