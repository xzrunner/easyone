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
#ifndef GAME_OBJ_ECS
	static ee0::GameObj Create(GameObjType type);
#else
	static ee0::GameObj Create(ecsx::World& world, GameObjType type);
#endif // GAME_OBJ_ECS

}; // GameObjFactory

}