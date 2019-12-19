#include "aurora_physic_engine.h"

#include "aurora_physic_gas_node.h"
#include "aurora_physic_constants.h"

#include <assert.h>
#include <stdio.h>

namespace aurora {

PhysicEngine::PhysicEngine()
{
    PhysicalConstants::Init();
}

PhysicEngine::~PhysicEngine()
{
    for(Transition* transition : m_transitions)
    {
        delete transition;
    }

    m_transitions.clear();
}


void PhysicEngine::Flush()
{
    m_nodes.clear();

    if(!m_transitions.empty())
    {
        assert(false);
    }
}




void PhysicEngine::Step(Scalar delta)
{
    auto ComputeEnergy = [this]()
    {
        __int128 energy = 0;
        for(FluidNode* node : m_nodes)
        {
            energy += node->GetCheckEnergy();
        }

        for(Transition* transition : m_transitions)
        {
            for(size_t i = 0; i < transition->GetNodeLinkCount(); i++)
            {
                assert(transition->GetNodeLink(i)->inputKineticEnergy >= 0);

                energy += transition->GetNodeLink(i)->inputKineticEnergy;
                energy += transition->GetNodeLink(i)->outputThermalEnergy;
                energy += transition->GetNodeLink(i)->outputKineticEnergy;
            }
        }

        return energy;
    };

    // Check
    __int128 initialTotalEnergy = ComputeEnergy();
    __int128 initialTotalN = 0;
    for(FluidNode* node : m_nodes)
    {
       // initialTotalEnergy += node->GetCheckEnergy();
        initialTotalN += node->GetCheckN();
    }

    /*for(Transition* transition : m_transitions)
    {
        for(size_t i = 0; i < transition->GetNodeLinkCount(); i++)
        {
            assert(transition->GetNodeLink(i)->inputKineticEnergy >= 0);
        }
    }*/

    for(FluidNode* node : m_nodes)
    {
        node->PrepareTransitions();
        // TODO move to post step to remove one compute cache ?
        node->ComputeCache();

        //__int128 check = ComputeEnergy();
        
        //assert(initialTotalEnergy == check);
    }

    /*for(Transition* transition : m_transitions)
    {
        for(size_t i = 0; i < transition->GetNodeLinkCount(); i++)
        {
            assert(transition->GetNodeLink(i)->inputKineticEnergy >= 0);
        }
    }*/

    for(Transition* transition : m_transitions)
    {
        //__int128 check1 = ComputeEnergy();
        //assert(initialTotalEnergy == check1);
        transition->Step(delta);
        //__int128 check2 = ComputeEnergy();
        //assert(initialTotalEnergy == check2);
    }

    __int128 finalTotalEnergy = 0;
    __int128 finalTotalEnergyP1 = 0;
    __int128 finalTotalEnergyP2 = 0;
    __int128 finalTotalEnergyP3 = 0;

    __int128 finalTotalN = 0;
    for(FluidNode* node : m_nodes)
    {
        finalTotalN += node->GetCheckN();
    }

    __int128 check = ComputeEnergy();
// TODO REPAIR
    assert(initialTotalEnergy == check);
    assert(initialTotalN == finalTotalN);

    for(FluidNode* node : m_nodes)
    {
#if 0
        GasNode* gasNode = (GasNode*) node;
        // Artificial radiation cooling
        gasNode->TakeThermalEnergy(0.0001 * gasNode->GetThermalEnergy());
#endif


        node->ApplyTransitions();
        node->ComputeCache();
    }


#if 1
    {
        GasNode* node =(GasNode*) m_nodes[597];
        //node->AddThermalEnergy(10000 * node->GetN());
        node->AddThermalEnergy(0.1 * node->GetN());
        node->ComputeCache();
        printf("N=%ld\n",node->GetN());
    }
#endif

#if 1
    {
        GasNode* node =(GasNode*) m_nodes[123];
        //node->AddThermalEnergy(10000 * node->GetN());
        
        //node->AddThermalEnergy(-0.1 * node->GetN());
        node->TakeThermalEnergy(0.1 * node->GetThermalEnergy());
        node->ComputeCache();
        printf("N=%ld\n",node->GetN());
    }
#endif
}

void PhysicEngine::CheckDuplicates()
{
    for(size_t i = 0 ; i< m_transitions.size(); i++)
    {
        for(size_t j = i+1 ; j< m_transitions.size(); j++)
        {
            assert(m_transitions[i] != m_transitions[j]);
            assert(!(m_transitions[i]->GetNodeA() == m_transitions[j]->GetNodeA() && m_transitions[i]->GetNodeB() == m_transitions[j]->GetNodeB()));
            assert(!(m_transitions[i]->GetNodeA() == m_transitions[j]->GetNodeB() && m_transitions[i]->GetNodeB() == m_transitions[j]->GetNodeA()));
        }
    }
}

void PhysicEngine::AddTransition(Transition* transition)
{
    m_transitions.push_back(transition);
    transition->GetNodeA()->AddTransition(TransitionLink(transition, 0));
    transition->GetNodeB()->AddTransition(TransitionLink(transition, 1));
}

void PhysicEngine::AddFluidNode(FluidNode* node)
{
    m_nodes.push_back(node);
}


}

