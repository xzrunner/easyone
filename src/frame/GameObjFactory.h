#pragma once

#include <ee0/GameObj.h>

#include "frame/GameObjType.h"

#ifdef GAME_OBJ_ECS
namespace ecsx { class World; }
#endif // GAME_OBJ_ECS

namespace eone
{

class GameObjFactory
{
public:
	static ee0::GameObj Create(
#ifdef GAME_OBJ_ECS
		ecsx::World& world, 
#endif // GAME_OBJ_ECS
		GameObjType type
	);

}; // GameObjFactory

}