#ifndef AURORA_WORLD_H
#define AURORA_WORLD_H

#include "scene/main/node.h"
#include "../tools/aurora_types.h"
#include "../physics/aurora_physic_engine.h"
#include "../physics/aurora_physic_gas_gas_transition.h"

#include <vector>

namespace aurora {

class Level;
class Tile;

class AuroraWorld : public Node {
    GDCLASS(AuroraWorld, Node)

    virtual ~AuroraWorld();

    int count;

    int64_t get_last_update_duration();

protected:
    static void _bind_methods();
    void _notification(int p_what);

public:
    AuroraWorld();

    void Update(Scalar deltaTime);

    Level* CreateLevel(Meter2 levelBottomLeftPosition, Meter tileSize, Meter depth, int tileHCount, int tileVCount, bool horizontalLoop);

    std::vector<Level*>& GetLevels() { return m_levels; }
    std::vector<Level*> const& GetLevels() const { return m_levels; }

    void ConnectTiles(GasGasTransition::Config const& config);

    bool IsPaused();
    void SetPause(bool pause);
    void Step();

private:
    void InitPhysics();

    std::vector<Level*> m_levels;

    PhysicEngine m_physicEngine;
    bool m_isPaused;
    int64_t m_lastUpdateDuration;
};

}

#endif
