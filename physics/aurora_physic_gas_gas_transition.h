#ifndef AURORA_PHYSIC_GAS_GAS_TRANSITION_H
#define AURORA_PHYSIC_GAS_GAS_TRANSITION_H

#include "aurora_physic_transition.h"
#include "aurora_physic_gas_node.h"

namespace aurora {


class GasGasTransition : public Transition
{
public:

    struct Config
    {
        Config(GasNode& iA, GasNode& iB): A(iA), B(iB) { }
        GasNode& A;
        GasNode& B;
        Direction direction;
        Meter relativeAltitudeA;
        Meter relativeAltitudeB;
        Meter relativeLongitudeA;
        Meter relativeLongitudeB;
        Meter section = 0;
    };

    GasGasTransition(Config const& config);

    FluidNode* GetNodeA() { return m_links[0].node; }
    FluidNode* GetNodeB() { return m_links[1].node; }

    NodeLink* GetNodeLink(size_t index) { return &m_links[index]; }
    NodeLink const* GetNodeLink(size_t index) const { return &m_links[index]; }
    size_t GetNodeLinkCount() { return LinkCount; }


    //Energy GetEnergy() const { return m_kineticEnergy; }

    void Step(Scalar delta);

    static const int LinkCount = 2;

private:


    NodeLink m_links[LinkCount];

    // Contants
    //GasNode& m_A;
    //GasNode& m_B;
    //Meter m_altitudeA;
    //Meter m_altitudeB;
    Scalar m_frictionCoef;

    // Variables
    //Energy m_kineticEnergy;
    //Energy m_inputEnergyFromA;
    //Energy m_inputEnergyFromB;

    //Energy m_outputEnergyBalanceToA;
    //Energy m_outputEnergyBalanceToB;
    //Quantity m_outputMaterialBalanceA[Material::GasMoleculeCount];
    //Quantity m_outputMaterialBalanceB[Material::GasMoleculeCount];
};

}

#endif
