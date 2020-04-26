#ifndef AURORA_WORLD_H
#define AURORA_WORLD_H

#include "scene/main/node.h"
#include "../tools/aurora_types.h"
#include "../physics/aurora_physic_engine.h"
#include "../physics/aurora_physic_gas_gas_transition.h"

#include <vector>

#include "core/reference.h"
#include "core/array.h"


namespace aurora {

class AuroraLevel;
class Tile;

class AuroraWorld : public Reference {
    GDCLASS(AuroraWorld, Reference)

    int64_t get_last_update_duration();

protected:
    static void _bind_methods();
    //void _notification(int p_what);

public:
    AuroraWorld();
	virtual ~AuroraWorld();

    void Update(Scalar deltaTime);

    Ref<AuroraLevel> CreateLevel(Meter2 levelBottomLeftPosition, Meter tileSize, Meter depth, int tileHCount, int tileVCount, bool horizontalLoop);



    std::vector<Ref<AuroraLevel>>& GetLevels() { return m_levels; }
    std::vector<Ref<AuroraLevel>> const& GetLevels() const { return m_levels; }
	Array GetLevelArray();
	size_t GetLevelCount();

	int64_t GetLevelsVersion() const;

    void ConnectTiles(GasGasTransition::Config const& config);

    //bool IsPaused();
    //void SetPause(bool pause);
    void Step();

private:
    void InitPhysics();
	void InitLevelPhysics(Ref<AuroraLevel> & level);

    std::vector<Ref<AuroraLevel>> m_levels;

    PhysicEngine m_physicEngine;
    //bool m_isPaused;
    int64_t m_lastUpdateDuration;
	int64_t m_levelsVersion;
};

}

#endif
