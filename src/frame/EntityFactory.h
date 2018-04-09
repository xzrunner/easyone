#pragma once

#include "frame/NodeType.h"

#include <ecsx/Entity.h>

namespace eone
{

class EntityFactory
{
public:
	static ecsx::Entity Create(ecsx::World& world, NodeType type);

}; // EntityFactory

}