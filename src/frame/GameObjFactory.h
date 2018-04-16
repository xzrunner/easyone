#pragma once

#include <ee0/GameObj.h>

#include "frame/GameObjType.h"

ECS_WORLD_DECL

namespace eone
{

class GameObjFactory
{
public:
	static ee0::GameObj Create(ECS_WORLD_PARAM GameObjType type);

}; // GameObjFactory

}