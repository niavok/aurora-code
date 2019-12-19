#include "aurora_physic_transition.h"


namespace aurora {

Transition::Direction Transition::GetDirection(size_t index) const
{
    if(index == 0)
    {
        return m_direction;
    }
    else
    {
        return m_direction.Opposite();
    }
}

}
