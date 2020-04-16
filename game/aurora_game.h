#ifndef AURORA_GAME_H
#define AURORA_GAME_H

#include "core/reference.h"

namespace aurora {
class AuroraGame : public Reference {
	GDCLASS(AuroraGame, Reference);
protected:
	static void _bind_methods();
public:
	AuroraGame();
	virtual ~AuroraGame();

	void Save();
};

}

#endif
