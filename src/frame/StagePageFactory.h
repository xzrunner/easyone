#pragma once

#include <ee0/Config.h>

#ifdef GAME_OBJ_ECS
namespace ecsx { class World; }
#endif // GAME_OBJ_ECS

namespace eone
{

class WxStagePanel;
class WxStagePage;

class StagePageFactory
{
public:
	static WxStagePage* Create(
#ifdef GAME_OBJ_ECS
		ecsx::World& world,
#endif // GAME_OBJ_ECS
		int page_type, 
		WxStagePanel* stage_panel
	);

	static void CreatePreviewOP(
#ifdef GAME_OBJ_ECS
		ecsx::World& world
#endif // GAME_OBJ_ECS
	);

}; // StagePageFactory

}