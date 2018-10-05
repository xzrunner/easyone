#pragma once

#include <ee0/Config.h>

ECS_WORLD_DECL

class wxWindow;

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

}; // PanelFactory

}