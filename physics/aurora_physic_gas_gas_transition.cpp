#include "aurora_physic_gas_gas_transition.h"
#include "aurora_physic_constants.h"

#include "stdio.h"
#include <assert.h>

namespace aurora {

GasGasTransition::GasGasTransition(GasGasTransition::Config const& config)
    : Transition (config.direction, config.section)
    , m_links {&config.A, &config.B }
    , m_frictionCoef(0.9) // TODO
    , m_kineticEnergy(0)
{
    m_links[0].altitudeRelativeToNode = config.relativeAltitudeA;
    m_links[1].altitudeRelativeToNode = config.relativeAltitudeB;
    m_links[0].longitudeRelativeToNode = config.relativeLongitudeA;
    m_links[1].longitudeRelativeToNode = config.relativeLongitudeB;

    for(int i = 0; i < LinkCount; i++)
    {
        m_links[i].outputInternalEnergy = 0;
        m_links[i].inputKineticEnergy = 0;
        m_links[i].outputKineticEnergy = 0;

        for(Gas gas : AllGas())
        {
            m_links[i].outputMaterial[gas] = 0;
        }

    }
}

void GasGasTransition::ApplyInput()
{
    NodeLink& linkA = m_links[0];
    NodeLink& linkB = m_links[1];
    m_kineticEnergy += linkA.inputKineticEnergy - linkB.inputKineticEnergy;

    linkA.inputKineticEnergy = 0;
    linkB.inputKineticEnergy = 0;
}

void GasGasTransition::Step(Scalar delta)
{
     NodeLink& linkA = m_links[0];
    NodeLink& linkB = m_links[1];
    GasNode& A = *((GasNode*) linkA.node);
    GasNode& B = *((GasNode*) linkB.node);

    Energy initialKineticEnergyDelta = m_kineticEnergy;

    assert(LinkCount == 2);
    Energy kineticEnergySum = std::abs(m_kineticEnergy);

    for(int i = 0; i < LinkCount; i++)
    {
        assert(m_links[i].inputKineticEnergy == 0);
        assert(m_links[i].outputInternalEnergy == 0);
        assert(m_links[i].outputKineticEnergy == 0);
    }

    Energy initialCheckEnergy = kineticEnergySum;

    Energy kineticEnergyDelta = initialKineticEnergyDelta;

    Scalar pressureA = A.GetPressure() + A.GetPressureGradient() * (A.GetHeight() - linkA.altitudeRelativeToNode);
    Scalar pressureB = B.GetPressure() + B.GetPressureGradient() * (A.GetHeight() - linkB.altitudeRelativeToNode);

    Meter deltaAltitude = B.GetCenterAltitude() - A.GetCenterAltitude();

    Scalar deltaMass = 0;

    // Apply pressure acceleration
    Scalar kineticAcceleration = 0;
    {
        size_t pressureSourceIndex;
        Scalar pressureDiff = pressureA - pressureB;
        if(pressureDiff > 0)
        {
            pressureSourceIndex = 0;
        }
        else
        {
            pressureSourceIndex = 1;
        }

        GasNode& pressureSourceNode = *((GasNode*) m_links[pressureSourceIndex].node);
        GasNode& pressureDestinationNode = *((GasNode*) m_links[1-pressureSourceIndex].node);

        Scalar movingRatio = 1;

        // Proto
        Scalar sourceN0 = pressureSourceNode.GetN()/* - movingRatio * pressureSourceNode.GetMovingN()*/;

        if(sourceN0 > 1e-15 && pressureSourceNode.GetThermalEnergy() > 0)
        {
            Energy sourceE0 = pressureSourceNode.GetThermalEnergy() * sourceN0 / pressureSourceNode.GetN();
            Scalar sourceMolarMass = pressureSourceNode.GetMolarMass();
            Scalar sourceMolarHeatCapacity = pressureSourceNode.GetEnergyPerK() / pressureSourceNode.GetN();
            Scalar sourceDh = pressureSourceNode.GetHeight() - m_links[pressureSourceIndex].altitudeRelativeToNode;

            Scalar destinationN0 = pressureDestinationNode.GetN()/* - movingRatio * pressureDestinationNode.GetMovingN()*/;
            Energy destinationE0 = destinationN0 > 0 ? pressureDestinationNode.GetThermalEnergy() * destinationN0 / pressureDestinationNode.GetN() : 0;
            Scalar destinationMolarMass = pressureDestinationNode.GetMolarMass();
            Scalar destinationMolarHeatCapacity = destinationN0 > 0 ? pressureDestinationNode.GetEnergyPerK() / pressureDestinationNode.GetN() : sourceMolarHeatCapacity;
            Scalar destinationDh = pressureDestinationNode.GetHeight() - m_links[1-pressureSourceIndex].altitudeRelativeToNode;

            Volume sourceVolume = pressureSourceNode.GetVolume();
            Volume destinationVolume = pressureDestinationNode.GetVolume();

            auto ComputeF = [](Energy E0, Scalar molarHeatCapacity, Quantity N0, Scalar molarMass, Scalar dh)
            {
                return (PhysicalConstants::gasConstant * E0) / molarHeatCapacity + N0 * molarMass * PhysicalConstants::gravity * dh;
            };

            auto ComputeM = [](Energy sourceE, Scalar molarHeatCapacity, Quantity sourceN, Scalar molarMass, Scalar dh)
            {
                return PhysicalConstants::gasConstant / molarHeatCapacity + sourceN * molarMass * PhysicalConstants::gravity * dh / sourceE;
            };

            Scalar sourceF = ComputeF(sourceE0, sourceMolarHeatCapacity, sourceN0, sourceMolarMass, sourceDh);
            Scalar destinationF = ComputeF(destinationE0, destinationMolarHeatCapacity, destinationN0, destinationMolarMass, destinationDh);

            Scalar sourceM = ComputeM(sourceE0, sourceMolarHeatCapacity, sourceN0, sourceMolarMass, sourceDh);
            Scalar destinationM = ComputeM(sourceE0, destinationMolarHeatCapacity, sourceN0, destinationMolarMass, destinationDh);


            Energy dE = (destinationVolume * sourceF - sourceVolume * destinationF) / (sourceVolume * destinationM + destinationVolume * sourceM);

            Quantity dN = dE * sourceN0 / sourceE0;

            Quantity sourceTotalN = pressureSourceNode.GetN();
            Quantity sourceTotalUsableN = sourceTotalN / pressureSourceNode.GetTransitionLinks().size();

            Quantity movingN = std::min(dN,sourceTotalUsableN);

            Scalar movingMass = movingN * pressureSourceNode.GetMolarMass();
            kineticAcceleration = sign(pressureDiff) * movingMass * PhysicalConstants::kineticCoef * PhysicalConstants::gasViscosity * pressureSourceNode.GetTemperature() / pressureDestinationNode.GetTransitionLinks().size();
            assert(!isnan(kineticAcceleration));
        }
    }

    Energy newKineticEnergyDelta = kineticEnergyDelta + kineticAcceleration;

    size_t sourceIndex;
    size_t destinationIndex;

    if(newKineticEnergyDelta > 0)
    {
        sourceIndex = 0;
        destinationIndex = 1;
    }
    else {
        sourceIndex = 1;
        destinationIndex = 0;
    }

    Energy kineticEnergy = abs(newKineticEnergyDelta);
    Scalar takenNRatio = 0;

    if(kineticEnergy > 0)
    {
        GasNode& sourceNode = *((GasNode*) m_links[sourceIndex].node);

        Quantity sourceTotalN = sourceNode.GetN();

        Quantity needTransfertNT = kineticEnergy / (sourceNode.GetMolarMass() * PhysicalConstants::kineticCoef);
        if(sourceTotalN > 0 && needTransfertNT > 0)
        {
            Quantity sourceTotalUsableN = sourceTotalN / (sourceNode.GetTransitionLinks().size() * 1.01);
            Energy sourceTotalUsableEnergy = sourceNode.GetInternalEnergy() / (sourceNode.GetTransitionLinks().size() * 1.01);

            auto TakeComposition = [&](bool useOutput, Quantity transfertN, Quantity totalN)
            {
                if(transfertN == 0)
                {
                    return;
                }

                for(Gas gas : AllGas())
                {
                    Quantity gasN;
                    if(useOutput)
                    {
                        gasN = sourceNode.GetOutputN(gas);
                    }
                    else
                    {
                        gasN = sourceNode.GetInputN(gas);
                    }

                    // No diffusion
                    Scalar compositionRatio = Scalar(gasN) / Scalar(totalN);
                    Quantity quantity = Quantity(transfertN * compositionRatio);
                    m_links[sourceIndex].outputMaterial[gas] -= quantity;
                    m_links[destinationIndex].outputMaterial[gas] += quantity;

                    Scalar mass = quantity * PhysicalConstants::GetMassByN(gas);
                    deltaMass += sign(newKineticEnergyDelta) * mass;
                }

                Energy energy;
                if(useOutput)
                {
                    energy = sourceNode.GetOutputEnergy();
                }
                else
                {
                    energy = sourceNode.GetInputEnergy();
                }

                // Take energy ratio
                Scalar takenRatio = Scalar(transfertN) / totalN;
                Energy takenEnergy = Energy(takenRatio * energy);
                m_links[sourceIndex].outputInternalEnergy -= takenEnergy;
                m_links[destinationIndex].outputInternalEnergy += takenEnergy;
            };

            Quantity takenN = 0;
            Quantity sourceTotalOutputN = sourceNode.GetOutputN();
            Quantity sourceOutputTemperature = sourceNode.GetOutputTemperature();

            assert(sourceOutputTemperature > 0);
            bool needInput = false;
            Energy takenEnergy = 0;
            if(sourceTotalOutputN > 0)
            {
                Quantity needTransfertNAtOutputTemperature = needTransfertNT / sourceOutputTemperature;

                Quantity needOutputTransfertN = needTransfertNAtOutputTemperature;
                if(needOutputTransfertN > sourceTotalOutputN)
                {
                    needOutputTransfertN = sourceTotalOutputN;
                    needInput = true;
                }

                Quantity outputTransfertN = needOutputTransfertN;
                if(outputTransfertN > sourceTotalUsableN)
                {
                    outputTransfertN = sourceTotalUsableN;
                    needInput = false;
                }

                Energy outputTransfertEnergy = sourceNode.GetOutputEnergy() * outputTransfertN / sourceTotalOutputN;

                if(outputTransfertEnergy > sourceTotalUsableEnergy)
                {
                    outputTransfertN = outputTransfertN * sourceTotalUsableEnergy / outputTransfertEnergy;
                    outputTransfertEnergy = sourceTotalUsableEnergy;
                    needInput = false;
                }

                takenN += outputTransfertN;
                takenEnergy += outputTransfertEnergy;

                TakeComposition(true, outputTransfertN, sourceTotalOutputN);
            }

            if(needInput)
            {
                Scalar takenNT = takenN * sourceOutputTemperature;
                Quantity sourceTotalInputN = sourceNode.GetInputN();
                if(sourceTotalInputN > 0)
                {
                    Quantity sourceTotalUsableNForInput = sourceTotalUsableN - takenN;
                    Quantity inputNeedTransfertNT = needTransfertNT - takenNT;

                    Quantity needInputTransfertN = inputNeedTransfertNT / sourceNode.GetInputTemperature();
                    Quantity inputTransfertN = std::min(needInputTransfertN, sourceTotalUsableNForInput);

                    Energy inputTransfertEnergy = sourceNode.GetInputEnergy() * inputTransfertN / sourceTotalInputN;
                    Energy sourceTotalUsableEnergyForInput = sourceTotalUsableEnergy - takenEnergy;

                    if(inputTransfertEnergy > sourceTotalUsableEnergyForInput)
                    {
                        inputTransfertN = inputTransfertN * sourceTotalUsableEnergyForInput / inputTransfertEnergy;
                    }

                    TakeComposition(false, inputTransfertN, sourceTotalInputN);
                    takenN += inputTransfertN;
                }
            }

            takenNRatio = takenN / sourceTotalUsableN;

        }
        else
        {
            // No composition movement, disperse thermal energy
            newKineticEnergyDelta = 0;
        }
    }


    // Absorb or add kinetic energy from altitude change
    Energy deltaPotentialEnergy = 0;

    if(deltaAltitude != 0)
    {
        deltaPotentialEnergy = -deltaAltitude * deltaMass * PhysicalConstants::gravity * PhysicalConstants::potentialEnergyCoef;
    }

    assert(m_links[sourceIndex].outputKineticEnergy == 0);

    // Apply pressure acceleration
    size_t newSourceIndex;
    size_t newDestinationIndex;

    if(newKineticEnergyDelta > 0)
    {
        newSourceIndex = 0;
        newDestinationIndex = 1;
    }
    else {
        newSourceIndex = 1;
        newDestinationIndex = 0;
    }

    Energy newKineticEnergyBeforeLoss = abs(newKineticEnergyDelta);

    Scalar kineticLossRatio = PhysicalConstants::gasKineticLossRatio;

    if(takenNRatio == 0)
    {
        kineticLossRatio = 1;
    }

    Energy thermalLoss = newKineticEnergyBeforeLoss * kineticLossRatio; // TODO make depend on section/material/context
    Energy newKineticEnergy = newKineticEnergyBeforeLoss - thermalLoss;
    assert(abs(newKineticEnergy) <= abs(newKineticEnergyDelta));

    Scalar kineticPropagationRatio = takenNRatio;
    Energy transmitedKineticEnergyUnchecked = newKineticEnergy * kineticPropagationRatio;
    Energy newLocalKineticEnergyUnchecked = newKineticEnergy - transmitedKineticEnergyUnchecked;

    auto ComputeAvailableEnergy = [&](int index)
    {
        GasNode& node = *((GasNode*) m_links[index].node);
        return node.GetInternalEnergy() / (node.GetTransitionLinks().size() * 1.01);
    };

    // Take transmited energy to destination
    Energy destinationAvailableEnergy = ComputeAvailableEnergy(newDestinationIndex);
    Energy destinationEnergyBalance = destinationAvailableEnergy + m_links[newDestinationIndex].outputInternalEnergy - transmitedKineticEnergyUnchecked;

    Energy transmitedKineticEnergy;
    if(destinationEnergyBalance < 0)
    {
        transmitedKineticEnergy = transmitedKineticEnergyUnchecked + destinationEnergyBalance;
    }
    else
    {
        transmitedKineticEnergy = transmitedKineticEnergyUnchecked;
    }
    m_links[newDestinationIndex].outputInternalEnergy -= transmitedKineticEnergy;
    m_links[newDestinationIndex].outputKineticEnergy = transmitedKineticEnergy;

    // Take local energy to source
    Energy sourceAvailableEnergy = ComputeAvailableEnergy(newSourceIndex);
    Energy localKineticEnergyDiff = newLocalKineticEnergyUnchecked - kineticEnergySum;
    Energy sourceEnergyBalance = sourceAvailableEnergy + m_links[newSourceIndex].outputInternalEnergy - localKineticEnergyDiff + deltaPotentialEnergy;

    Energy localKineticEnergyGain;
    if(sourceEnergyBalance < 0)
    {
        localKineticEnergyGain = localKineticEnergyDiff + sourceEnergyBalance;
    }
    else
    {
        localKineticEnergyGain = localKineticEnergyDiff;
    }
    m_links[newSourceIndex].outputInternalEnergy -= (localKineticEnergyGain - deltaPotentialEnergy);
    Energy newLocalKineticEnergy =sign(newKineticEnergyDelta) * (kineticEnergySum + localKineticEnergyGain);

#if 0
    Energy finalCheckEnergy = abs(newLocalKineticEnergy);
    finalCheckEnergy -= deltaPotentialEnergy; // The potential energy is exchanged from external
    for(int i = 0; i < LinkCount; i++)
    {
        finalCheckEnergy += m_links[i].outputInternalEnergy;
        finalCheckEnergy += m_links[i].outputKineticEnergy;
        assert(m_links[i].inputKineticEnergy == 0);
    }

    Energy energyCheckError = initialCheckEnergy - finalCheckEnergy;
    assert(std::abs(energyCheckError) < 1e-6);
#endif
    m_kineticEnergy = newLocalKineticEnergy;
}

}