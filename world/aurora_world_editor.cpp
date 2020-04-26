#include "aurora_world_editor.h"
#include "aurora_level.h"

#include <assert.h>

#include "../physics/aurora_physic_constants.h"
#include "../physics/aurora_physic_types.h"


namespace aurora {

AuroraWorldEditor::AuroraWorldEditor()
    : m_world(nullptr)
{
}

AuroraWorldEditor::~AuroraWorldEditor()
{
}

// void AuroraWorldEditor::set_world(Node* p_world) {
//     assert(p_world->get_class() == AuroraWorld::get_class_static());
// 	m_world = static_cast<AuroraWorld*>(p_world);
// }

Ref<AuroraWorld> AuroraWorldEditor::get_world() const {
	return m_world;
}


// void AuroraWorldEditor::set_world(const Ref<AuroraWorld> &p_world) {
// 	m_testWorld = p_world;
// }

// Ref<AuroraWorld> AuroraWorldEditor::get_world() const {
// 	return m_testWorld;
// }

void AuroraWorldEditor::_bind_methods() {
    // ClassDB::bind_method(D_METHOD("set_pause", "pause"), &AuroraWorld::SetPause);
    // ClassDB::bind_method(D_METHOD("is_paused"), &AuroraWorld::IsPaused);
    // ClassDB::bind_method(D_METHOD("step"), &AuroraWorld::Step);

    // ClassDB::bind_method(D_METHOD("get_last_update_duration"), &AuroraWorld::get_last_update_duration);

    //ClassDB::bind_method(D_METHOD("set_world", "world"), &AuroraWorldEditor::set_world);
	ClassDB::bind_method(D_METHOD("get_world"), &AuroraWorldEditor::get_world);

	//ClassDB::bind_method(D_METHOD("set_world_node", "node"), &AuroraWorldEditor::set_world_node);
	//ClassDB::bind_method(D_METHOD("get_world_node"), &AuroraWorldEditor::get_world_node);

	//ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "world_node", PROPERTY_HINT_NODE_PATH_VALID_TYPES, "AuroraWorld"), "set_world_node", "get_world_node");
    //ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "world", PROPERTY_HINT_RESOURCE_TYPE, "World"), "set_world", "get_world");
}


void AuroraWorldEditor::_notification(int p_what) {

	switch (p_what) {
    //     case NOTIFICATION_INTERNAL_PHYSICS_PROCESS: {
    //         Variant delta = get_physics_process_delta_time();
    //         Update(delta);
    //     } break;
		case NOTIFICATION_READY: {
			/*if (has_node(m_targetWorldPath)) {
				m_world = Object::cast_to<AuroraWorld>(get_node(m_targetWorldPath));
			}
			else
			{
				printf("set_world_node fail to find node %ls\n", String(m_targetWorldPath).c_str());
				return;
			}*/

		} break;
	}
}

/*void AuroraWorldEditor::set_world_node(const NodePath &p_node)
{
	m_targetWorldPath = p_node;
	m_world = nullptr;
}

NodePath AuroraWorldEditor::get_world_node()
{
	return m_targetWorldPath;
}*/

void AuroraWorldEditor::GenerateTestWorld1()
{
    int blockCountX = 2;
    int blockCountY = 80;
    Meter tileSize = 0.050;
    Meter2 levelPosition(0, 0);
    Meter levelDepth = 100;

    Level* surfaceLevel = m_world->CreateLevel(levelPosition, tileSize, levelDepth, blockCountX, blockCountY, false); // 20 x 1 blocks * 50 mm * 2^ 1 = 1 m x 1 m
    Meter surfaceWidth = surfaceLevel->GetLevelSize().x;
    Meter surfaceHeight = surfaceLevel->GetLevelSize().y;

    TileComposition dryAir;
    // No solid
    // No liquid
    // Nitrogen 80%, Oxygen 20%
    // Temperature 125K
    // Pressure 1 bar
    dryAir.Gas.composition[Gas::Nitrogen] = 80;
    dryAir.Gas.composition[Gas::Oxygen] = 20;
    dryAir.Gas.pressure = 100000. ; // 1 bar
    dryAir.Gas.temperature = 274.; // 0 degree C
    PaintTiles(surfaceLevel,  dryAir,  MeterRect(0,0, surfaceWidth,surfaceHeight));

    TileComposition highPressureDryAir = dryAir;
    highPressureDryAir.Gas.pressure = 100000 * 1. ;
    highPressureDryAir.Gas.temperature = 274. + 500 *  0.5;
    PaintTiles(surfaceLevel, highPressureDryAir,  MeterRect(0, 30 * surfaceHeight/ blockCountY, surfaceWidth/ blockCountX, 30 * surfaceHeight/ blockCountY));
}

void AuroraWorldEditor::GenerateTestWorld2()
{
    //Level* surfaceLevel = m_world.CreateLevel(50, 8, 1, 1); // 1 x 1 blocks * 50 mm * 2^ 8 = 12.8 m x 12.8 m
    //Level* surfaceLevel = m_world.CreateLevel(50, 0, 256, 256); // 256 x 256 blocks * 50 mm * 2^ 1 = 12.8 m x 12.8 m
    int blockCountX = 1000;
    int blockCountY = 20;
    Meter tileSize = 0.05;
    Meter2 levelPosition(0, 0);
    Meter levelDepth = 100;

    Level* surfaceLevel = m_world->CreateLevel(levelPosition, tileSize, levelDepth, blockCountX, blockCountY, true); // 20 x 1 blocks * 50 mm * 2^ 1 = 1 m x 1 m


    TileComposition dryAir;
    // No solid
    // No liquid
    // Nitrogen 80%, Oxygen 20%
    // Temperature 125K
    // Pressure 1 bar
    dryAir.Gas.composition[Gas::Nitrogen] = 80;
    dryAir.Gas.composition[Gas::Oxygen] = 20;
    dryAir.Gas.pressure = 100000. * 1; // 1 bar
    dryAir.Gas.temperature = 274. * 0.1; // 0 degree C

    TileComposition steam;
    // No solid
    // No liquid
    // Nitrogen 80%, Oxygen 20%
    // Temperature 125K
    // Pressure 1 bar
    steam.Gas.composition[Gas::Water] = 100;
    steam.Gas.pressure = 100000. * 0.1; // 1 bar
    steam.Gas.temperature = 274.; // 0 degree C

    // Paint hot air tile
    TileComposition hotDryAir = dryAir;
    hotDryAir.Gas.temperature = 4000;
    hotDryAir.Gas.pressure = 100000*0.1;


    Meter surfaceWidth = surfaceLevel->GetLevelSize().x;
    Meter surfaceHeight = surfaceLevel->GetLevelSize().y;
    PaintTiles(surfaceLevel, dryAir,  MeterRect(0,0, surfaceWidth,surfaceHeight));
    //PaintTiles(surfaceLevel, hotDryAir,  MmRect(0,0, surfaceWidth,surfaceHeight));


    //PaintTiles(surfaceLevel, hotDryAir,  MmRect(surfaceWidth / 4, surfaceHeight - surfaceHeight / 6, surfaceWidth / 6,surfaceHeight /8));

    TileComposition highPressureDryAir = dryAir;
    highPressureDryAir.Gas.pressure = 100000 * 200;
    highPressureDryAir.Gas.temperature = 274.+2000;
    PaintTiles(surfaceLevel, highPressureDryAir,  MeterRect(surfaceWidth / 4, 1*surfaceHeight/3, tileSize * 4, tileSize * 4));
    //PaintTiles(surfaceLevel, highPressureDryAir,  MmRect(0, 3*surfaceHeight/4, surfaceWidth-5* tileSizeMm, surfaceHeight / 4));
}


void AuroraWorldEditor::GenerateTestWorld3()
{
    int blockCountX = 50;
    int blockCountY = 20;
    Meter tileSize = 0.05;
    Meter2 levelPosition(0, 0);
    Meter levelDepth = 100;

    Level* surfaceLevel = m_world->CreateLevel(levelPosition, tileSize, levelDepth, blockCountX, blockCountY, true); // 20 x 1 blocks * 50 mm * 2^ 1 = 1 m x 1 m
    //cave1 = m_world.CreateLevel(64, 1,1); // 1 x 1 block = 64 m x 64 m


    TileComposition dryAir;
    // No solid
    // No liquid
    // Nitrogen 80%, Oxygen 20%
    // Temperature 125K
    // Pressure 1 bar
    dryAir.Gas.composition[Gas::Nitrogen] = 80;
    dryAir.Gas.composition[Gas::Oxygen] = 20;
    dryAir.Gas.pressure = 1.f;
    dryAir.Gas.temperature = 125.f;

    TileComposition saltWater;
    // Water 100% of water with 10% of salt, 115 K, at 1 bar
    saltWater.AddLiquidVolume(Liquid::Water, 1.f, 0.1f , 115, 1);


    TileComposition clayRock;
    clayRock.porosity = 0;
    clayRock.solidTemperature = 120;
    clayRock.AddSolidVolume(Solid::Clay, 1000);
    clayRock.AddSolidVolume(Solid::Gold, 1);

    Meter surfaceWidth = surfaceLevel->GetLevelSize().x;
    Meter surfaceHeight = surfaceLevel->GetLevelSize().y;
    PaintTiles(surfaceLevel, dryAir,  MeterRect(0,0, surfaceWidth,surfaceHeight/3));
    PaintTiles(surfaceLevel, clayRock,  MeterRect(0, surfaceHeight/3, surfaceWidth, surfaceHeight));

            //PaintTile(Rect2(0,0, worldWidth,worldHeight/2), air);
            //PaintTile(Rect2(0,worldHeight/3, worldWidth,2 * worldHeight/3), limestoneRock);
            //PaintTile(Rect2(worldWidth/2,worldHeight/3 - 10, 10,10), limestoneRock);
            //PaintTile(Rect2(worldWidth/3,worldHeight/3-200 , 100,200), limestoneRock);

    // TODO
    //Repack(surfaceLevel);

/*
    int worldBlockWidth = 16; // 4 km
    int worldBlockHeight = 4; // 1 km

    scalar worldWidth = worldBlockWidth * maxBlockSize;
    scalar worldHeight = worldBlockHeight * maxBlockSize;

    m_worldArea = Rect2(0,0,worldWidth, worldHeight);




    // Fill world

    // Air
    AuroraMaterial air;
    air.AddGasMaterial(MaterialQuantity(Material::Nitrogen, 78, 0));
    air.AddGasMaterial(MaterialQuantity(Material::Oxygen, 21, 0));
    air.AddGasMaterial(MaterialQuantity(Material::Water, 1, 0));
    air.SetTemperature(25);

    AuroraMaterial limestoneRock;
    limestoneRock.SetSolidProperties(false);
    limestoneRock.AddSolidMaterial(MaterialQuantity(Material::Limestone, 0.8, 1));
    limestoneRock.SetTemperature(25);


    PaintTile(Rect2(0,0, worldWidth,worldHeight/2), air);
    PaintTile(Rect2(0,worldHeight/3, worldWidth,2 * worldHeight/3), limestoneRock);
    PaintTile(Rect2(worldWidth/2,worldHeight/3 - 10, 10,10), limestoneRock);
    PaintTile(Rect2(worldWidth/3,worldHeight/3-200 , 100,200), limestoneRock);
*/
    //Repack();
}

void AuroraWorldEditor::PaintTiles(Level* level, TileComposition const& composition, MeterRect area)
{
    for(Tile* tile : level->GetTiles())
    {
        PaintTile(level, tile, composition, area);
    }
}

void AuroraWorldEditor::PaintTile(Level* level, Tile* tile, TileComposition const& composition, MeterRect area)
{
    //printf("PaintTile area=%ls tile_area=%ls\n", String(area).c_str(), String(m_worldArea).c_str());

    switch(tile->IsInside(area))
    {
    case Tile::InsideMode::Yes:
    case Tile::InsideMode::Partially:
    {
        SetTileComposition(tile, composition);
    break;
    }
    case Tile::InsideMode::No:
    break;
    }
}

void AuroraWorldEditor::SetTileComposition(Tile* tile, TileComposition composition)
{
    Volume volume = tile->GetTotalVolume();

    //TileContent newContent(volume, 0);
    tile->ClearContent();

    if(composition.solids.size() > 0)
    {
        Volume solidsVolume = Volume(volume * (1.f - composition.porosity));

        Volume remainingVolume = solidsVolume;
        Volume remainingVolumePart = 0;

        for (TileSolidVolume& solid : composition.solids) {
            remainingVolumePart += solid.volumePart;
        }

        for (TileSolidVolume& solid : composition.solids) {
            Scalar proportion = Scalar(solid.volumePart) / Scalar(remainingVolumePart);
            Volume solidVolume = Volume(remainingVolume * proportion);

            remainingVolume -= solidVolume;
            remainingVolumePart-= solid.volumePart;

            Quantity solidN = PhysicalConstants::EstimateSolidNByVolume(solid.solid, solidVolume);

            //Energy thermalEnergy = PhysicalConstants::EstimateThermalEnergy(solid.solid, solidN, composition.solidTemperature);
            Energy thermalEnergy = 0; // TODO

            tile->AddSolid(solid.solid, solidN, thermalEnergy);
        }
    }

    if(composition.liquids.size() > 0)
    {
        Volume availableVolume = tile->GetTotalVolume() - tile->GetSolidsVolume();

         for (TileLiquidVolume& liquid : composition.liquids) {
            Volume liquidVolume = Volume(availableVolume * liquid.volumeProportion);


            Quantity totalQuantity = PhysicalConstants::EstimateLiquidNByVolume(liquid.liquid, liquidVolume, liquid.pressure, liquid.temperature);
            Quantity dissolvedQuantity = Quantity(totalQuantity * liquid.dissolvedProportion);
            Quantity liquidQuantity = totalQuantity - dissolvedQuantity;

            Energy thermalEnergy = 0;
            //Energy thermalEnergy = PhysicalConstants::EstimateThermalEnergy(liquid.liquid, liquidQuantity, liquid.temperature);
            // TODO
            //thermalEnergy += PhysicalConstants::EstimateThermalEnergy(PhysicalConstants::GetDissolvedMaterial(liquid.liquid), dissolvedQuantity, liquid.temperature);

            tile->AddLiquid(liquid.liquid, liquidQuantity, dissolvedQuantity, thermalEnergy);
         }
    }

    Quantity GasPartSum = 0;
    for (Gas gas : AllGas())
    {
        GasPartSum += composition.Gas.composition[gas];
    }

    if(GasPartSum > 0)
    {
        Volume gasVolume = tile->GetGasVolume();
        Quantity totalN = PhysicalConstants::EstimateGasN(gasVolume, composition.Gas.pressure, composition.Gas.temperature);

        for (Gas gas : AllGas())
        {
            if(GasPartSum <= 0)
            {
                break;
            }

            Scalar proportion = Scalar(composition.Gas.composition[gas]) / Scalar(GasPartSum);
            Quantity GasN= Quantity(totalN * proportion);

            totalN -= GasN;
            GasPartSum-= composition.Gas.composition[gas];

            Energy internalEnergy = PhysicalConstants::EstimateInternalEnergy(gas, GasN, composition.Gas.temperature, gasVolume);

            tile->AddGas(gas, GasN, internalEnergy);
        }
    }
    tile->GetGazNode().ComputeCache();
}

void TileComposition::AddLiquidVolume(Liquid liquid, Scalar volumeProportion, Scalar dissolvedProportion, Scalar temperature, Scalar pressure)
{
    TileLiquidVolume volume;
    volume.liquid = liquid;
    volume.volumeProportion = volumeProportion;
    volume.dissolvedProportion = dissolvedProportion;
    volume.temperature = temperature;
    volume.pressure = pressure;
     liquids.push_back(volume);
}

void TileComposition::AddSolidVolume(Solid solid, Volume volumePart)
{
    TileSolidVolume volume;
    volume.solid = solid;
    volume.volumePart = volumePart;
    solids.push_back(volume);
}

TileGasComposition::TileGasComposition()
{
    for(Gas gas : AllGas())
    {
        composition[gas] = 0;
    }
}



}

