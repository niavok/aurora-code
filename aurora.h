#ifndef AURORA_H
#define AURORA_H

#include "core/reference.h"

namespace aurora
{

class AuroraGame : public Reference {
	GDCLASS(AuroraGame, Reference);

    int count;

protected:
    static void _bind_methods();

public:
    void add(int value);
    void reset();
    int get_total() const;

	AuroraGame();
};

}

#endif
