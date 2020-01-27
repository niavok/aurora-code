#ifndef AURORA_PHYSIC_ENGINE_H
#define AURORA_PHYSIC_ENGINE_H

// #include "core/math/vector2.h"
// #include "core/math/rect2.h"
#include "../tools/aurora_types.h"
#include <vector>
#include "aurora_physic_transition.h"
#include "aurora_physic_fluid_node.h"

namespace aurora {


/*
class SharedVolumeGasLiquidTransition : public Transition
{
    SharedVolumeGasLiquidTransition(GasNode& A, LiquidNode& B);

    // Contants
    GasNode& m_A;
    LiquidNode& m_B;
    Mm m_altitudeA;
    Mm m_altitudeB;
    Scalar m_frictionCoef;

    // Variables
    Energy m_kineticEnergy;
};*/

class PhysicEngine
{
public:
    PhysicEngine();
    ~PhysicEngine();

    void Step(Scalar delta);
    void SubStep(Scalar delta);

    void Flush(); // Delete transition but not nodes

    void CheckDuplicates();

    void AddTransition(Transition* transition);

    void AddFluidNode(FluidNode* node);

private:

    std::vector<Transition*> m_transitions;
    std::vector<FluidNode*> m_nodes;

    void PrepareTransitions();
    void ComputeTransitions(Scalar delta);
    void ApplyTransitions();

    Energy ComputeEnergy(const char* label);

    enum StepState
    {
        TRANSITION_PREPARED,
        TRANSITION_COMPUTED,
        TRANSITION_APPLIED
    };

    StepState m_stepState;

};


}

#endif
