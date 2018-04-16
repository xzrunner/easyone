#pragma once

#include <ee0/Config.h>

ECS_WORLD_DECL

namespace eone
{

class WxStagePanel;
class WxStagePage;

class StagePageFactory
{
public:
	static WxStagePage* Create(ECS_WORLD_PARAM int page_type, WxStagePanel* stage_panel);

	static void CreatePreviewOP(
#ifdef GAME_OBJ_ECS
		ecsx::World& world
#endif // GAME_OBJ_ECS
	);

}; // StagePageFactory

}