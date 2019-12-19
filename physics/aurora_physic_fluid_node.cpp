#include "aurora_physic_fluid_node.h"


namespace aurora {

FluidNode::~FluidNode()
{

}

void FluidNode::AddTransition(TransitionLink const& transitionLink)
{
    m_transitionLinks.push_back(transitionLink);
}


}