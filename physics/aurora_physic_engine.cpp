#include "aurora_physic_engine.h"

#include "aurora_physic_gas_node.h"
#include "aurora_physic_constants.h"

#include "core/os/os.h"

#include <assert.h>
#include <stdio.h>

#define ENERGY_CHECK 0

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
    printf("PhysicEngine::Step %d", stepCount);
    stepCount++;

    uint64_t tsStart = OS::get_singleton()->get_ticks_usec();

    PrepareTransitions();
    uint64_t tsPrepareTransitions = OS::get_singleton()->get_ticks_usec();
    ComputeTransitions(delta);
    uint64_t tsComputeTransitions = OS::get_singleton()->get_ticks_usec();
    ApplyTransitionsToNodes();
    uint64_t tsApplyTransitions = OS::get_singleton()->get_ticks_usec();
    ApplyTransitionsInput();
    uint64_t tsApplyTransitionsInput = OS::get_singleton()->get_ticks_usec();

    int64_t totalDuration = tsApplyTransitionsInput - tsStart;
    int64_t prepareTransitionsDuration = tsPrepareTransitions - tsStart;
    int64_t computeTransitionsDuration = tsComputeTransitions - tsPrepareTransitions;
    int64_t applyTransitionsDuration = tsApplyTransitions - tsComputeTransitions;
    int64_t applyTransitionsInputDuration = tsApplyTransitionsInput - tsApplyTransitions;

    printf(" in %ld us (%ld us - %ld us - %ld us - %ld us)\n",
        totalDuration,
        prepareTransitionsDuration,
        computeTransitionsDuration,
        applyTransitionsDuration,
        applyTransitionsInputDuration);
}

Energy PhysicEngine::ComputeEnergy(const char* label)
{
    Energy energy = 0;
    for(FluidNode* node : m_nodes)
    {
        energy += node->GetInternalEnergy();
        if(node->GetInternalEnergy() < 0)
        {
                printf("Node %p e=%g (%s)\n", node, node->GetInternalEnergy(), sprintf_int128(energy));
        }
    }

    for(Transition* transition : m_transitions)
    {
        energy += abs(transition->GetKineticEnergy());

        for(size_t i = 0; i < transition->GetNodeLinkCount(); i++)
        {
            assert(transition->GetNodeLink(i)->inputKineticEnergy >= 0);

            energy += transition->GetNodeLink(i)->inputKineticEnergy;
            energy += transition->GetNodeLink(i)->outputInternalEnergy;
            energy += transition->GetNodeLink(i)->outputKineticEnergy;

            if(energy < 0)
        {
                printf("Transition inputKineticEnergy=%g outputThermalEnergy=%g outputKineticEnergy=%g (%s)\n",
                transition->GetNodeLink(i)->inputKineticEnergy,
                transition->GetNodeLink(i)->outputInternalEnergy,
                transition->GetNodeLink(i)->outputKineticEnergy, sprintf_int128(energy));
        }
        }
    }

    //printf("ComputeEnergy %s %s\n", label, sprintf_int128(energy));

    return energy;
}


void PhysicEngine::PrepareTransitions()
{
    // Prepare transition do nothing for now
    // for(FluidNode* node : m_nodes)
    // {
    //     node->PrepareTransitions();
    // }
}

void PhysicEngine::ApplyTransitionsInput()
{
    for(Transition* transition : m_transitions)
    {
        transition->ApplyInput();
    }
}

void PhysicEngine::ComputeTransitions(Scalar delta)
{
   

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


void PhysicEngine::ApplyTransitionsToNodes()
{

    for(FluidNode* node : m_nodes)
    {
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

