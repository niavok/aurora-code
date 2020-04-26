#ifndef AURORA_WORLD_EDITOR_H
#define AURORA_WORLD_EDITOR_H

#include "scene/2d/node_2d.h"
#include "aurora_world.h"
#include "aurora_tile.h"
#include "../physics/aurora_physic_engine.h"

namespace aurora {

struct TileGasComposition
{
    TileGasComposition();

    GasComposition composition;
    Scalar pressure;
    Scalar temperature;
};

struct TileLiquidVolume
{
    Liquid liquid;
    Scalar volumeProportion;
    Scalar dissolvedProportion;
    Scalar temperature;
    Scalar pressure;
};

struct TileSolidVolume
{
    Solid solid;
    Quantity volumePart;
};

struct TileComposition
{
    TileGasComposition Gas;
    std::vector<TileLiquidVolume> liquids;
    Scalar porosity; // Quantity of volume not used by solid, if solid present
    std::vector<TileSolidVolume> solids;
    Scalar solidTemperature;

    void AddLiquidVolume(Liquid liquid, Scalar volumeProportion, Scalar dissolvedProportion, Scalar temperature, Scalar pressure);
    void AddSolidVolume(Solid solid, Volume volumePart);
};


class AuroraWorldEditor : public Reference {
	GDCLASS(AuroraWorldEditor, Reference)

protected:
	static void _bind_methods();

public:
	AuroraWorldEditor();
	virtual ~AuroraWorldEditor();

	Ref<AuroraWorld> GetWorld() const;
	void SetWorld(Ref<AuroraWorld> world);

	Ref<AuroraLevel> CreateEmptyLevel();

    void GenerateTestWorld1();
    void GenerateTestWorld2();
    void GenerateTestWorld3();

private:
    Ref<AuroraWorld> m_world;
	NodePath m_targetWorldPath;

    void PaintTiles(Ref<AuroraLevel> level, TileComposition const& composition, MeterRect area);
    void PaintTile(Ref<AuroraLevel> level, Tile* tile, TileComposition const& composition, MeterRect area);
    void SetTileComposition(Tile* tile, TileComposition composition);

    void Repack(AuroraLevel* level);
};

}

#endif
