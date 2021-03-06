#include "aurora_physic_gas_node.h"
#include <assert.h>
#include "aurora_physic_constants.h"
#include "aurora_physic_types.h"

#include <stdio.h>

namespace aurora {


GasNode::GasNode()
    : m_centerAltitude(0)
    , m_volume(0)
    , m_height(0)
    , m_inputInternalEnergy(0)
    , m_outputInternalEnergy(0)
    //, m_movingN(0)
{
    for(Gas gas : AllGas())
    {
        m_inputNComposition[gas] = 0;
        m_outputNComposition[gas] = 0;
    }
}

GasNode::GasNode(GasNode& node)
    : m_centerAltitude(node.m_centerAltitude)
    , m_volume(node.m_volume)
    , m_inputInternalEnergy(node.m_inputInternalEnergy)
    , m_outputInternalEnergy(node.m_outputInternalEnergy)
    //, m_movingN(node.m_movingN)
    , m_cacheComputed(false)
{
    for(Gas gas : AllGas())
    {
        m_inputNComposition[gas] = node.m_inputNComposition[gas];
        m_outputNComposition[gas] = node.m_outputNComposition[gas];
    }
}

void GasNode::ClearContent()
{
    m_inputInternalEnergy = 0;
    m_outputInternalEnergy = 0;
    for(Gas gas : AllGas())
    {
        m_inputNComposition[gas] = 0;
        m_outputNComposition[gas] = 0;
    }
    m_cacheComputed = false;
}

void GasNode::SetVolume(Volume volume)
{
    m_volume = volume;
    m_cacheComputed = false;
}

void GasNode::SetHeight(Meter height)
{
    m_height = height;
}

void GasNode::SetCenterAltitude(Meter centerAltitude)
{
    m_centerAltitude = centerAltitude;
}


Volume GasNode::GetVolume() const
{
    return m_volume;
}

void GasNode::AddN(Gas gas, Quantity N)
{
    m_outputNComposition[gas] += N;
    m_cacheComputed = false;
}

void GasNode::AddInternalEnergy(Energy internalEnergy)
{
    m_outputInternalEnergy += internalEnergy;
    m_cacheComputed = false;
}

void GasNode::TakeInternalEnergy(Energy internalEnergy)
{
    m_outputInternalEnergy -= internalEnergy;
    m_cacheComputed = false;
}

Quantity GasNode::GetN(Gas gas) const
{
    return m_inputNComposition[gas] + m_outputNComposition[gas];
}

Quantity GasNode::GetInputN(Gas gas) const
{
    return m_inputNComposition[gas];
}

Quantity GasNode::GetOutputN(Gas gas) const
{
    return m_outputNComposition[gas];
}

void GasNode::PrepareTransitions()
{
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
    /*for(int i = 0; i < 4 ; i++)
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
    }*/


#if 1
    // Transform opposite direction as heat
    Energy divertedEnergy = 0;
    for(int i = 0; i < 2; i++)
    {
        Energy energyDiff = energyByDirection[i] - energyByDirection[i+2];

        divertedEnergy += energyByDirection[i] + energyByDirection[i+2] - std::abs(energyDiff);
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
    }

    m_inputInternalEnergy += divertedEnergy;
    assert(!isnan(m_inputInternalEnergy));

#endif

    // Transform opposite direction as heat or reflect in opposite directions

    /*const float DiversionRatio = 1.0; // TODO remove if 0

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
    }*/

    /*for(int i = 0; i < 2; i++)
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
*/
    for(int i = 0; i < 2; i++)
    {
       // assert(energyByDirection[i] * energyByDirection[i+2] == 0);
    }


    // Dispatch energy
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
            m_inputInternalEnergy += TakeEnergy(Transition::Direction(i) , 1.0f);
            assert(!isnan(m_inputInternalEnergy));
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
    //m_movingN = 0;
    bool isFlushNeeded = false;

    // Apply transition output
    for(TransitionLink& transitionLink : m_transitionLinks)
    {
        // First, take from output buffer, then if missing some energy or N, flush input buffer

        Transition::NodeLink* link = transitionLink.transition->GetNodeLink(transitionLink.index);
        assert(link->node == this);

        if(link->outputInternalEnergy > 0)
        {
            m_inputInternalEnergy += link->outputInternalEnergy;
            assert(!isnan(m_inputInternalEnergy));
        }
        else
        {
            m_outputInternalEnergy += link->outputInternalEnergy;
            if(m_outputInternalEnergy < 0)
            {
                isFlushNeeded = true;
            }
        }
        link->outputInternalEnergy = 0;

        for(Gas gas : AllGas())
        {
            Quantity quantity = link->outputMaterial[gas];

            if(quantity == 0)
            {
                continue;
            }

            if(quantity > 0)
            {
                m_inputNComposition[gas] += quantity;
                //m_movingN += quantity;
            }
            else
            {
                m_outputNComposition[gas] += quantity;
                if(m_outputNComposition[gas] < 0)
                {
                    isFlushNeeded = true;
                }
            }
            link->outputMaterial[gas] = 0;
        }
    }
    if(isFlushNeeded)
    {
        FlushInput();
    }

    MigrateKineticEnergy();
}

void GasNode::FlushInput()
{
    m_outputInternalEnergy += m_inputInternalEnergy;
    m_inputInternalEnergy = 0;

    for(Gas gas : AllGas())
    {
        m_outputNComposition[gas] += m_inputNComposition[gas];
        m_inputNComposition[gas] = 0;
    }
}

void GasNode::ComputeCache()
{
    // Compute N, energy and mass
    m_cacheCheckN = 0;
    m_cacheMass = 0;
    m_cacheEnergyPerK = 0;
    Quantity totalInputN = 0;
    Quantity totalOutputN = 0;
    Scalar inputEnergyPerK = 0;
    Scalar outputEnergyPerK = 0;

    assert(!isnan(m_inputInternalEnergy));
    assert(!isnan(m_outputInternalEnergy));
    assert(m_outputInternalEnergy >= 0);

     for(Gas gas : AllGas())
    {
        Quantity inputMaterialN = m_inputNComposition[gas];
        Quantity outputMaterialN = m_outputNComposition[gas];

        Quantity materialN = inputMaterialN + outputMaterialN;
        totalInputN += m_inputNComposition[gas];
        totalOutputN += m_outputNComposition[gas];

        m_cacheCheckN += materialN;
        if(materialN > 0)
        {
            Scalar specificHeatByN = PhysicalConstants::GetSpecificHeatByN(gas);
            m_cacheEnergyPerK += specificHeatByN * materialN;
            inputEnergyPerK += specificHeatByN * inputMaterialN;
            outputEnergyPerK += specificHeatByN * outputMaterialN;

            m_cacheMass += PhysicalConstants::GetMassByN(gas) * materialN;
        }
    }

    assert(totalOutputN >= 0);

    if(m_cacheCheckN < 0)
    {
        m_cacheN = 0;
    }
    else
    {
        m_cacheN = m_cacheCheckN;
    }

    if(m_cacheN > 0)
    {
        m_cacheMolarMass = m_cacheMass / m_cacheN;
    }
    else
    {
        m_cacheMolarMass = 0;
    }

    if(totalInputN < 0)
    {
        m_cacheInputN = 0;
    }
    else
    {
        m_cacheInputN = totalInputN;
    }

    if(totalOutputN < 0)
    {
        m_cacheOutputN = 0;
    }
    else
    {
        m_cacheOutputN = totalOutputN;
    }

    // Compute elastic energy
    //Quantity pressureN = m_cacheN - m_movingN; // Exclude moving N from pressure computation
    Quantity pressureN = m_cacheN;
	Energy internalEnergy = m_inputInternalEnergy + m_outputInternalEnergy;
	if(pressureN == 0)
	{
		m_cachePressure = 0;
	}
	else
	{
    	Scalar A = pressureN * PhysicalConstants::gasConstant / (m_volume * m_cacheEnergyPerK);
    	m_cachePressure = A * internalEnergy / (1 + A*m_volume * PhysicalConstants::gasElasticCoef);
	}
	assert(m_cachePressure >= 0);

    Energy elasticEnergy = PhysicalConstants::gasElasticCoef * m_cachePressure * m_volume;
    Energy thermalEnergy = internalEnergy - elasticEnergy;
    m_cacheThermalEnergyRatio = thermalEnergy / internalEnergy;


    // Compute temperature
    if(m_cacheEnergyPerK == 0)
    {
        m_cacheTemperature = 0;
    }
    else
    {
        m_cacheTemperature = thermalEnergy / m_cacheEnergyPerK;
    }

    if(inputEnergyPerK == 0)
    {
        m_cacheInputTemperature = 0;
    }
    else
    {
        Energy inputThermalEnergy = m_cacheThermalEnergyRatio * m_inputInternalEnergy;
        m_cacheInputTemperature = inputThermalEnergy / inputEnergyPerK;
    }

    if(outputEnergyPerK == 0)
    {
        m_cacheOutputTemperature = 0;
    }
    else
    {
        Energy outputThermalEnergy = m_cacheThermalEnergyRatio * m_outputInternalEnergy;
        m_cacheOutputTemperature = outputThermalEnergy / outputEnergyPerK;
    }

    if(m_cacheTemperature < 0)
    {
        m_cacheTemperature = 0;
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

Scalar GasNode::GetInputTemperature() const
{
    assert(m_cacheComputed);
    return m_cacheInputTemperature;
}

Scalar GasNode::GetOutputTemperature() const
{
    assert(m_cacheComputed);
    return m_cacheOutputTemperature;
}

Quantity GasNode::GetN() const
{
    assert(m_cacheComputed);
    return m_cacheN;
}

Quantity GasNode::GetOutputN() const
{
    assert(m_cacheComputed);
    return m_cacheOutputN;
}
Quantity GasNode::GetInputN() const
{
    assert(m_cacheComputed);
    return m_cacheInputN;
}
/*
Quantity GasNode::GetMovingN() const
{
    return m_movingN;
}*/

Scalar GasNode::GetPressureGradient() const
{
    assert(m_cacheComputed);
    return m_cachePressureGradient;
}

Energy GasNode::GetInternalEnergy() const
{
    Energy totalEnergy = m_inputInternalEnergy + m_outputInternalEnergy;
    assert(totalEnergy > 0);
    return totalEnergy;
}

Energy GasNode::GetThermalEnergy() const
{
    return GetInternalEnergy() * m_cacheThermalEnergyRatio;
}

Energy GasNode::GetOutputEnergy() const
{
    return m_outputInternalEnergy;
}

Energy GasNode::GetInputEnergy() const
{
    return m_inputInternalEnergy;
}


}
