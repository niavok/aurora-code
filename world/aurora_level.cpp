#include<stdio.h>
#include "aurora_level.h"


namespace aurora {

int const Level::TileChildEdgeCount = 2; // 2 or modify 1<<maxTileSubdivision
int const Level::TileChildCount = Level::TileChildEdgeCount * Level::TileChildEdgeCount;


Level::Level(Mm minTileSize, int maxTileSubdivision, int rootTileHCount, int rootTileVCount)
    : m_minTileSize(minTileSize)
    , m_maxTileSize(minTileSize * (1<<maxTileSubdivision))
    , m_size(Mm2(rootTileHCount, rootTileVCount) * m_maxTileSize)

{
    size_t worldRootTileCount = size_t(rootTileHCount *  rootTileVCount);
    m_rootTiles.reserve(worldRootTileCount);

    for(int x = 0; x < rootTileHCount; x++)
    {
        for(int y = 0; y < rootTileVCount; y++)
        {
            Tile* tile = new Tile(m_maxTileSize, Mm2(x * m_maxTileSize, y * m_maxTileSize));
            m_rootTiles.emplace_back(tile);
        }
    }

    printf("new Level: %lu tiles created\n", m_rootTiles.size());
}


void Level::FindTileAt(std::vector<Tile*>& matchs, MmRect area)
{
    for(Tile* tile : m_rootTiles)
    {
        tile->FindTileAt(matchs, area);
    }
}



//scalar const AuroraWorld::MinTileSize = 0.0625;

//scalar const AuroraWorld::TileChildCount = AuroraWorld::TileChildEdgeCount * AuroraWorld::TileChildEdgeCount;

//AuroraWorld::~AuroraWorld()
//{
//	for(AuroraTile* tile : m_rootTiles)
//	{
//		delete tile;
//	}
//}

//void AuroraWorld::add(int value) {

//    count += value;
//}

//void AuroraWorld::reset() {

//    count = 0;
//}

//int AuroraWorld::get_total() const {

//    return count;
//}

//void AuroraWorld::_bind_methods() {

//	ClassDB::bind_method(D_METHOD("add", "value"), &AuroraWorld::add);
//	ClassDB::bind_method(D_METHOD("reset"), &AuroraWorld::reset);
//	ClassDB::bind_method(D_METHOD("get_total"), &AuroraWorld::get_total);
//}

//AuroraWorld::AuroraWorld() {
//    count = 0;
//	printf("plop1\n");
//    WorldEditor worldEditor(*this);
//    worldEditor.GenerateTestWord();
//}

//void AuroraWorld::Generate()
//{

//}

//void AuroraWorld::PaintTile(Rect2 area, AuroraMaterial const& material)
//{
//	for(AuroraTile* tile : m_rootTiles)
//	{
//		tile->PaintTile(area, material);
//	}
//}

//void AuroraWorld::Repack()
//{
//	for(AuroraTile* tile : m_rootTiles)
//	{
//		tile->Repack();
//	}
//}

//Rect2 AuroraWorld::GetWorldArea() const
//{
//	return m_worldArea;
//}

}

