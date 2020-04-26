#ifndef AURORA_LEVEL_H
#define AURORA_LEVEL_H


#include "core/reference.h"


#include "aurora_tile.h"
#include <vector>
#include <functional>

namespace aurora {

class AuroraLevel : public Reference {
    GDCLASS(AuroraLevel, Reference)

protected:
    static void _bind_methods();

public:
	String const& GetName();

    //static scalar const MinTileSize;
    static int const TileChildEdgeCount;
    static int const TileChildCount;

    // TODO test
    //void add(int value);
    //void reset();
    //int get_total() const;

	AuroraLevel();
    AuroraLevel(Meter2 levelBottomLeftPosition, Meter tileSize, Meter levelDepth, int tileHCount, int tileVCount, bool horizontalLoop);

    //void PaintTile(Rect2 area, AuroraMaterial const& material);

    //void Repack();

    std::vector<Tile*>& GetTiles() { return m_tiles; }
    std::vector<Tile*> const& GetTiles() const { return m_tiles; }

    Meter2 GetLevelSize() const { return m_levelSize; };
    Meter GetTileSize() const { return m_tileSize; }
    Meter2 GetLevelBottomLeftPosition() const { return m_levelBottomLeftPosition; }

    //void FindTileAt(std::vector<Tile*>& matchs, MmRect area);


    //Rect2 GetArea() const;

    bool IsHorizontalLoop() const { return m_horizontalLoop; }

    void ForEachTransition(std::function<void(Tile* tileA, Tile* tileB, Transition::Direction direction)> callback);

    Tile* GetTileAtIndex(int x, int y);

private:
	String m_name;
    std::vector<Tile*> m_tiles;
    Meter m_tileSize;
    Meter m_levelDepth;
    int m_tileHCount;
    int m_tileVCount;
    Meter2 m_levelSize;
    Meter2 m_levelBottomLeftPosition;

    bool m_horizontalLoop;
};

}

#endif
