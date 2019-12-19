#ifndef AURORA_PHYSIC_FLUID_NODE_H
#define AURORA_PHYSIC_FLUID_NODE_H

#include <vector>
#include "aurora_physic_transition.h"

namespace aurora {

class FluidNode
{
public:

    virtual ~FluidNode();

    virtual void ComputeCache() = 0;
    virtual void PrepareTransitions() = 0;
    virtual void ApplyTransitions() = 0;
    virtual Energy GetEnergy() const = 0;
    virtual Energy GetCheckEnergy() const = 0;
    virtual Quantity GetCheckN() const = 0;

    void AddTransition(TransitionLink const& transitionLink);
    std::vector<TransitionLink>& GetTransitionLinks() { return m_transitionLinks; }
    std::vector<TransitionLink> const& GetTransitionLinks() const { return m_transitionLinks; }

protected:
    std::vector<TransitionLink> m_transitionLinks;
};

}

#endif
