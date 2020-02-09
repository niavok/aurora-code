#include "aurora_world_renderer.h"
#include "scene/2d/sprite.h"


#include "../world/aurora_world.h"
#include "../world/aurora_tile.h"
#include "../world/aurora_level.h"

#include "../physics/aurora_physic_gas_gas_transition.h"

#include <string>
#include <assert.h>

namespace aurora {

static Scalar MmToGodot = 1.; // Mn in micrometer, Godot in Mm
static Scalar MeterToGodot = 1.e3; // Mn in micrometer, Godot in Mm

AuroraWorldRenderer::AuroraWorldRenderer()
	: m_targetWorld(nullptr)
{
    m_control = memnew(Control);
    m_debugFont = m_control->get_font("mono_font", "Fonts");
}


AuroraWorldRenderer::~AuroraWorldRenderer()
{
    memdelete(m_control);

	if (m_testTexture1.is_valid())
		m_testTexture1->remove_change_receptor(this);

	if (m_testTexture2.is_valid())
		m_testTexture2->remove_change_receptor(this);
}

//if (!target_node_override && !target_node_path_override.is_empty())
//	target_node_override = Object::cast_to<Spatial>(get_node(target_node_path_override));

void AuroraWorldRenderer::set_world_node(const NodePath &p_node)
{

	printf("set_world_node p_node=%ls\n", String(p_node).c_str());


	m_targetWorldPath = p_node;
	m_targetWorld = nullptr;
}

NodePath AuroraWorldRenderer::get_world_node()
{
	return m_targetWorldPath;
}


void AuroraWorldRenderer::_bind_methods() {

	ClassDB::bind_method(D_METHOD("set_world_node", "node"), &AuroraWorldRenderer::set_world_node);
	ClassDB::bind_method(D_METHOD("get_world_node"), &AuroraWorldRenderer::get_world_node);

	ClassDB::bind_method(D_METHOD("set_texture1", "texture"), &AuroraWorldRenderer::set_texture1);
	ClassDB::bind_method(D_METHOD("get_texture1"), &AuroraWorldRenderer::get_texture1);
	ClassDB::bind_method(D_METHOD("set_texture2", "texture"), &AuroraWorldRenderer::set_texture2);
	ClassDB::bind_method(D_METHOD("get_texture2"), &AuroraWorldRenderer::get_texture2);



	ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "world_node", PROPERTY_HINT_NODE_PATH_VALID_TYPES, "AuroraWorld"), "set_world_node", "get_world_node");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "texture1", PROPERTY_HINT_RESOURCE_TYPE, "Texture"), "set_texture1", "get_texture1");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "texture2", PROPERTY_HINT_RESOURCE_TYPE, "Texture"), "set_texture2", "get_texture2");

}

void AuroraWorldRenderer::_notification(int p_what) {

	switch (p_what) {

		case NOTIFICATION_INTERNAL_PROCESS: {
            //printf("NOTIFICATION_INTERNAL_PROCESS\n");
            update();
            //_change_notify("frame");
            //emit_signal(SceneStringNames::get_singleton()->frame_changed);
		} break;
		case NOTIFICATION_TRANSFORM_CHANGED: {
		} break;
		case NOTIFICATION_ENTER_TREE: {
            set_process_internal(true);
		} break;
		case NOTIFICATION_DRAW: {
            //printf("NOTIFICATION_DRAW\n");
			if (m_testTexture1.is_null())
				return;
			if (m_testTexture2.is_null())
				return;

			if (has_node(m_targetWorldPath)) {
				m_targetWorld = Object::cast_to<AuroraWorld>(get_node(m_targetWorldPath));
			}
			else
			{
				printf("set_world_node fail to find node %ls\n", String(m_targetWorldPath).c_str());
				return;
			}



			if(m_targetWorld != nullptr)
			{
				RID ci = get_canvas_item();
				DrawWorld(ci);
			}


			/*Rect2 src_rect, dst_rect;
			bool filter_clip;
			_get_rects(src_rect, dst_rect, filter_clip);
			m_testTexture->draw_rect_region(ci, dst_rect, src_rect, Color(1, 1, 1), false, normal_map, filter_clip);*/
		} break;
	}
}


void AuroraWorldRenderer::set_texture1(const Ref<Texture> &p_texture) {

	if (p_texture == m_testTexture1)
		return;

	if (m_testTexture1.is_valid())
		m_testTexture1->remove_change_receptor(this);

	m_testTexture1 = p_texture;

	if (m_testTexture1.is_valid())
		m_testTexture1->add_change_receptor(this);

	update();
	emit_signal("texture_changed");
	item_rect_changed();
	_change_notify("texture");
}

Ref<Texture> AuroraWorldRenderer::get_texture1() const {

	return m_testTexture1;
}

void AuroraWorldRenderer::set_texture2(const Ref<Texture> &p_texture) {

	if (p_texture == m_testTexture2)
		return;

	if (m_testTexture2.is_valid())
		m_testTexture2->remove_change_receptor(this);

	m_testTexture2 = p_texture;

	if (m_testTexture2.is_valid())
		m_testTexture2->add_change_receptor(this);

	update();
	emit_signal("texture_changed");
	item_rect_changed();
	_change_notify("texture");
}

Ref<Texture> AuroraWorldRenderer::get_texture2() const {

	return m_testTexture2;
}

static int drawTileCount;

void AuroraWorldRenderer::DrawWorld(RID& ci)
{
    for(Level* level : m_targetWorld->GetLevels())
    {
        DrawLevel(ci, level);
    }

}

void AuroraWorldRenderer::DrawLevel(RID& ci, Level const* level)
{
	drawTileCount = 0;

    for(Tile const* tile : level->GetRootTiles())
	{
		DrawTile(ci, tile);
	}

	for(Tile const* tile : level->GetRootTiles())
	{
		DrawTileOverlay(ci, tile);
	}

	//printf("tilecount=%d\n", drawTileCount);
}


void AuroraWorldRenderer::DrawTileOverlay(RID& ci, Tile const* tile)
{
	if(tile->IsComposite())
	{
        for(Tile const* child: tile->GetChildren())
		{
			DrawTileOverlay(ci, child);
		}
	}
	else
	{
		GasNode const& gas = tile->GetContent()->GetGazNode();
		Vector2 tilePosition = tile->GetPositionMm().ToVector2() * MmToGodot;

		// Draw flow
		for(TransitionLink const & transitionLink : gas.GetTransitionLinks())
		{
			if(transitionLink.index > 0)
			{
				// Display on master
				//continue;
			}

			GasGasTransition const* transition = reinterpret_cast<GasGasTransition*>(transitionLink.transition);

			Transition::NodeLink const* link = transition->GetNodeLink(transitionLink.index);
			if(link->outputKineticEnergy == 0 && link->inputKineticEnergy == 0)
			{
				continue;
			}

			//Meter altitudeRelativeToNode;
			//Meter longitudeRelativeToNode;
			Vector2 relativeBase(link->longitudeRelativeToNode, link->altitudeRelativeToNode);

			Transition::Direction direction = transition->GetDirection(transitionLink.index);

			Vector2 offsetDirection(1,1);
			Vector2 offsetPosition(0,0);
			Color transitionColor(0, 0, 0);

			real_t margin_offset = tile->GetSizeMm() * MmToGodot / 6;

			switch (direction)
			{
			case Transition::Direction::DIRECTION_DOWN:
				offsetPosition = Vector2(-margin_offset, 0);
				offsetDirection = Vector2(0, -1);
				transitionColor = Color(1, 0, 0);
				break;
			case Transition::Direction::DIRECTION_UP:
				offsetPosition = Vector2(margin_offset, 0);
				offsetDirection = Vector2(0, 1);
				transitionColor = Color(0, 1, 0);
				break;
			case Transition::Direction::DIRECTION_LEFT:
				offsetPosition = Vector2(0, -margin_offset);
				offsetDirection = Vector2(1, 0);
				transitionColor = Color(0, 0, 1);
				break;
			case Transition::Direction::DIRECTION_RIGHT:
				offsetPosition = Vector2(0, margin_offset);
				offsetDirection = Vector2(-1, 0);
				transitionColor = Color(1, 1, 0);
				break;
			default:
				assert(false);
				break;
			}

			real_t length = (link->outputKineticEnergy + link->inputKineticEnergy) * 10.0;
			real_t width = 1.f;
			real_t maxLength = tile->GetSizeMm() * MmToGodot * 0.9;
			if(length > maxLength)
			{
				width = length / maxLength;
				length = maxLength;
			}

			//real_t length = sqrt(link->outputKineticEnergy) * 1;

			draw_line(tilePosition + relativeBase * MeterToGodot + offsetPosition, tilePosition + relativeBase * MeterToGodot + offsetPosition + offsetDirection * length, transitionColor, width);
		}
	}
}

void AuroraWorldRenderer::DrawTile(RID& ci, Tile const* tile)
{
	drawTileCount++;

	if(tile->IsComposite())
	{
        for(Tile const* child: tile->GetChildren())
		{
			DrawTile(ci, child);
		}
	}
	else
	{
        //printf("DrawTile at %ls size %f\n", String(tile->GetPosition()).c_str(), tile->GetSize());
        //m_testTexture->draw(ci,tile->GetPosition());

		Texture* texture;


		if(tile->GetContent()->HasSolid())
		{
			texture = *m_testTexture1;
		}
		else
		{
			texture = *m_testTexture2;
		}

        Vector2 pos = tile->GetPositionMm().ToVector2() * MmToGodot;
        real_t size = tile->GetSizeMm() * MmToGodot;

        //texture->draw_rect(ci, Rect2(pos, Vector2(size, size)));



        if(true || (pos.x < 1000 && pos.y < 1000))
        {
            Color color(0.9f,0.9f,0.9f);
            GasNode const& gas = tile->GetContent()->GetGazNode();

            Scalar tilePressure = gas.GetPressure();
            Scalar temperature = gas.GetTemperature();

            //gas.ComputeNPT(N, pressure, temperature);
            Scalar bottomPressure = tilePressure + gas.GetPressureGradient() * MmToMeter(tile->GetSizeMm());

            auto PressureToColor = [](Scalar pressure)
            {
                //Scalar const minPressure = 16000;
                //Scalar const maxPressure = 300000;
				Scalar const minPressure = 50000;
                Scalar const maxPressure = 300000;

                return float((pressure-minPressure) / (maxPressure - minPressure));
				//return 0;
            };

            float temperatureColor = float(temperature / 300.);
            float temperatureColor2 = float(temperature / 600.);

			float pressureColor = PressureToColor(tilePressure);
            float bottomPressureColor = PressureToColor(bottomPressure);



            Vector<Vector2> points;
            points.push_back(pos);
            points.push_back(pos + Vector2(size, 0));
            points.push_back(pos + Vector2(size, size));
            points.push_back(pos + Vector2(0, size));



            Color topColor(temperatureColor, temperatureColor2, pressureColor);
            Color bottomColor(temperatureColor, temperatureColor2, bottomPressureColor);

            Vector<Color> colors;


            colors.push_back(topColor);
            colors.push_back(topColor);
            colors.push_back(bottomColor);
            colors.push_back(bottomColor);
            //colors.push_back(Color(0, 0, 1, 0.5));


            draw_polygon(points, colors);

            //m_debugFont->draw(ci, pos + Vector2(10, 20), rtos(tilePressure * 1e-5), color);
            //m_debugFont->draw(ci, pos + Vector2(10, 40), rtos(temperature), color);
			//draw_line(pos, pos + Vector2(0, size), Color(0.5,0.5,0.5));
			//draw_line(pos, pos + Vector2(size, 0), Color(0.5,0.5,0.5));

			/*draw_line(pos, pos + Vector2(0, size-1), Color(0.5,0.5,0.5));
			draw_line(pos, pos + Vector2(size-1, 0), Color(0.5,0.5,0.5));
			draw_line(pos+ Vector2(size-1, size-1), pos + Vector2(0, size-1), Color(0.5,0.5,0.5));
			draw_line(pos+ Vector2(size-1, size-1), pos + Vector2(size-1, 0), Color(0.5,0.5,0.5));*/
		}
		//virtual void draw_rect(RID p_canvas_item, const Rect2 &p_rect, bool p_tile = false, const Color &p_modulate = Color(1, 1, 1), bool p_transpose = false, const Ref<Texture> &p_normal_map = Ref<Texture>()) const;


		//bool filter_clip;
		//_get_rects(src_rect, dst_rect, filter_clip);
		//m_testTexture->draw_rect_region(ci, dst_rect, src_rect, Color(1, 1, 1), false, normal_map, filter_clip);*/
	}
}

Rect2 AuroraWorldRenderer::_edit_get_rect() const
{
    if (m_testTexture1.is_valid() && m_testTexture2.is_valid() && m_targetWorld != nullptr && m_targetWorld->GetLevels().size() > 0)
	{
        Level* firstLevel = m_targetWorld->GetLevels()[0];
        return Rect2(Vector2(0, 0) , firstLevel->GetSizeMm().ToVector2() * MmToGodot);
	}
	return Rect2(0, 0, 0, 0);
}

bool AuroraWorldRenderer::_edit_use_rect() const
{
	if (m_testTexture1.is_valid() && m_testTexture2.is_valid() && m_targetWorld != nullptr)
		return true;

	return false;
}

}
