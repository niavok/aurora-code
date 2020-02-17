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
    worldEditor.GenerateTestWorld1();
    //worldEditor.GenerateTestWorld2();

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



Level* AuroraWorld::CreateLevel(bool horizontalLoop, Mm minTileSize, int maxTileSubdivision, int rootTileHCount, int rootTileVCount)
{
    Level* level = new Level(horizontalLoop, minTileSize, maxTileSubdivision, rootTileHCount, rootTileVCount);
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

                tileLeft.position.y = tile->GetPositionMm().y;
                if(tile->GetPositionMm().x == 0 && level->IsHorizontalLoop())
                {
                    // Make world circular
                    tileLeft.position.x = level->GetSizeMm().x - 1;
                }
                else
                {
                    tileLeft.position.x = tile->GetPositionMm().x - 1;
                }

                tileLeft.size.x = 1;
                tileLeft.size.y = tile->GetSizeMm();


                std::vector<Tile*> tilesToConnect;
                level->FindTileAt(tilesToConnect, tileLeft);
                for(Tile* tileToConnect : tilesToConnect)
                {
                    Mm relativeAltitudeBMm;
                    Mm relativeAltitudeAMm;
                    if(tileToConnect->GetSizeMm() < tile->GetSizeMm())
                    {
                        relativeAltitudeBMm = tileToConnect->GetSizeMm() / 2;
                        relativeAltitudeAMm = tileToConnect->GetPositionMm().y - tile->GetPositionMm().y + relativeAltitudeBMm;
                    }
                    else
                    {
                        relativeAltitudeAMm = tile->GetSizeMm() / 2;
                        relativeAltitudeBMm = tile->GetPositionMm().y - tileToConnect->GetPositionMm().y + relativeAltitudeAMm;
                    }

                    Meter relativeLongitudeAMm = 0;
                    Meter relativeLongitudeBMm = tileToConnect->GetSizeMm();

                    Meter relativeAltitudeB = MmToMeter(relativeAltitudeBMm);
                    Meter relativeAltitudeA = MmToMeter(relativeAltitudeAMm);
                    Meter relativeLongitudeA = MmToMeter(relativeLongitudeAMm);
                    Meter relativeLongitudeB = MmToMeter(relativeLongitudeBMm);

                    Meter section = MmToMeter(MIN(tileToConnect->GetSizeMm(), tile->GetSizeMm()));
                    
                    if(tileToConnect->GetPositionMm().y < 500 ||  tileToConnect->GetPositionMm().y > 3500)
                    {
                        ConnectTiles(tile, tileToConnect, Transition::Direction(Transition::Direction::DIRECTION_LEFT), relativeAltitudeA, relativeAltitudeB, relativeLongitudeA, relativeLongitudeB, section);
                    }
                }
            }



            {
                MmRect tileBottom;
                tileBottom.position.x = tile->GetPositionMm().x;
                tileBottom.position.y = tile->GetPositionMm().y + tile->GetSizeMm();
                tileBottom.size.x = tile->GetSizeMm();
                tileBottom.size.y = 1;

                std::vector<Tile*> tilesToConnect;
                level->FindTileAt(tilesToConnect, tileBottom);
                for(Tile* tileToConnect : tilesToConnect)
                {
                    Meter section = MmToMeter(MIN(tileToConnect->GetSizeMm(), tile->GetSizeMm()));

                    Mm relativeAltitudeAMm = tile->GetSizeMm();
                    Mm relativeAltitudeBMm = 0;
                    Mm relativeLongitudeAMm;
                    Mm relativeLongitudeBMm;

                    if(tileToConnect->GetSizeMm() < tile->GetSizeMm())
                    {
                        relativeLongitudeBMm = tileToConnect->GetSizeMm() / 2;
                        relativeLongitudeAMm = tileToConnect->GetPositionMm().x - tile->GetPositionMm().x + relativeLongitudeBMm;
                    }
                    else
                    {
                        relativeLongitudeAMm = tile->GetSizeMm() / 2;
                        relativeLongitudeBMm = tile->GetPositionMm().x - tileToConnect->GetPositionMm().x + relativeLongitudeAMm;
                    }

                    Meter relativeAltitudeB = MmToMeter(relativeAltitudeBMm);
                    Meter relativeAltitudeA = MmToMeter(relativeAltitudeAMm);
                    Meter relativeLongitudeA = MmToMeter(relativeLongitudeAMm);
                    Meter relativeLongitudeB = MmToMeter(relativeLongitudeBMm);

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

