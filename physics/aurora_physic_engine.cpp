#include "aurora_physic_engine.h"

#include "aurora_physic_gas_node.h"
#include "aurora_physic_constants.h"

#include <assert.h>
#include <stdio.h>

namespace aurora {

PhysicEngine::PhysicEngine()
    : m_stepState(TRANSITION_APPLIED)
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
     Scalar initialTotalEnergy = ComputeEnergy("initial");
    __int128 initialTotalN = 0;
    for(FluidNode* node : m_nodes)
    {
       // initialTotalEnergy += node->GetCheckEnergy();
        initialTotalN += node->GetCheckN();
    }

    if(m_stepState !=  TRANSITION_APPLIED)
    {
        SubStep(delta);
    }
    else
    {
        PrepareTransitions();
        ComputeTransitions(delta);
        ApplyTransitions();
    }

     __int128 finalTotalN = 0;
    for(FluidNode* node : m_nodes)
    {
        finalTotalN += node->GetCheckN();
    }

    Scalar check = ComputeEnergy("after all step");
    assert((initialTotalEnergy - check) < 1e-2);
    assert(initialTotalN == finalTotalN);


#if 1
    {
        for(int i = 0; i < 4; i++)
        {
            GasNode* node =(GasNode*) m_nodes[80*(40+i)+70];
            //node->AddThermalEnergy(10000 * node->GetN());
            node->AddThermalEnergy(2.0 * node->GetN());
            node->ComputeCache();
            //printf("N=%ld\n",node->GetN());
        }
    }
#endif

#if 1
    {
        for(int i = 0; i < 80; i++)
        {
            GasNode* node =(GasNode*) m_nodes[i * 80];
            //node->AddThermalEnergy(10000 * node->GetN());

            //node->AddThermalEnergy(-0.1 * node->GetN());
            node->TakeThermalEnergy(0.0002 * node->GetThermalEnergy());
            node->ComputeCache();
            //printf("N=%ld\n",node->GetN());
        }
    }
#endif
}

void PhysicEngine::SubStep(Scalar delta)
{
    Scalar initialTotalEnergy = ComputeEnergy("initial");
    __int128 initialTotalN = 0;
    for(FluidNode* node : m_nodes)
    {
       // initialTotalEnergy += node->GetCheckEnergy();
        initialTotalN += node->GetCheckN();
    }


    switch (m_stepState)
    {
    case TRANSITION_APPLIED:
        PrepareTransitions();
        m_stepState = TRANSITION_PREPARED;
        break;
    case TRANSITION_PREPARED:
        ComputeTransitions(delta);
        m_stepState = TRANSITION_COMPUTED;
    break;
    case TRANSITION_COMPUTED:
        ApplyTransitions();
        m_stepState = TRANSITION_APPLIED;
    break;
    default:
        break;
    }

     __int128 finalTotalN = 0;
    for(FluidNode* node : m_nodes)
    {
        finalTotalN += node->GetCheckN();
    }

    

    Scalar check = ComputeEnergy("after all step");
    assert((initialTotalEnergy - check) < 1e-8);
    assert(initialTotalN == finalTotalN);

}

Energy PhysicEngine::ComputeEnergy(const char* label)
{
    Energy energy = 0;
    for(FluidNode* node : m_nodes)
    {
        energy += node->GetCheckEnergy();
        if(node->GetCheckEnergy() < 0)
        {
                printf("Node %p e=%g (%s)\n", node, node->GetCheckEnergy(), sprintf_int128(energy));
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
                printf("Transition inputKineticEnergy=%g outputThermalEnergy=%g outputKineticEnergy=%g (%s)\n",
                transition->GetNodeLink(i)->inputKineticEnergy,
                transition->GetNodeLink(i)->outputThermalEnergy,
                transition->GetNodeLink(i)->outputKineticEnergy, sprintf_int128(energy));
        }
        }
    }

    //printf("ComputeEnergy %s %s\n", label, sprintf_int128(energy));

    return energy;
}

void PhysicEngine::PrepareTransitions()
{
    for(FluidNode* node : m_nodes)
    {
        node->PrepareTransitions();
        // TODO move to post step to remove one compute cache ?
        node->ComputeCache();

        //__int128 check = ComputeEnergy("after prepare");
        //assert(initialTotalEnergy == check);
    }
}

void PhysicEngine::ComputeTransitions(Scalar delta)
{
    static int stepCount = 0;
    printf("PhysicEngine::Step %d\n", stepCount);
    stepCount++;

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
}


void PhysicEngine::ApplyTransitions()
{

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

