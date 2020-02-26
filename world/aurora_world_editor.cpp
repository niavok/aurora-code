#include "aurora_world_editor.h"
#include "aurora_level.h"

#include "../physics/aurora_physic_constants.h"
#include "../physics/aurora_physic_types.h"


namespace aurora {

WorldEditor::WorldEditor(AuroraWorld& world)
    : m_world(world)
{
}

WorldEditor::~WorldEditor()
{
}

void WorldEditor::GenerateTestWorld1()
{
    int blockCountX = 2;
    int blockCountY = 80;
    Mm tileSizeMm = 50;
    Level* surfaceLevel = m_world.CreateLevel(false, tileSizeMm, 0, blockCountX, blockCountY); // 20 x 1 blocks * 50 mm * 2^ 1 = 1 m x 1 m
    int surfaceWidth = surfaceLevel->GetSizeMm().x;
    int surfaceHeight = surfaceLevel->GetSizeMm().y;

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
    PaintTiles(surfaceLevel,  dryAir,  MmRect(0,0, surfaceWidth,surfaceHeight));

    TileComposition highPressureDryAir = dryAir;
    highPressureDryAir.Gas.pressure = 100000 * 1. ;
    highPressureDryAir.Gas.temperature = 274. + 500 *  0.5;
    PaintTiles(surfaceLevel, highPressureDryAir,  MmRect(0, 30 * surfaceHeight/ blockCountY, surfaceWidth/ blockCountX, 30 * surfaceHeight/ blockCountY));
}

void WorldEditor::GenerateTestWorld2()
{
    //Level* surfaceLevel = m_world.CreateLevel(50, 8, 1, 1); // 1 x 1 blocks * 50 mm * 2^ 8 = 12.8 m x 12.8 m
    //Level* surfaceLevel = m_world.CreateLevel(50, 0, 256, 256); // 256 x 256 blocks * 50 mm * 2^ 1 = 12.8 m x 12.8 m
    Mm tileSizeMm = 50;
    int blockCountX = 80;
    int blockCountY = 80;
    Level* surfaceLevel = m_world.CreateLevel(false, tileSizeMm, 0, blockCountX, blockCountY); // 20 x 20 blocks * 50 mm * 2^ 1 = 1 m x 1 m


    TileComposition dryAir;
    // No solid
    // No liquid
    // Nitrogen 80%, Oxygen 20%
    // Temperature 125K
    // Pressure 1 bar
    dryAir.Gas.composition[Gas::Nitrogen] = 80;
    dryAir.Gas.composition[Gas::Oxygen] = 20;
    dryAir.Gas.pressure = 100000.; // 1 bar
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


    int surfaceWidth = surfaceLevel->GetSizeMm().x;
    int surfaceHeight = surfaceLevel->GetSizeMm().y;
    PaintTiles(surfaceLevel, dryAir,  MmRect(0,0, surfaceWidth,surfaceHeight));
    //PaintTiles(surfaceLevel, hotDryAir,  MmRect(0,0, surfaceWidth,surfaceHeight));


    //PaintTiles(surfaceLevel, hotDryAir,  MmRect(surfaceWidth / 4, surfaceHeight - surfaceHeight / 6, surfaceWidth / 6,surfaceHeight /8));

    TileComposition highPressureDryAir = dryAir;
    highPressureDryAir.Gas.pressure = 100000 * 200;
    highPressureDryAir.Gas.temperature = 274.+500; 
    PaintTiles(surfaceLevel, highPressureDryAir,  MmRect(surfaceWidth / 4, 2*surfaceHeight/3, surfaceHeight / 32, surfaceHeight / 32));
}


void WorldEditor::GenerateTestWorld3()
{

    Level* surfaceLevel = m_world.CreateLevel(true, 50, 12, 5, 2); // 20 x 5 blocks = 4096 m x 1024 m

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

    int surfaceWidth = surfaceLevel->GetSizeMm().x;
    int surfaceHeight = surfaceLevel->GetSizeMm().y;
    PaintTiles(surfaceLevel, dryAir,  MmRect(0,0, surfaceWidth,surfaceHeight/3));
    PaintTiles(surfaceLevel, clayRock,  MmRect(0, surfaceHeight/3, surfaceWidth, surfaceHeight));

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

void WorldEditor::PaintTiles(Level* level, TileComposition const& composition, MmRect area)
{
    for(Tile* tile : level->GetRootTiles())
    {
        PaintTile(level, tile, composition, area);
    }
}

void WorldEditor::PaintTile(Level* level, Tile* tile, TileComposition const& composition, MmRect area)
{
    //printf("PaintTile area=%ls tile_area=%ls\n", String(area).c_str(), String(m_worldArea).c_str());

    switch(tile->IsInside(area))
    {
    case Tile::InsideMode::Yes:
    {
        //printf("inside: Yes\n");
        // All the tile is in the target area, put the material everywhere
        SetTileComposition(tile, composition);
    }
    break;
    case Tile::InsideMode::Partially:
    {
        //printf("inside: Partially\n");
        if(tile->Split(level))
        {
            //printf("split ok\n");
            for(Tile* child: tile->GetChildren())
            {
                //printf("split child\n");
                PaintTile(level, child, composition, area);
            }
            //printf("split done\n");
        }
        else
        {
            //printf("split ko\n");
            // Cannot split more, already at min size, override
            SetTileComposition(tile, composition);
        }
    }
    break;
    case Tile::InsideMode::No:
        //printf("inside: No\n");
    break;
    }
}

void WorldEditor::SetTileComposition(Tile* tile, TileComposition composition)
{
    Volume volume = tile->GetVolume();

    TileContent newContent(volume, 0);

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

            newContent.AddSolid(solid.solid, solidN, thermalEnergy);
        }
    }

    if(composition.liquids.size() > 0)
    {
        Volume availableVolume = newContent.GetTotalVolume() - newContent.GetSolidsVolume();

         for (TileLiquidVolume& liquid : composition.liquids) {
            Volume liquidVolume = Volume(availableVolume * liquid.volumeProportion);


            Quantity totalQuantity = PhysicalConstants::EstimateLiquidNByVolume(liquid.liquid, liquidVolume, liquid.pressure, liquid.temperature);
            Quantity dissolvedQuantity = Quantity(totalQuantity * liquid.dissolvedProportion);
            Quantity liquidQuantity = totalQuantity - dissolvedQuantity;

            Energy thermalEnergy = 0;
            //Energy thermalEnergy = PhysicalConstants::EstimateThermalEnergy(liquid.liquid, liquidQuantity, liquid.temperature);
            // TODO
            //thermalEnergy += PhysicalConstants::EstimateThermalEnergy(PhysicalConstants::GetDissolvedMaterial(liquid.liquid), dissolvedQuantity, liquid.temperature);

            newContent.AddLiquid(liquid.liquid, liquidQuantity, dissolvedQuantity, thermalEnergy);
         }
    }

    Quantity GasPartSum = 0;
    for (Gas gas : AllGas())
    {
        GasPartSum += composition.Gas.composition[gas];
    }

    if(GasPartSum > 0)
    {
        Volume GasVolume = newContent.GetGasVolume();
        Quantity totalN = PhysicalConstants::EstimateGasN(GasVolume, composition.Gas.pressure, composition.Gas.temperature);

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

            Energy thermalEnergy = PhysicalConstants::EstimateThermalEnergy(gas, GasN, composition.Gas.temperature);

            newContent.AddGas(gas, GasN, thermalEnergy);
        }
    }
    tile->SetContent(newContent);
    tile->GetContent()->GetGazNode().ComputeCache();
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

