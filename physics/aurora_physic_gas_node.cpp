#include "aurora_physic_gas_node.h"
#include <assert.h>
#include "aurora_physic_constants.h"
#include "aurora_physic_types.h"

namespace aurora {


GasNode::GasNode()
    : m_altitude(0)
    , m_volume(0)
    , m_thermalEnergy(0)
{
    for(Gas gas : AllGas())
    {
        m_nComposition[gas] = 0;
    }
}

GasNode::GasNode(GasNode& node)
    : m_altitude(node.m_altitude)
    , m_volume(node.m_volume)
    , m_thermalEnergy(node.m_thermalEnergy)
    , m_cacheComputed(false)
{
    for(Gas gas : AllGas())
    {
        m_nComposition[gas] = node.m_nComposition[gas];
    }
}

void GasNode::SetVolume(Volume volume)
{
    m_volume = volume;
    m_cacheComputed = false;
}


Volume GasNode::GetVolume() const
{
    return m_volume;
}

void GasNode::AddN(Gas gas, Quantity N)
{
    m_nComposition[gas] += N;
    m_cacheComputed = false;
}

void GasNode::AddThermalEnergy(Energy thermalEnergy)
{
    m_thermalEnergy += thermalEnergy;
    m_cacheComputed = false;
}

void GasNode::TakeN(Gas gas, Quantity N)
{
    m_nComposition[gas] -= N;
    m_cacheComputed = false;
}

void GasNode::TakeThermalEnergy(Energy thermalEnergy)
{
    m_thermalEnergy -= thermalEnergy;
    m_cacheComputed = false;
}

Quantity GasNode::GetN(Gas gas) const
{
    return m_nComposition[gas];
}

Energy GasNode::GetThermalEnergy() const
{
    return m_thermalEnergy;
}

void GasNode::PrepareTransitions()
{
    MigrateKineticEnergy();
}

void GasNode::MigrateKineticEnergy()
{
    Energy energyByDirection[4];
    Scalar sectionSumByDirection[4];

    for(int i = 0; i < 4 ; i++)
    {
        energyByDirection[i] = 0;
        sectionSumByDirection[i] = 0;
    }

//   auto ComputeEnergy = [&]()
//     {
//         Energy energy = m_thermalEnergy;
//         for(TransitionLink& transitionLink : m_transitionLinks)
//         {
//             Transition::NodeLink* link = transitionLink.transition->GetNodeLink(transitionLink.index);
//             energy += link->outputKineticEnergy;
//             energy += link->inputKineticEnergy;
//         }
//         for(int i = 0; i < 4 ; i++)
//         {
//             energy += energyByDirection[i];
//         }

//         return energy;
//     };

    //Energy initialEnergyCheck = ComputeEnergy();


    Scalar sectionSum = 0;
    
    auto TakeEnergy = [&energyByDirection](Transition::Direction direction, Scalar ratio) 
    {
        Energy takenEnergy  = Energy(energyByDirection[direction] * ratio);
        energyByDirection[direction] -= takenEnergy;
        return takenEnergy;
    };

    const float DiffusionFactor = 1.0; // TODO remove if 1

    for(TransitionLink& transitionLink : m_transitionLinks)
    {

        Transition::NodeLink* link = transitionLink.transition->GetNodeLink(transitionLink.index);

        Transition::Direction linkDirection = transitionLink.transition->GetDirection(transitionLink.index);
        
        Energy energyToDispatch = link->outputKineticEnergy;

        Energy forwardEnergy = energyToDispatch * DiffusionFactor;
        energyToDispatch -= forwardEnergy;

        Energy leftEnergy = energyToDispatch / 2;
        energyToDispatch -= leftEnergy;

        energyByDirection[linkDirection.Opposite()] += forwardEnergy;
        energyByDirection[linkDirection.Next()] += leftEnergy;
        energyByDirection[linkDirection.Previous()] += energyToDispatch;
        link->outputKineticEnergy = 0;
        
        sectionSumByDirection[linkDirection] += transitionLink.transition->GetSection();
        sectionSum += transitionLink.transition->GetSection();
    }

    // If not section in front, divert to side
    for(int i = 0; i < 4 ; i++)
    {
        if(sectionSumByDirection[i] == 0 && energyByDirection[i] > 0)
        {
            uint8_t leftIndex = i+1;
            uint8_t rightIndex = (i+3) & 3;

            Energy leftEnergy = energyByDirection[i] / 2;
            energyByDirection[i] -= leftEnergy;
            energyByDirection[leftIndex] += leftEnergy;
            energyByDirection[rightIndex] += energyByDirection[i];
            energyByDirection[i] = 0;
        }
    }


    // Transform opposite direction as heat or reflect in opposite directions

    const float DiversionRatio = 1.0; // TODO remove if 0

    Energy divertedEnergy[2];

    for(int i = 0; i < 2; i++)
    {
        Energy energyDiff = energyByDirection[i] - energyByDirection[i+2];
        divertedEnergy[i] = energyByDirection[i] + energyByDirection[i+2] - std::abs(energyDiff);
        if(energyDiff > 0)
        {
            energyByDirection[i] = energyDiff;
            energyByDirection[i + 2] = 0;
        }
        else if(energyDiff < 0)
        {
            energyByDirection[i] = 0;
            energyByDirection[i + 2] = -energyDiff;
        }
        else
        {
            energyByDirection[i] = 0;
            energyByDirection[i + 2] = 0;
        }
        
        

        {
            //Energy finalEnergyCheck = ComputeEnergy();
            //assert(finalEnergyCheck == initialEnergyCheck);
        }
    }

    for(int i = 0; i < 2; i++)
    {
        assert(energyByDirection[i] * energyByDirection[i+2] == 0);
    }

    for(int i = 0; i < 2; i++)
    {
        Energy thermalLoss = divertedEnergy[i] * (1-DiversionRatio);
        m_thermalEnergy += thermalLoss;
        divertedEnergy[i] -= thermalLoss;

        uint8_t leftIndex = i+1;
        uint8_t rightIndex = (i+3) & 3;

        if(energyByDirection[leftIndex] == 0 && energyByDirection[rightIndex] == 0)
        {
            // No flow, divert in both direction
            Energy leftEnergy = divertedEnergy[i] / 2;
            divertedEnergy[i] -= leftEnergy;
            energyByDirection[leftIndex] += leftEnergy;
            energyByDirection[rightIndex] += divertedEnergy[i];
        }
        else if (energyByDirection[leftIndex] != 0)
        {
            // Left have flow, use it
            energyByDirection[leftIndex] += divertedEnergy[i];
        }
        else
        {
            assert(energyByDirection[rightIndex] != 0);
            energyByDirection[rightIndex] += divertedEnergy[i];
        }        
    }

    for(int i = 0; i < 2; i++)
    {
       // assert(energyByDirection[i] * energyByDirection[i+2] == 0);
    }
    

    for(TransitionLink& transitionLink : m_transitionLinks)
    {

        Transition::NodeLink* link = transitionLink.transition->GetNodeLink(transitionLink.index);
        Transition::Direction linkDirection = transitionLink.transition->GetDirection(transitionLink.index);

        Scalar section = transitionLink.transition->GetSection();
        Scalar sectionRatio = section / sectionSumByDirection[linkDirection];

        link->inputKineticEnergy += TakeEnergy(linkDirection , sectionRatio);

        sectionSumByDirection[linkDirection] -= section;

        assert(link->inputKineticEnergy  >= 0);
    }

    // Find remaining ene

/*
    for(int i = 0; i < 4 ; i++)
    {
        if(energyByDirection[i] > 0)
        {
            // Rebound or absorb
            // For now rebound to all transitions
            Scalar localSectionSum = sectionSum;

            for(TransitionLink& transitionLink : m_transitionLinks)
            {
                Transition::NodeLink* link = transitionLink.transition->GetNodeLink(transitionLink.index);

                Scalar section = transitionLink.transition->GetSection();
                Scalar sectionRatio = section / localSectionSum;

                link->inputKineticEnergy += TakeEnergy(Transition::Direction(i) , sectionRatio);

                localSectionSum -= section;
                assert(link->inputKineticEnergy  >= 0);
            }
        }
    }*/

        for(int i = 0; i < 4 ; i++)
    {
        if(energyByDirection[i] > 0)
        {
            m_thermalEnergy += TakeEnergy(Transition::Direction(i) , 1.0f);
        }
    }

    
    for(int i = 0; i < 4 ; i++)
    {
        assert(energyByDirection[i] == 0);
    }

    {
        //Energy finalEnergyCheck = ComputeEnergy();
        //assert(finalEnergyCheck == initialEnergyCheck);
    }
}

void GasNode::ApplyTransitions()
{
    // Apply transition output
    for(TransitionLink& transitionLink : m_transitionLinks)
    {
        Transition::NodeLink* link = transitionLink.transition->GetNodeLink(transitionLink.index);
        assert(link->node == this);

        m_thermalEnergy += link->outputThermalEnergy;
        link->outputThermalEnergy = 0;

        for(Gas gas : AllGas())
        {
            m_nComposition[gas] += link->outputMaterial[gas];
            link->outputMaterial[gas] = 0;
        }
    }

    //MigrateKineticEnergy();
}

void GasNode::ComputeCache()
{
    // Compute N, energy and mass
    m_cacheCheckN = 0;
    m_cacheMass = 0;
    Energy energyPerK = 0;
     for(Gas gas : AllGas())
    {
        Quantity materialN = m_nComposition[gas];

        m_cacheCheckN += materialN;
        if(materialN > 0)
        {
            energyPerK += PhysicalConstants::GetSpecificHeatByN(gas) * materialN;
            m_cacheMass += PhysicalConstants::GetMassByN(gas) * materialN;
        }
    }

    if(m_cacheCheckN < 0)
    {
        m_cacheN = 0;
    }
    else
    {
        m_cacheN = m_cacheCheckN;
    }

    // Compute temperature
    if(energyPerK == 0)
    {
        m_cacheTemperature = 0;
    }
    else
    {
        m_cacheTemperature = m_thermalEnergy / energyPerK;
    }

    if(m_cacheTemperature < 0)
    {
        m_cacheTemperature = 0;
    }

    // Compute pressure
     m_cachePressure = PhysicalConstants::ComputePressure(m_cacheN, m_volume, m_cacheTemperature);
    if(m_cachePressure < 0)
    {
        m_cachePressure = 0;
    }

    Scalar density = Scalar(m_cacheMass) / m_volume;
    m_cachePressureGradient = density *  PhysicalConstants::gravity;

    m_cacheComputed = true;
}

Scalar GasNode::GetPressure() const
{
    assert(m_cacheComputed);
    return m_cachePressure;
}

Scalar GasNode::GetTemperature() const
{
    assert(m_cacheComputed);
    return m_cacheTemperature;
}

Quantity GasNode::GetN() const
{
    assert(m_cacheComputed);
    return m_cacheN;
}

Scalar GasNode::GetPressureGradient() const
{
    assert(m_cacheComputed);
    return m_cachePressureGradient;
}

Energy GasNode::GetEnergy() const
{
    if(m_thermalEnergy > 0)
    {
        return m_thermalEnergy;
    }
    else
    {
        return 0;
    }
}

}