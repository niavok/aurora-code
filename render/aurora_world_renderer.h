#ifndef AURORA_WORLD_RENDERER_H
#define AURORA_WORLD_RENDERER_H

#include "scene/2d/node_2d.h"
#include "scene/gui/control.h"

#include "../game/aurora_game_manager.h"

namespace aurora {

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

	void set_game_manager_node(const NodePath &p_node);
	NodePath get_game_manager_node();

	void set_texture1(const Ref<Texture> &p_texture);
	Ref<Texture> get_texture1() const;

	void set_texture2(const Ref<Texture> &p_texture);
	Ref<Texture> get_texture2() const;

	int64_t get_last_render_duration();

private:

	void DrawWorld(RID& ci);
    void DrawLevel(RID& ci, Ref<AuroraLevel> const& level);
    void DrawTile(RID& ci, Tile const* tile);
	void DrawTileOverlay(RID& ci, Tile const* tile);

	NodePath m_targetGameManagerPath;
	AuroraGameManager* m_gameManager;
	Ref<AuroraWorld> m_renderWorld;
	Ref<Texture> m_testTexture1;
	Ref<Texture> m_testTexture2;
    Control *m_control;
    Ref<Font> m_debugFont;
	int64_t m_lastRenderDuration;
};

}

#endif
