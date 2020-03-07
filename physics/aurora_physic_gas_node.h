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

    void AddN(Gas gas, Quantity N);
    //void AddThermalEnergy(Energy thermalEnergy);
    void AddInternalEnergy(Energy internalEnergy);



    //void TakeN(Gas gas, Quantity N);
    //void TakeThermalEnergy(Energy thermalEnergy);
    void TakeInternalEnergy(Energy internalEnergy);

    Scalar GetPressure() const;
    Scalar GetTemperature() const;
    Scalar GetInputTemperature() const;
    Scalar GetOutputTemperature() const;
    Quantity GetN() const;
    Quantity GetMovingN() const;
    Scalar GetPressureGradient() const;
    Quantity GetOutputN() const;
    Quantity GetInputN() const;


    Energy GetInternalEnergy() const;
    Energy GetThermalEnergy() const;
    Energy GetOutputEnergy() const;
    Energy GetInputEnergy() const;
    Energy GetEnergyPerK() const { return m_cacheEnergyPerK; }
    Quantity GetCheckN() const { return m_cacheCheckN; }
    Scalar GetMolarMass() const { return m_cacheMolarMass; }

    Meter GetCenterAltitude() const { return m_centerAltitude; }
    void SetCenterAltitude(Meter centerAltitude);
    Meter GetHeight() const { return m_height; }
    void SetHeight(Meter height);

    void ClearContent();

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
    Meter m_centerAltitude;
    Volume m_volume;
    Meter m_height;
    //int8_t m_transitionCount;

    // Variables
    //Quantity m_N; // TODO cache ?

    GasComposition m_inputNComposition;
    GasComposition m_outputNComposition;
    Energy m_inputInternalEnergy;
    Energy m_outputInternalEnergy;
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
    Scalar m_cacheThermalEnergyRatio;
};

}

#endif
