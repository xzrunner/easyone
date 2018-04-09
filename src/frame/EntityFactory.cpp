#include "frame/EntityFactory.h"

#include <guard/check.h>
#include <SM_Rect.h>
#include <entity2/CompImage.h>
#include <entity2/CompBoundingBox.h>

namespace eone
{

ecsx::Entity EntityFactory::Create(ecsx::World& world, NodeType type)
{
	GD_ASSERT(type != NODE_UNKNOWN, "err type");

	auto entity = world.CreateEntity();
	sm::rect sz;

	switch (type)
	{
		case NODE_IMAGE:
		{
			auto& cimage = entity.AddComponent<e2::CompImage>();
			sz.Build(100, 100);
		}
		break;
	}

	// aabb
	auto& cbb = entity.AddComponent<e2::CompBoundingBox>();
	cbb.aabb.Build(sz);

	return entity;
}

}