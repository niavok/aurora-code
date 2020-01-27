#include "aurora_physic_gas_gas_transition.h"
#include "aurora_physic_constants.h"

#include <assert.h>

namespace aurora {

GasGasTransition::GasGasTransition(GasGasTransition::Config const& config)
    : Transition (config.direction, config.section)
    , m_links {&config.A, &config.B }
    , m_frictionCoef(0.9) // TODO
{
    m_links[0].altitudeRelativeToNode = config.relativeAltitudeA;
    m_links[1].altitudeRelativeToNode = config.relativeAltitudeB;
    m_links[0].longitudeRelativeToNode = config.relativeLongitudeA;
    m_links[1].longitudeRelativeToNode = config.relativeLongitudeB;

    for(int i = 0; i < LinkCount; i++)
    {
        m_links[i].outputThermalEnergy = 0;
        m_links[i].inputKineticEnergy = 0;
        m_links[i].outputKineticEnergy = 0;

        for(Gas gas : AllGas())
        {
            m_links[i].outputMaterial[gas] = 0;
        }

    }
}

void GasGasTransition::Step(Scalar delta)
{
    //TODO dt

    // For each tile
    // Propagate Ec to from A Ec
    
    // For each transition
    // add all Ec to current Ec
    // compute delta pressure and pressure dp
    // compute dp from Ec
    // compute total dp before friction
    // compute velocity square
    // compute friction lost
    // compute final need dp
    // commit dp, energy ejection


    // For each tile
    // add N, add E lost or gain




    //while negative N or E ask to all transitions some maters and energy
    // add negative tile to a liste and propagate until all tiles are ok



    // TODO no sqrt ?

    // Get energy propagation

    NodeLink& linkA = m_links[0];
    NodeLink& linkB = m_links[1];
    GasNode& A = *((GasNode*) linkA.node);
    GasNode& B = *((GasNode*) linkB.node);


    Energy initialKineticEnergyDelta = linkA.inputKineticEnergy - linkB.inputKineticEnergy; 

    assert(LinkCount == 2);
    Energy kineticEnergySum = 0;

    for(int i = 0; i < LinkCount; i++)
    {
        assert(m_links[i].inputKineticEnergy >= 0);
        kineticEnergySum += m_links[i].inputKineticEnergy;
        m_links[i].inputKineticEnergy = 0;
        assert(m_links[i].outputThermalEnergy == 0);
        assert(m_links[i].outputKineticEnergy == 0);
    }

    Energy initialCheckEnergy = kineticEnergySum;

    Energy kineticEnergyDelta = initialKineticEnergyDelta;

    // if(B.GetTemperature() > 150 || A.GetTemperature() > 150)
    // {
    //     char* plop;
    //     plop = "hot\n";
    // }

    Scalar pressureA = A.GetPressure() + A.GetPressureGradient() * linkA.altitudeRelativeToNode;
    Scalar pressureB = B.GetPressure() + B.GetPressureGradient() * linkB.altitudeRelativeToNode;

    Meter deltaAltitude = MmToMeter(B.GetAltitudeMm() - A.GetAltitudeMm());

    Scalar viscosity = 0.1;

    //Scalar pressureADeltaN = pressureA * m_section * viscosity;
    //Scalar pressureBDeltaN = pressureB * m_section * viscosity;

    //Quantity transfertN = MAX(0, abs(finalDeltaN));


    float diffusionRatio = 0;
    // TODO

    Scalar deltaMass = 0;

    for(int sourceIndex = 0; sourceIndex < LinkCount; sourceIndex++)
    {
        NodeLink& link = m_links[sourceIndex];
        GasNode& sourceNode = *((GasNode*) link.node);
        int destinationIndex = 1 -sourceIndex;

        Scalar pressure = sourceNode.GetPressure() + sourceNode.GetPressureGradient() * link.altitudeRelativeToNode;
        Scalar pressureDeltaN = diffusionRatio * pressure * m_section * viscosity / sourceNode.GetTemperature();

        Quantity sourceTotalN = sourceNode.GetN();
        Quantity transfertN = Quantity(pressureDeltaN);


        if(transfertN > 0)
        {

            for(Gas gas : AllGas())
            //for(size_t gazIndex = 0; gazIndex < GasCount; gazIndex++)
            {
                // TODO diffusion
                Scalar compositionRatio = Scalar(sourceNode.GetN(gas)) / Scalar(sourceTotalN);
                Quantity quantity = Quantity(transfertN * compositionRatio);
                m_links[sourceIndex].outputMaterial[gas] -= quantity;
                m_links[destinationIndex].outputMaterial[gas] += quantity;

                Scalar mass = quantity * PhysicalConstants::GetMassByN(gas);
                deltaMass += mass;
            }

            // Take energy ratio
            Scalar takenRatio = Scalar(transfertN) / sourceTotalN;
            Energy takenEnergy = Energy(takenRatio * sourceNode.GetEnergy());
            m_links[sourceIndex].outputThermalEnergy -= takenEnergy;
            m_links[destinationIndex].outputThermalEnergy += takenEnergy;
        }
    }


    // Optimal deltaN

    //PVNRT1 - 



    // Apply pressure acceleration
    Scalar pressureDiff = pressureA - pressureB;
    Scalar kineticAcceleration = pressureDiff * m_section * viscosity * PhysicalConstants::kineticCoef;
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

    if(kineticEnergy > 0)
    {
        GasNode& sourceNode = *((GasNode*) m_links[sourceIndex].node);

        Scalar pressureDeltaN = kineticEnergy / (PhysicalConstants::kineticCoef * sourceNode.GetTemperature());



        Quantity sourceTotalN = sourceNode.GetN();
        Quantity sourceTotalUsableN = sourceTotalN / sourceNode.GetTransitionLinks().size();


        Quantity totalTransfertN = std::min(sourceTotalUsableN, Quantity(pressureDeltaN));


        if(totalTransfertN > 0)
        {
            auto TakeComposition = [&](bool useOutput, Quantity transfertN, Quantity totalN)
            {
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
                    deltaMass += mass;
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
                m_links[sourceIndex].outputThermalEnergy -= takenEnergy;
                m_links[destinationIndex].outputThermalEnergy += takenEnergy;
            };

            Quantity sourceTotalOutputN = sourceNode.GetOutputN();
            Quantity outputTransfertN = std::min(totalTransfertN, sourceTotalOutputN);

            if(outputTransfertN)
            {
                TakeComposition(true, outputTransfertN, sourceTotalOutputN);
            }

            if(outputTransfertN < totalTransfertN)
            {
                Quantity inputTransfertN = totalTransfertN - outputTransfertN;
                TakeComposition(false, inputTransfertN, sourceNode.GetInputN());
            }
        }
        else
        {
            // No composition movement, disperse thermal energy
            newKineticEnergyDelta = 0;
        }
        
    }


    // Absorb or add kinetic energy from altitude change
#if 0
    if(deltaAltitude != 0)
    {
        Energy deltaPotentialEnergy = -deltaAltitude * deltaMass * PhysicalConstants::gravity * PhysicalConstants::energyPerJoule;

        if(deltaPotentialEnergy != 0)
        {
            if(newKineticEnergyDelta * deltaPotentialEnergy > 0)
            {
                // Kinetic energy in the right direction, accelerate
                newKineticEnergyDelta += deltaPotentialEnergy;
            }
            else
            {
                // Kinetic energy in the opposite direction, remove as much acceleration as possible
                if(abs(deltaPotentialEnergy) > abs(newKineticEnergyDelta))
                {
                    deltaPotentialEnergy += newKineticEnergyDelta;
                    newKineticEnergyDelta = 0;

                    // Take the remaining energy delta in the source node
                    m_links[sourceIndex].outputThermalEnergy -= abs(deltaPotentialEnergy);
                }
                else
                {
                    newKineticEnergyDelta += deltaPotentialEnergy;
                }
            }
        }
    }
#endif

    assert(m_links[sourceIndex].outputKineticEnergy == 0);

    /*// Apply pressure acceleration
    Scalar pressureDiff = pressureA - pressureB;
    Scalar kineticAcceleration = pressureDiff * m_section * viscosity * KineticCoef;
    Energy newKineticEnergyDelta = kineticEnergyDelta + kineticAcceleration;*/
    size_t newSourceIndex;
    size_t newDestinationIndex;
    //TODO energy dissipation

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
    Energy thermalLoss = newKineticEnergyBeforeLoss * 0.001;
    Energy newKineticEnergy = newKineticEnergyBeforeLoss - thermalLoss;


    Energy energyDiff = newKineticEnergy - kineticEnergySum;

    m_links[newDestinationIndex].outputKineticEnergy = newKineticEnergy;

    Energy sourceEnergyDiff =  energyDiff /2;
    Energy destinationEnergyDiff =  energyDiff - sourceEnergyDiff;

    /*if(energyDiff < 0)
    {
        m_links[newSourceIndex].outputThermalEnergy -= energyDiff;
    }
    else
    {
        m_links[newDestinationIndex].outputThermalEnergy -= energyDiff;
    }*/

    m_links[newSourceIndex].outputThermalEnergy -= sourceEnergyDiff;
    m_links[newDestinationIndex].outputThermalEnergy -= destinationEnergyDiff;

/*
    if(energyDiff > 0)
    {
        m_links[newSourceIndex].outputThermalEnergy -= energyDiff;
    }
    else
    {
        m_links[newDestinationIndex].outputThermalEnergy -= energyDiff;
    }*/


    Energy finalCheckEnergy = 0;
    for(int i = 0; i < LinkCount; i++)
    {
        finalCheckEnergy += m_links[i].outputThermalEnergy;
        finalCheckEnergy += m_links[i].outputKineticEnergy;
        assert(m_links[i].inputKineticEnergy == 0);
    }

    assert(std::abs(initialCheckEnergy - finalCheckEnergy) < 1e-8);
}

}