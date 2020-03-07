#ifndef AURORA_PHYSIC_TRANSITION_H
#define AURORA_PHYSIC_TRANSITION_H

#include "aurora_physic_types.h"

namespace aurora {

class Transition;
class FluidNode;

struct TransitionLink
{
    TransitionLink(Transition* iTransition, size_t iIndex) : transition(iTransition), index(iIndex) {}

    Transition* transition;
    size_t index;
};

class Transition {
public:
    class Direction {
        public:
        enum Value : uint8_t {
            DIRECTION_UP,
            DIRECTION_RIGHT,
            DIRECTION_DOWN,
            DIRECTION_LEFT,
        };

        Direction() = default;
        constexpr Direction(Direction const& iDirection) : value(iDirection.value) { }
        constexpr Direction(Value iValue) : value(iValue) { }
        constexpr Direction(uint8_t iValue) : value(Value(iValue)) { }

        operator Value() const { return value; }

        Direction Opposite() const { 
            /*uint8_t value2 = value +2;
            uint8_t value3 = value2 & 3;
            Direction::Value value4 = Direction::Value(value3);
            Direction value5(value4);
            Direction value6((value + 2) & 3);

            //return value5;*/
            return Direction((value + 2) & 3);
        }

        Direction Next() const { 
            return Direction((value + 1) & 3);
        }

        Direction Previous() const { 
            return Direction((value + 3) & 3);
        }
        
        private:
        Value value;
    };

    Transition(Direction direction, Scalar section) : m_direction(direction), m_section(section) {}

    virtual ~Transition() {}
    virtual void Step(Scalar delta) = 0;
    virtual void ApplyInput() = 0;

    virtual FluidNode* GetNodeA() = 0;
    virtual FluidNode* GetNodeB() = 0;

    Direction GetDirection(size_t index) const;
    Scalar GetSection() const { return m_section; }

    struct NodeLink
    {
        NodeLink(FluidNode* iNode) : node(iNode) {}

        // Contants
        FluidNode* node;
        Meter altitudeRelativeToNode;
        Meter longitudeRelativeToNode;

        // Input
        Energy inputKineticEnergy;

        // Ouput
        Energy outputInternalEnergy;
        Energy outputKineticEnergy;
        GasComposition outputMaterial;
    };

    virtual NodeLink* GetNodeLink(size_t index) = 0;
    virtual size_t GetNodeLinkCount() = 0;


    virtual Energy GetKineticEnergy() const = 0;

protected:
    Direction m_direction;
    Scalar m_section;

};


}

#endif
