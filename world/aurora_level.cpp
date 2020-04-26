#include<stdio.h>
#include "aurora_level.h"


namespace aurora {


void AuroraLevel::_bind_methods() {

	ClassDB::bind_method(D_METHOD("get_name"), &AuroraLevel::GetName);
}

AuroraLevel::AuroraLevel()
	: AuroraLevel(Meter2(1,1), 1.0, 1.0, 1, 1, false)
{

}

AuroraLevel::AuroraLevel(Meter2 levelBottomLeftPosition, Meter tileSize, Meter levelDepth, int tileHCount, int tileVCount, bool horizontalLoop)
    : m_tileSize(tileSize)
    , m_levelDepth(levelDepth)
    , m_tileHCount(tileHCount)
    , m_tileVCount(tileVCount)
    , m_levelSize(Meter2(tileHCount, tileVCount) * tileSize)
    , m_levelBottomLeftPosition(levelBottomLeftPosition)
    , m_horizontalLoop(horizontalLoop)

{
    size_t worldRootTileCount = size_t(m_tileHCount *  m_tileVCount);
    m_tiles.reserve(worldRootTileCount);

    for(int y = 0; y < m_tileVCount; y++)
    {
        for(int x = 0; x < m_tileHCount; x++)
        {
            Tile* tile = new Tile(m_tileSize, levelBottomLeftPosition + Meter2(x * m_tileSize, y * m_tileSize), m_levelDepth);
            m_tiles.emplace_back(tile);
        }
    }

    printf("new Level: %lu tiles created (%f x %f m)\n", m_tiles.size(), m_levelSize.x, m_levelSize.y);
}

void AuroraLevel::ForEachTransition(std::function<void(Tile* tileA, Tile* tileB, Transition::Direction direction)> callback)
{
    // Vertical transition, skip top line
    for(int y = 0; y < m_tileVCount - 1; y++)
    {
        for(int x = 0; x < m_tileHCount; x++)
        {
            callback(GetTileAtIndex(x, y), GetTileAtIndex(x, y + 1), Transition::Direction::DIRECTION_UP);
        }
    }

    // Horizontal transition, skip right line
    for(int y = 0; y < m_tileVCount; y++)
    {
        for(int x = 0; x < m_tileHCount -1; x++)
        {
            callback(GetTileAtIndex(x, y), GetTileAtIndex(x + 1, y), Transition::Direction::DIRECTION_RIGHT);
        }
    }

    if(m_horizontalLoop && m_tileHCount > 1)
    {
        // Horizontal loop transition, last right line only
        for(int y = 0; y < m_tileVCount; y++)
        {
            callback(GetTileAtIndex(m_tileHCount - 1, y), GetTileAtIndex(0, y), Transition::Direction::DIRECTION_RIGHT);
        }
    }
}

String const& AuroraLevel::GetName()
{
	return m_name;
}

Tile* AuroraLevel::GetTileAtIndex(int x, int y)
{
    size_t index = x + y * m_tileHCount;
    return m_tiles[index];
}

/*
void Level::FindTileAt(std::vector<Tile*>& matchs, MmRect area)
{
    for(Tile* tile : m_rootTiles)
    {
        tile->FindTileAt(matchs, area);
    }
}*/

}

