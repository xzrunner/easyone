#pragma once

#include <ee0/Config.h>
#include <ee0/typedef.h>

ECS_WORLD_DECL

class wxWindow;

namespace pt0 { class Camera; }
namespace ee0 { class WxStagePage; class RenderContext; class WindowContext; }

namespace eone
{

class WxStagePanel;
class WxStagePage;

class PanelFactory
{
public:
	static WxStagePage* CreateStagePage(ECS_WORLD_PARAM int page_type, WxStagePanel* stage_panel);

	static void CreatePreviewOP(
#ifdef GAME_OBJ_ECS
		e0::World& world
#endif // GAME_OBJ_ECS
	);

    static ee0::EditOPPtr CreateNode2DSelectOP(const std::shared_ptr<pt0::Camera>& camera, ee0::WxStagePage& stage,
        const ee0::RenderContext& rc, const ee0::WindowContext& wc);

}; // PanelFactory

}