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


class AuroraWorldEditor : public Node2D {
     GDCLASS(AuroraWorldEditor, Node2D)

protected:
    static void _bind_methods();
	void _notification(int p_what);

	Node* get_world() const;

	void set_world_node(const NodePath &p_node);
	NodePath get_world_node();

public:
    AuroraWorldEditor();
    virtual ~AuroraWorldEditor();

    void GenerateTestWorld1();
    void GenerateTestWorld2();
    void GenerateTestWorld3();

private:
    AuroraWorld* m_world;
	NodePath m_targetWorldPath;

    void PaintTiles(Level* level, TileComposition const& composition, MeterRect area);
    void PaintTile(Level* level, Tile* tile, TileComposition const& composition, MeterRect area);
    void SetTileComposition(Tile* tile, TileComposition composition);

    void Repack(Level* level);
};

}

#endif
