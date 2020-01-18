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


char * sprintf_int128( __int128_t n ) {
    static char str[41] = { 0 };        // sign + log10(2**128) + '\0'
    char *s = str + sizeof( str ) - 1;  // start at the end
    bool neg = n < 0;
    if( neg )
        n = -n;
    do {
        *--s = "0123456789"[n % 10];    // save last digit
        n /= 10;                // drop it
    } while ( n );
    if( neg )
        *--s = '-';
    return s;
}

void PhysicEngine::Step(Scalar delta)
{
    static int stepCount = 0;
    printf("PhysicEngine::Step %d\n", stepCount);
    stepCount++;


    auto ComputeEnergy = [this](char* label)
    {
        __int128 energy = 0;
        for(FluidNode* node : m_nodes)
        {
            energy += node->GetCheckEnergy();
            if(node->GetCheckEnergy() < 0)
            {
                 printf("Node %p e=%lld (%s)\n", node, node->GetCheckEnergy(), sprintf_int128(energy));
            }
        }

        for(Transition* transition : m_transitions)
        {
            for(size_t i = 0; i < transition->GetNodeLinkCount(); i++)
            {
                assert(transition->GetNodeLink(i)->inputKineticEnergy >= 0);

                energy += transition->GetNodeLink(i)->inputKineticEnergy;
                energy += transition->GetNodeLink(i)->outputThermalEnergy;
                energy += transition->GetNodeLink(i)->outputKineticEnergy;

                if(energy < 0)
            {
                 printf("Transition inputKineticEnergy=%lld outputThermalEnergy=%lld outputKineticEnergy=%lld (%s)\n",
                  transition->GetNodeLink(i)->inputKineticEnergy,
                  transition->GetNodeLink(i)->outputThermalEnergy,
                   transition->GetNodeLink(i)->outputKineticEnergy, sprintf_int128(energy));
            }
            }
        }

        //printf("ComputeEnergy %s %s\n", label, sprintf_int128(energy));

        return energy;
    };

    // Check
    __int128 initialTotalEnergy = ComputeEnergy("initial");
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

        //__int128 check = ComputeEnergy("after prepare");
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

        //__int128 check = ComputeEnergy("after step");
        //printf("%s -> %s\n", sprintf_int128(initialTotalEnergy), sprintf_int128(check));
        //assert(initialTotalEnergy == check);
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

    __int128 check = ComputeEnergy("after all step");
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


#if 0
    {
        GasNode* node =(GasNode*) m_nodes[597];
        //node->AddThermalEnergy(10000 * node->GetN());
        node->AddThermalEnergy(0.1 * node->GetN());
        node->ComputeCache();
        printf("N=%ld\n",node->GetN());
    }
#endif

#if 0
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

