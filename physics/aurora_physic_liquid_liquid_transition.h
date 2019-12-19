#ifndef AURORA_PHYSIC_LIQUID_LIQUID_TRANSITION_H
#define AURORA_PHYSIC_LIQUID_LIQUID_TRANSITION_H

//#include "core/math/vector2.h"
//#include "core/math/rect2.h"
#include "../tools/aurora_types.h"
//#include <vector>

namespace aurora {


class LiquidLiquidTransition : public Transition
{
    LiquidLiquidTransition(LiquidNode& A, LiquidNode& B);

    // Contants
    LiquidNode& m_A;
    LiquidNode& m_B;
    int32_t m_altitudeA;
    int32_t m_altitudeB;
    float m_frictionCoef;

    // Variables
    int64_t m_kineticEnergy;
};


}

#endif
