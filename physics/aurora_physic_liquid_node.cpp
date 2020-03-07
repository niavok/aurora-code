#include "aurora_physic_liquid_node.h"


namespace aurora {

LiquidNode::LiquidNode(Liquid liquid)
    : m_liquid(liquid)
    , m_centerAltitude(0)
    , m_volume(0)
    , m_N(0)
    , m_dissolvedN(0)
    , m_thermalEnergy(0)
{
}

void LiquidNode::SetVolume(Volume volume)
{
    m_volume = volume;
}


Volume LiquidNode::GetVolume() const
{
    return m_volume;
}

Liquid LiquidNode::GetLiquid() const
{
    return m_liquid;
}

Quantity LiquidNode::GetN() const
{
    return m_N;
}

Quantity LiquidNode::GetDissolvedN() const
{
    return m_dissolvedN;
}

Energy LiquidNode::GetThermalEnergy() const
{
    return m_thermalEnergy;
}

void LiquidNode::AddN(Quantity N)
{
    m_N += N;
}

void LiquidNode::AddDissolvedN(Quantity N)
{
    m_dissolvedN += N;
}

void LiquidNode::AddThermalEnergy(Energy thermalEnergy)
{
    m_thermalEnergy += thermalEnergy;
}


void LiquidNode::TakeN(Quantity N)
{
    m_N -= N;
}

void LiquidNode::TakeDissolvedN(Quantity N)
{
    m_dissolvedN -= N;
}

void LiquidNode::TakeThermalEnergy(Energy thermalEnergy)
{
    m_thermalEnergy -= thermalEnergy;
}

}