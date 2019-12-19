#ifndef AURORA_TILE_H
#define AURORA_TILE_H

#include "core/math/vector2.h"
#include "core/math/rect2.h"
#include "../tools/aurora_types.h"
#include "../physics/aurora_physic_engine.h"
#include "../physics/aurora_physic_gas_node.h"
#include "../physics/aurora_physic_liquid_node.h"
#include <vector>


namespace aurora {
class Level;

/*




struct MaterialQuantity
{
	MaterialQuantity(Material pMaterial, scalar pQuantity, scalar pQuality);

	Material material;
	scalar quantity;
	scalar quality;
};
*/

struct SolidQuantity
{
    SolidQuantity(Solid iSolid, Quantity iN);

    Solid solid;
    Quantity N;
};

/*
typedef std::vector<MaterialQuantity> MaterialComposition;

bool operator ==(const MaterialComposition &a, const MaterialComposition &b);
*/

/**
 * @brief The content of 1m2
 */
//class AuroraMaterial
//{

//public:

//	void AddSolidMaterial(MaterialQuantity solidQuantity);
//	void AddLiquidMaterial(MaterialQuantity liquidQuantity);
//	void AddGasMaterial(MaterialQuantity GasQuantity);

//	void SetTemperature(scalar temperature);

//	std::vector<MaterialQuantity> const& GetSolidComposition() const;
//	std::vector<MaterialQuantity> const& GetLiquidComposition() const;
//	std::vector<MaterialQuantity> const& GetGasComposition() const;

//private:
//	bool m_isPowder;
//	std::vector<MaterialQuantity> m_solidComposition;
//	std::vector<MaterialQuantity> m_liquidComposition;
//	std::vector<MaterialQuantity> m_GasComposition;

//	scalar m_solidHeat;
//	scalar m_liquidHeat;
//	scalar m_GasHeat;
//};


class TileContent {
public:
    TileContent(Volume volume);

    ~TileContent();

//	void SetSolidProperties(bool isPowder);

//    /**
//     * @brief Fill the tile with a material
//     * @param material
//     */
//    void SetMaterial(AuroraMaterial const& material);

    void SetContent(TileContent const& content);

    void AddSolid(Solid solid, Quantity N, Energy thermalEnergy);

    void AddLiquid(Liquid liquid, Quantity N, Quantity dissolvedN, Energy thermalEnergy);


    void AddGas(Gas gas, Quantity N, Energy thermalEnergy);


//    void ClearContent();

//    AuroraTileContent Scaled(scalar ratio) const;
//    void Scale(scalar ratio);

//    void AddMaterial(AuroraMaterial const& material, scalar volume);
//    void RemoveMaterial(scalar volume);

//    void AddSolidQuantity(MaterialQuantity solidQuantity);
//    void AddLiquidQuantity(MaterialQuantity liquidQuantity);
//    void AddGasQuantity(MaterialQuantity GasQuantity);

//    void AddGasComposition(MaterialComposition const& composition);
//    void AddLiquidComposition(MaterialComposition const& composition);
//    void AddSolidComposition(MaterialComposition const& composition);

//    void SetTemperature(scalar temperature);

    bool HasSolid() const;

    bool HasLiquid() const;

    bool HasGas() const;

    Volume GetTotalVolume() const;

    Volume GetGasVolume() const;

    Volume GetLiquidsVolume() const;

    Volume GetSolidsVolume() const;

//    MaterialComposition& GetSolidComposition();
//    MaterialComposition& GetLiquidComposition();
//    MaterialComposition& GetGasComposition();

    bool IsEmpty() const;

    TileContent TakeProportion(int proportion);

    GasNode& GetGazNode() { return m_gasNode; }
    GasNode const& GetGazNode() const { return m_gasNode; }

private:

    TileContent(TileContent& content); // Not recommanded


    Volume m_volume;
    Volume m_solidVolume;

    std::vector<SolidQuantity> m_solidComposition;
    Energy m_solidThermalEnergy;

    std::vector<LiquidNode*> m_liquidNodes;

    GasNode m_gasNode;

    void UpdateVolumes();

//    scalar m_isPowder;

//	MaterialComposition m_solidComposition;
//	MaterialComposition m_liquidComposition;
//	MaterialComposition m_GasComposition;

//	scalar solidHeat;
//	scalar liquidHeat;
//	scalar GasHeat;
};

class Tile {
public:
    Tile(Mm size, Mm2 position);
    ~Tile();

	enum InsideMode	{
		Yes,
		No,
		Partially,
	};

    InsideMode IsInside(MmRect area);

    //void PaintTile(Rect2 area, AuroraMaterial const& material);

    Volume GetVolume() const;

//	/**
//	 * @brief Make the tile composite if possible,
//	 * @return return true if the tile is already composite or
//	 * has been made composite
//	 */
//	bool SplitTile();

    /**
     * @brief IsComposite
     * @return Return true if the tile has children
     */
    bool IsComposite() const;

//	/**
//	 * @brief Erase all the content with the given material
//	 * @param material used as material so it will be scaled
//	 */
//	void PaintContent(AuroraMaterial const& material);

//	/**
//	 * @brief Merge composite tiles with same composition
//	 */
//	void Repack();

	/**
	 * @brief Erase all the content and replace with the given content
	 * @param content used as quantity so it won't be scaled
	 */
    void SetContent(TileContent const& content);

    Mm2 GetPosition() const { return m_position; }

    Mm GetSize() const { return m_size; }

    std::vector<Tile*> const& GetChildren() const { return m_children; }

    std::vector<Tile*>& GetChildren() { return m_children; }

    TileContent* GetContent() { return m_content; }
    TileContent const* GetContent() const { return m_content; }

    bool Split(Level* level);

    void FindTileAt(std::vector<Tile*>& matchs, MmRect area);

private:




    Mm m_size;
    Mm2 m_position; // Optionnal ?
    std::vector<Tile*> m_children;
    TileContent* m_content;

	// Cache
    MmRect m_worldArea;
};

}

#endif
