#ifndef AURORA_WORLD_H
#define AURORA_WORLD_H

#include "scene/main/node.h"
#include "../tools/aurora_types.h"
#include "../physics/aurora_physic_engine.h"

#include <vector>

namespace aurora {

class Level;
class Tile;

class AuroraWorld : public Node {
    GDCLASS(AuroraWorld, Node)

    virtual ~AuroraWorld();

    int count;

protected:
    static void _bind_methods();
    void _notification(int p_what);

public:
    AuroraWorld();

    void Update(Scalar delta);

    Level* CreateLevel(bool horizontalLoop, Mm minTileSize, int maxTileSubdivision, int rootTileHCount, int rootTileVCount);

    std::vector<Level*>& GetLevels() { return m_levels; }
    std::vector<Level*> const& GetLevels() const { return m_levels; }

    void ConnectTiles(Tile* tileA, Tile* tileB, Transition::Direction direction, Mm relativeAltitudeA, Mm relativeAltitudeB,  Mm relativeLongitudeA, Mm relativeLongitudeB, Mm section);

    bool IsPaused();
    void SetPause(bool pause);
    void Step();

private:
    void InitPhysics();

    std::vector<Level*> m_levels;

    PhysicEngine m_physicEngine;
    bool m_isPaused;
};

}

#endif
