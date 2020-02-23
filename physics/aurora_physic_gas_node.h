#ifndef AURORA_PHYSIC_GAS_NODE_H
#define AURORA_PHYSIC_GAS_NODE_H

#include "aurora_physic_types.h"
#include "aurora_physic_fluid_node.h"

namespace aurora {

class GasNode : public FluidNode
{
public:
    GasNode();
    GasNode(GasNode& node);

    Volume GetVolume() const;
    void SetVolume(Volume volume);

    Quantity GetN(Gas gas) const;
    Quantity GetInputN(Gas gas) const;
    Quantity GetOutputN(Gas gas) const;
    Energy GetThermalEnergy() const;
    
    void AddN(Gas gas, Quantity N);
    void AddThermalEnergy(Energy thermalEnergy);



    //void TakeN(Gas gas, Quantity N);
    void TakeThermalEnergy(Energy thermalEnergy);

    Scalar GetPressure() const;
    Scalar GetTemperature() const;
    Scalar GetInputTemperature() const;
    Scalar GetOutputTemperature() const;
    Quantity GetN() const;
    Quantity GetMovingN() const;
    Scalar GetPressureGradient() const;
    Quantity GetOutputN() const;
    Quantity GetInputN() const;


    Energy GetEnergy() const;
    Energy GetOutputEnergy() const;
    Energy GetInputEnergy() const;
    Energy GetCheckEnergy() const { return m_inputThermalEnergy + m_outputThermalEnergy; }
    Energy GetEnergyPerK() const { return m_cacheEnergyPerK; }
    Quantity GetCheckN() const { return m_cacheCheckN; }
    Scalar GetMolarMass() const { return m_cacheMolarMass; }

    Mm GetAltitudeMm() const { return m_altitude; }
    void SetAltitudeMm(Mm altitude);


    //Scalar ComputePressure() const;
    //Scalar ComputeTemperature() const;
    //Quantity ComputeN() const;
    //void ComputeNPT(Quantity& N, Scalar& pressure, Scalar& temperature) const;

    void PrepareTransitions();
    void ApplyTransitions();
    void ComputeCache();

private:

    void MigrateKineticEnergy();
    void FlushInput();

    // Constants
    Mm m_altitude;
    Volume m_volume;
    //int8_t m_transitionCount;

    // Variables
    //Quantity m_N; // TODO cache ?

    GasComposition m_inputNComposition;
    GasComposition m_outputNComposition;
    Energy m_inputThermalEnergy;
    Energy m_outputThermalEnergy;
    Quantity m_movingN;

    // Cache
    bool m_cacheComputed;
    Quantity m_cacheN;
    Quantity m_cacheInputN;
    Quantity m_cacheOutputN;
    Quantity m_cacheCheckN;
    Scalar m_cacheMass;
    Scalar m_cachePressure;
    Scalar m_cachePressureGradient;
    Scalar m_cacheTemperature;
    Scalar m_cacheInputTemperature;
    Scalar m_cacheOutputTemperature;
    Scalar m_cacheEnergyPerK;
    Scalar m_cacheMolarMass;
};

}

#endif
