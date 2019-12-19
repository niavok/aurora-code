#include "aurora.h"

namespace aurora {

void AuroraGame::add(int value) {

    count += value;
}

void AuroraGame::reset() {

    count = 0;
}

int AuroraGame::get_total() const {

    return count;
}

void AuroraGame::_bind_methods() {

	ClassDB::bind_method(D_METHOD("add", "value"), &AuroraGame::add);
	ClassDB::bind_method(D_METHOD("reset"), &AuroraGame::reset);
	ClassDB::bind_method(D_METHOD("get_total"), &AuroraGame::get_total);
}

AuroraGame::AuroraGame() {
    count = 0;
}

}
