#ifndef AURORA_WORLD_RENDERER_H
#define AURORA_WORLD_RENDERER_H

#include "scene/2d/node_2d.h"
#include "scene/gui/control.h"


namespace aurora {

class AuroraWorld;
class Tile;
class Level;

class AuroraWorldRenderer : public Node2D {
     GDCLASS(AuroraWorldRenderer, Node2D)

protected:
    static void _bind_methods();
	void _notification(int p_what);

public:

	AuroraWorldRenderer();
	virtual ~AuroraWorldRenderer();

	virtual Rect2 _edit_get_rect() const;
	virtual bool _edit_use_rect() const;

	void set_world_node(const NodePath &p_node);
	NodePath get_world_node();

	void set_texture1(const Ref<Texture> &p_texture);
	Ref<Texture> get_texture1() const;

	void set_texture2(const Ref<Texture> &p_texture);
	Ref<Texture> get_texture2() const;

private:

	void DrawWorld(RID& ci);
    void DrawLevel(RID& ci, Level const* level);
    void DrawTile(RID& ci, Tile const* tile);
	void DrawTileOverlay(RID& ci, Tile const* tile);

	NodePath m_targetWorldPath;
	AuroraWorld const* m_targetWorld;
	Ref<Texture> m_testTexture1;
	Ref<Texture> m_testTexture2;
    Control *m_control;
    Ref<Font> m_debugFont;
};

}

#endif
