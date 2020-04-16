#ifndef AURORA_GAME_MANAGER_H
#define AURORA_GAME_MANAGER_H

#include "scene/main/node.h"
#include "aurora_game.h"

namespace aurora {

class AuroraGameManager : public Node {
	GDCLASS(AuroraGameManager, Node)
protected:
	static void _bind_methods();
	void _notification(int p_what);

public:
	AuroraGameManager();
	virtual ~AuroraGameManager();

	Ref<AuroraGame> GetGame();
	bool HasGame();

	void CreateGame();

private:
	Ref<AuroraGame> m_game;
};

}

#endif
