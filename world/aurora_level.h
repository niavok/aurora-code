#ifndef AURORA_LEVEL_H
#define AURORA_LEVEL_H


#include "aurora_tile.h"
#include <vector>

namespace aurora {

class Level {

public:
    //static scalar const MinTileSize;
    static int const TileChildEdgeCount;
    static int const TileChildCount;

    // TODO test
    //void add(int value);
    //void reset();
    //int get_total() const;

    Level(bool horizontalLoop, Mm minTileSize, int maxTileSubdivision, int rootTileHCount, int rootTileVCount);

    //void PaintTile(Rect2 area, AuroraMaterial const& material);

    //void Repack();

    std::vector<Tile*>& GetRootTiles() { return m_rootTiles; }
    std::vector<Tile*> const& GetRootTiles() const { return m_rootTiles; }

    Mm2 GetSizeMm() const { return m_size; };
    Mm GetMinTileSizeMm() const { return m_minTileSize; }

    void FindTileAt(std::vector<Tile*>& matchs, MmRect area);


    //Rect2 GetArea() const;

    bool IsHorizontalLoop() const { return m_horizontalLoop; }

private:
    std::vector<Tile*> m_rootTiles;
    Mm m_minTileSize;
    Mm m_maxTileSize;
    Mm2 m_size;
    bool m_horizontalLoop;
};

}

#endif
