#ifndef AURORA_GAME_H
#define AURORA_GAME_H

#include "core/reference.h"
#include "../world/aurora_world.h"
#include "../world/aurora_world_editor.h"

namespace aurora {
class AuroraGame : public Reference {
	GDCLASS(AuroraGame, Reference);
protected:
	static void _bind_methods();

public:
	AuroraGame();
	virtual ~AuroraGame();

	void Save();

	Ref<AuroraWorld> GetWorld();
	Ref<AuroraWorldEditor> GetWorldEditor();
private:
	Ref<AuroraWorld> m_world;
	Ref<AuroraWorldEditor> m_worldEditor;

	bool m_isPaused;
};

}

#endif
