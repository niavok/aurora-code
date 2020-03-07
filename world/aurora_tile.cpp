#include <cassert>
 #include <algorithm>

#include "aurora_tile.h"
#include "aurora_level.h"
#include "aurora_world.h"

#include "../physics/aurora_physic_constants.h"

namespace aurora {

Tile::Tile(Meter size, Meter2 bottomLeftPosition, Meter depth)
	: m_size(size)
    , m_depth(depth)
	, m_bottomLeftPosition(bottomLeftPosition)
    , m_centerAltitude(m_bottomLeftPosition.y + size *0.5)
    , m_totalVolume(sqr(m_size) * m_depth)
    , m_worldArea(m_bottomLeftPosition, Meter2(size, size))
{
    m_gasNode.SetCenterAltitude(m_centerAltitude);


    //m_content = new TileContent(GetVolume(), m_altitude); // TODO optimized in case of tile that will be splitted
	//printf("AuroraTile new AuroraTileContent %p\n", m_content);
}


Tile::~Tile()
{
	/*if(m_content)
	{
		//printf("AuroraTile delete AuroraTileContent %p\n", m_content);
		delete m_content;
	}*/
}


//void Tile::PaintTile(Rect2 area, AuroraMaterial const& material)
//{
//
//}

//void AuroraTile::Repack()
//{
//	if(!IsComposite())
//	{
//		// Nothing to repack
//		return;
//	}

//	bool hasCompositeChildAfterRepack = false;

//	// Try to repack child
//	for(AuroraTile* child: m_children)
//	{
//		child->Repack();
//		if(child->IsComposite())
//		{
//			hasCompositeChildAfterRepack = true;
//		}
//	}

//	if(!hasCompositeChildAfterRepack)
//	{
//		MaterialComposition& solidComposition = m_children[0]->GetContent()->GetSolidComposition();

//		bool isSameSolidComposition = true;

//		for(AuroraTile* child: m_children)
//		{
//            if(child->GetContent()->GetSolidComposition() != solidComposition)
//			{
//                isSameSolidComposition = false;
//                break;
//			}
//		}

//		if(isSameSolidComposition)
//		{
//			AuroraTileContent mergedContent(GetVolume());

//            for(AuroraTile* child: m_children)
//			{
//                mergedContent.AddGasComposition(child->GetContent()->GetGasComposition());
//                mergedContent.AddLiquidComposition(child->GetContent()->GetLiquidComposition());
//                mergedContent.AddSolidComposition(child->GetContent()->GetSolidComposition());
//				// TODO merge heat
//			}
//			SetContent(mergedContent);
//		}

//	}



//}

Volume Tile::GetTotalVolume() const
{
    return m_totalVolume;
}
/*
bool Tile::IsComposite() const
{
	return m_content == nullptr;
}*/

//void Tile::PaintContent(AuroraMaterial const& material)
//{
//	if(IsComposite())
//	{
//		for(AuroraTile* child: m_children)
//		{
//			delete child;
//		}
//		m_children.clear();
//		m_content = new AuroraTileContent(GetVolume());
//	}

//	m_content->SetMaterial(material);
//}
/*
void Tile::SetContent(TileContent const& content)
{
	if(IsComposite())
	{
        for(Tile* child: m_children)
		{
			delete child;
		}
		m_children.clear();
        m_content = new TileContent(GetVolume(), m_altitude);
	}

	m_content->SetContent(content);
}*/

Tile::InsideMode Tile::IsInside(MeterRect area)
{
    MeterRect intersection = area.Clip(m_worldArea);

    if(intersection.IsEmpty())
	{
		return No;
	}
    else if(intersection.size == m_worldArea.size)
	{
		return Yes;
	}
	else
	{
		return Partially;
	}
}
/*
bool Tile::Split(Level* level)
{
    if(IsComposite())
    {
        //printf("Split tile : alreaydy composite\n");
        // Already composite
        return true;
    }
    else if(m_size <= level->GetMinTileSizeMm())
    {
        //printf("Split tile : too small %f\n", m_size);
        // Too small, cannot split
        return false;
    }
    else
    {
        Mm childrenSize = m_size / Level::TileChildEdgeCount;

        //printf("do split childrenSize=%f\n", childrenSize);

        //TileContent childContent = m_content->Scaled(1 / AuroraWorld::TileChildCount);

        int proportionToTake = 4;

        for(int y = 0; y < Level::TileChildEdgeCount; y++)
        {
            for(int x = 0; x < Level::TileChildEdgeCount; x++)
            {
                Mm2 position = m_position + Mm2(x * childrenSize, y * childrenSize);

                Tile* newTile = new Tile(childrenSize, position);
                m_children.emplace_back(newTile);

                TileContent&& childContent = m_content->TakeProportion(proportionToTake);
                newTile->SetContent(childContent);

                //printf("newTile pos=%ls\n", String(newTile->GetPosition()).c_str());

                proportionToTake--;
            }
        }

        assert(m_content->IsEmpty());

        delete m_content;
        m_content = nullptr;

        return true;
    }
}*/
/*
void Tile::FindTileAt(std::vector<Tile*>& matchs, MmRect area)
{
    switch(IsInside(area))
    {
    case Tile::InsideMode::Yes:
    case Tile::InsideMode::Partially:
    {
        if(IsComposite())
        {
            for(Tile* child: GetChildren())
            {
                child->FindTileAt(matchs, area);
            }
        }
        else
        {
            matchs.push_back(this);
        }
    }
    break;
    case Tile::InsideMode::No:
    break;
    }
}
*/

////////////////////////
/// MaterialQuantity
////////////////////////

//MaterialQuantity::MaterialQuantity(Material pMaterial, Scalar pQuantity, Scalar pQuality)
//	: material(pMaterial)
//	, quantity(pQuantity)
//	, quality(pQuality)
//{
//}

////////////////////////
/// AuroraMaterial
////////////////////////

//void AuroraMaterial::SetSolidProperties(bool isPowder)
//{
//	m_isPowder = isPowder;
//}


//static void AddInComposition(MaterialQuantity& materialQuantity, std::vector<MaterialQuantity>& composition)
//{
//	for(MaterialQuantity& quantity : composition)
//	{
//		if(quantity.material == materialQuantity.material)
//		{
//            Scalar newQuantity = materialQuantity.quantity + quantity.quantity;
//            Scalar newQuality = (materialQuantity.quality * materialQuantity.quantity + quantity.quality * quantity.quantity) / newQuantity;

//			quantity.quantity = newQuantity;
//			quantity.quality = newQuality;
//			return;
//		}
//	}

//	// Not found
//	composition.emplace_back(materialQuantity);
//}

//void AuroraMaterial::AddSolidMaterial(MaterialQuantity solidQuantity)
//{
//	AddInComposition(solidQuantity, m_solidComposition);
//}
//void AuroraMaterial::AddLiquidMaterial(MaterialQuantity liquidQuantity)
//{
//	AddInComposition(liquidQuantity, m_liquidComposition);
//}
//void AuroraMaterial::AddGasMaterial(MaterialQuantity GasQuantity)
//{
//	AddInComposition(GasQuantity, m_GasComposition);
//}

//void AuroraMaterial::SetTemperature(Scalar temperature)
//{
//	// TODO
//}

//std::vector<MaterialQuantity> const& AuroraMaterial::GetSolidComposition() const
//{
//	return m_solidComposition;
//}

//std::vector<MaterialQuantity> const& AuroraMaterial::GetLiquidComposition() const
//{
//	return m_liquidComposition;
//}

//std::vector<MaterialQuantity> const& AuroraMaterial::GetGasComposition() const
//{
//	return m_GasComposition;
//}

//////////////////////////
///// AuroraTileContent
//////////////////////////
/*

TileContent::~TileContent()
{
    for (LiquidNode* liquidNode : m_liquidNodes)
    {
        delete liquidNode;
    }
}

*/
void Tile::AddSolid(Solid solid, Quantity N, Energy thermalEnergy)
{
    bool materialFound = false;

    for(SolidQuantity& quantity : m_solidComposition)
    {
        if(quantity.solid == solid)
        {
            quantity.N =  N;
            materialFound = true;
        }
    }

    if(!materialFound)
    {
        SolidQuantity newQuantity(solid, N);
        m_solidComposition.push_back(newQuantity);
    }

    m_solidThermalEnergy += thermalEnergy;

    UpdateVolumes();
}

void Tile::AddLiquid(Liquid liquid, Quantity N, Quantity dissolvedN, Energy thermalEnergy)
{
    LiquidNode* nodeToUse = nullptr;

    std::vector<LiquidNode*> m_liquidNodes;
    for(LiquidNode* node : m_liquidNodes)
    {
        if(node->GetLiquid() == liquid)
        {
            nodeToUse = node;
            break;
        }
    }

    if(nodeToUse == nullptr)
    {
        nodeToUse = new LiquidNode(liquid);
        m_liquidNodes.push_back(nodeToUse);
    }

    nodeToUse->AddN(N);
    nodeToUse->AddDissolvedN(dissolvedN);
    nodeToUse->AddThermalEnergy(thermalEnergy);

    UpdateVolumes();
}

void Tile::AddGas(Gas gas, Quantity N, Energy internalEnergy)
{
    m_gasNode.AddN(gas, N);
    m_gasNode.AddInternalEnergy(internalEnergy);

    UpdateVolumes();
}

void Tile::ClearContent()
{
    m_gasNode.ClearContent();
    m_liquidNodes.clear();
    m_solidComposition.clear();

    UpdateVolumes();
}

void Tile::UpdateVolumes()
{
    // Solid volume
    Volume maxSolidVolume = m_totalVolume;

    if(HasGas())
    {
        maxSolidVolume-=1e-9;
    }

    if(HasLiquid())
    {
        maxSolidVolume-=1e-9;
    }

    Volume needSolidVolume = 0;

    for(SolidQuantity& quantity : m_solidComposition)
    {
        Volume needVolume = PhysicalConstants::GetSolidVolumeByN(quantity.solid, quantity.N);
        needSolidVolume += needVolume;
    }

    if(needSolidVolume > maxSolidVolume)
    {
        assert(false);
        needSolidVolume = maxSolidVolume;
    }

    m_solidVolume = needSolidVolume;

    // Liquid volume
    Volume maxLiquidVolume = m_totalVolume - m_solidVolume;

    if(HasGas())
    {
        maxLiquidVolume-= 1e-9;
    }

    Volume needLiquidVolume = 0;

    for(LiquidNode* liquidNode: m_liquidNodes)
    {
        needLiquidVolume += liquidNode->GetVolume();
    }

    if(needLiquidVolume > maxLiquidVolume)
    {
        assert(false);
        Volume volumeToGain = maxSolidVolume - needLiquidVolume;
        for(LiquidNode* liquidNode: m_liquidNodes)
        {
            Volume volumeToTake = MIN(liquidNode->GetVolume() - 1e-9, volumeToGain);

            liquidNode->SetVolume(liquidNode->GetVolume() - volumeToTake);
            volumeToGain -= volumeToTake;
            if(volumeToGain <= 0)
            {
                break;
            }
        }

        assert(volumeToGain <= 0);
    }

    m_gasNode.SetVolume(GetGasVolume());
    m_gasNode.SetHeight(m_size);
}

Volume Tile::GetGasVolume() const
{
    return std::max(0., m_totalVolume - m_solidVolume - GetLiquidsVolume());
}

/*
Volume TileContent::GetTotalVolume() const
{
    return m_volume;
}
*/
Volume Tile::GetSolidsVolume() const
{
    return m_solidVolume;
}


Volume Tile::GetLiquidsVolume() const
{
    Volume volume = 0;

    for(LiquidNode* liquidNode: m_liquidNodes)
    {
        volume += liquidNode->GetVolume();
    }

    return volume;
}


bool Tile::HasSolid() const
{
    return m_solidComposition.size() > 0;
}

bool Tile::HasGas() const
{
    for (Gas gas : AllGas())
    {
        if(m_gasNode.GetN(gas) > 0)
        {
                return true;
        }
    }

    return false;
}

bool Tile::HasLiquid() const
{
    return m_liquidNodes.size() > 0;
}

bool Tile::IsEmpty() const
{
    return !HasGas() && !HasLiquid() && !HasSolid();
}

/*

void TileContent::SetContent(TileContent const& content)
{
    m_volume = content.m_volume;
    m_solidVolume = content.m_solidVolume;
    m_solidComposition = content.m_solidComposition;
    m_solidThermalEnergy = content.m_solidThermalEnergy;

    // Keep altitude
    Mm oldAltitude = m_gasNode.GetAltitudeMm();
    m_gasNode = content.m_gasNode;
    m_gasNode.SetAltitudeMm(oldAltitude);

    for(LiquidNode* liquidNode: content.m_liquidNodes)
    {
        m_liquidNodes.push_back(new LiquidNode(*liquidNode));
    }
}

TileContent TileContent::TakeProportion(int proportion)
{
    TileContent tileProportion(m_volume / proportion, 0);

    // Take solid
    std::vector<Solid> solidsToRemove;
    for (SolidQuantity& solid : m_solidComposition)
    {
        Quantity quantityToTake = solid.N/proportion;

        tileProportion.m_solidComposition.push_back(SolidQuantity(solid.solid, quantityToTake));
        solid.N -= quantityToTake;

        if(solid.N == 0)
        {
            solidsToRemove.push_back(solid.solid);
        }
    }

    Energy solidThermalEnergyToTake = m_solidThermalEnergy / proportion;
    tileProportion.m_solidThermalEnergy = solidThermalEnergyToTake;
    m_solidThermalEnergy -= solidThermalEnergyToTake;

    // Take liquid
    std::vector<LiquidNode*> liquidsToRemove;
    for (LiquidNode* liquid : m_liquidNodes)
    {
        Quantity quantityToTake = liquid->GetN()/proportion;
        Quantity dissolvedQuantityToTake = liquid->GetDissolvedN()/proportion;
        Energy thermalEnergyToTake = liquid->GetThermalEnergy() / proportion;

        tileProportion.AddLiquid(liquid->GetLiquid(), quantityToTake, dissolvedQuantityToTake, thermalEnergyToTake);
        liquid->TakeN(quantityToTake);
        liquid->TakeDissolvedN(quantityToTake);
        liquid->TakeThermalEnergy(thermalEnergyToTake);

        if(liquid->GetN() == 0 && liquid->GetDissolvedN() == 0)
        {
            liquidsToRemove.push_back(liquid);
        }
    }

    // Take gas
    for (Gas gas : AllGas())
    {
        Quantity quantityToTake = m_gasNode.GetN(gas) / proportion;
        tileProportion.m_gasNode.AddN(gas, quantityToTake);
    }

    Energy internalEnergyToTake = m_gasNode.GetInternalEnergy() / proportion;
    tileProportion.m_gasNode.AddInternalEnergy(internalEnergyToTake);

    // Clean
    for (Solid solidToRemove : solidsToRemove)
    {
        int index = 0;
        for (SolidQuantity& solid : m_solidComposition)
        {
            if(solid.solid == solidToRemove)
            {
                break;
            }
            index++;
        }

        m_solidComposition.erase(m_solidComposition.begin() + index);
    }

    for (LiquidNode* liquidToRemove : liquidsToRemove)
    {
        int index = 0;
        for (LiquidNode* liquid : m_liquidNodes)
        {
            if(liquid == liquidToRemove)
            {
                break;
            }
            index++;
        }

        m_liquidNodes.erase(m_liquidNodes.begin() + index);
        delete liquidToRemove;
    }


    tileProportion.UpdateVolumes();

    return tileProportion;
}







//AuroraTileContent::AuroraTileContent(Scalar volume)
//	: m_volume(volume)
//{

//}

//void AuroraTileContent::SetSolidProperties(bool isPowder)
//{
//	m_isPowder = isPowder;
//}

//void AuroraTileContent::SetMaterial(AuroraMaterial const& material)
//{
//	ClearContent();
//	AddMaterial(material, m_volume);
//}



//void AuroraTileContent::ClearContent()
//{
//	m_solidComposition.clear();
//	m_liquidComposition.clear();
//	m_GasComposition.clear();
//}

//static void ScaleComposition(std::vector<MaterialQuantity>& composition, Scalar scaleRatio)
//{
//	for(MaterialQuantity& quantity : composition)
//	{
//		quantity.quantity *= scaleRatio;
//	}
//}

//AuroraTileContent AuroraTileContent::Scaled(Scalar ratio) const
//{
//	AuroraTileContent newContent = *this;
//	newContent.Scale(ratio);
//	return newContent;
//}

//void AuroraTileContent::AddMaterial(AuroraMaterial const& material, Scalar volume)
//{
//	for(MaterialQuantity quantity : material.GetSolidComposition())
//	{
//		quantity.quantity *= volume;
//		AddInComposition(quantity, m_solidComposition);
//	}

//	for(MaterialQuantity quantity : material.GetLiquidComposition())
//	{
//		quantity.quantity *= volume;
//		AddInComposition(quantity, m_liquidComposition);
//	}

//	for(MaterialQuantity quantity : material.GetGasComposition())
//	{
//		quantity.quantity *= volume;
//		AddInComposition(quantity, m_GasComposition);
//	}
//}

//void AuroraTileContent::Scale(Scalar ratio)
//{
//	m_volume *= ratio;
//	ScaleComposition(m_solidComposition, ratio);
//	ScaleComposition(m_liquidComposition, ratio);
//	ScaleComposition(m_GasComposition, ratio);
//}

//void AuroraTileContent::RemoveMaterial(Scalar volume)
//{
//    Scalar remainingRatio = 1 - volume/m_volume;
//	Scale(remainingRatio);
//}

//void AuroraTileContent::AddSolidQuantity(MaterialQuantity solidQuantity)
//{
//	AddInComposition(solidQuantity, m_solidComposition);
//}

//void AuroraTileContent::AddLiquidQuantity(MaterialQuantity liquidQuantity)
//{
//	AddInComposition(liquidQuantity, m_liquidComposition);
//}
//void AuroraTileContent::AddGasQuantity(MaterialQuantity GasQuantity)
//{
//	AddInComposition(GasQuantity, m_GasComposition);
//}

//void AuroraTileContent::SetTemperature(Scalar temperature)
//{
//	//TODOT
//}

//bool AuroraTileContent::HasSolid() const
//{
//	// TODO Cache
//	return !m_solidComposition.empty();
//}

//MaterialComposition& AuroraTileContent::GetSolidComposition()
//{
//	return m_solidComposition;
//}
//MaterialComposition& AuroraTileContent::GetLiquidComposition()
//{
//	return m_liquidComposition;
//}

//MaterialComposition& AuroraTileContent::GetGasComposition()
//{
//	return m_GasComposition;
//}
*/
SolidQuantity::SolidQuantity(Solid iSolid, Quantity iN)
    : solid(iSolid)
    , N(iN)
{

}



}
