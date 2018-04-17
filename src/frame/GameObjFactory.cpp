#include "frame/GameObjFactory.h"

#include "particle3d/config.h"

#ifndef GAME_OBJ_ECS
#include <ee0/CompNodeEditor.h>
#else
#include <ee0/CompEntityEditor.h>
#endif // GAME_OBJ_ECS

#ifndef GAME_OBJ_ECS
#include <node0/SceneNode.h>
#include <node2/CompComplex.h>
#include <node2/CompImage.h>
#include <node2/CompText.h>
#include <node2/CompMask.h>
#include <node2/CompMesh.h>
#include <node2/CompScale9.h>
#include <node2/CompAnim.h>
#include <node2/CompAnimInst.h>
#include <node2/CompParticle3d.h>
#include <node2/CompParticle3dInst.h>
#include <node2/CompBoundingBox.h>
#include <node2/CompTransform.h>
#else
#include <ecsx/World.h>
#include <entity2/CompImage.h>
#include <entity2/CompText.h>
#include <entity2/CompMask.h>
#include <entity2/CompMesh.h>
#include <entity2/CompScale9.h>
#include <entity2/CompComplex.h>
#include <entity2/CompBoundingBox.h>
#include <entity2/CompTransform.h>
#endif // GAME_OBJ_ECS

#include <anim/KeyFrame.h>
#include <anim/Layer.h>
#include <emitter/P3dTemplate.h>

namespace eone
{

ee0::GameObj GameObjFactory::Create(ECS_WORLD_PARAM GameObjType type)
{
	if (type == GAME_OBJ_UNKNOWN) {
		return GAME_OBJ_NULL;
	}

#ifndef GAME_OBJ_ECS
	ee0::GameObj obj = std::make_shared<n0::SceneNode>();
#else
	auto obj = world.CreateEntity();
#endif // GAME_OBJ_ECS

	sm::rect sz;

	switch (type)
	{
	case GAME_OBJ_IMAGE:
		{
#ifndef GAME_OBJ_ECS
			obj->AddSharedComp<n2::CompImage>();
#else
			world.AddComponent<e2::CompImage>(obj);
#endif // GAME_OBJ_ECS
			sz.Build(100, 100);
		}
		break;
	case GAME_OBJ_TEXT:
		{
#ifndef GAME_OBJ_ECS
			auto& ctext = obj->AddSharedComp<n2::CompText>();
			auto& tb = ctext.GetText().tb;
#else
			auto& ctext = world.AddComponent<e2::CompText>(obj);
			auto& tb = ctext.text.tb;
#endif // GAME_OBJ_ECS
			sz.Build(static_cast<float>(tb.width), static_cast<float>(tb.height));
		}
		break;
	case GAME_OBJ_MASK:
		{
#ifndef GAME_OBJ_ECS
			obj->AddSharedComp<n2::CompMask>();
#else
			world.AddComponent<e2::CompMask>(obj);
#endif // GAME_OBJ_ECS
			sz.Build(100, 100);
		}
		break;
	case GAME_OBJ_MESH:
		{
#ifndef GAME_OBJ_ECS
			obj->AddSharedComp<n2::CompMesh>();
#else
			world.AddComponent<e2::CompMesh>(obj);
#endif // GAME_OBJ_ECS
			sz.Build(100, 100);
		}
		break;
	case GAME_OBJ_SCALE9:
		{
#ifndef GAME_OBJ_ECS
			obj->AddSharedComp<n2::CompScale9>();
#else
			world.AddComponent<e2::CompScale9>(obj);
#endif // GAME_OBJ_ECS
			sz.Build(100, 100);
		}
		break;
	case GAME_OBJ_ANIM:
		{
#ifndef GAME_OBJ_ECS
			sz.Build(100, 100);

			auto layer = std::make_unique<anim::Layer>();
			layer->SetName("Layer0");

			layer->AddKeyFrame(std::make_unique<anim::KeyFrame>(0));

			auto& canim = obj->AddSharedComp<n2::CompAnim>();
			canim.AddLayer(layer);

			obj->AddUniqueComp<n2::CompAnimInst>(canim.GetAnimTemplate());
#endif // GAME_OBJ_ECS
		}
		break;
	case GAME_OBJ_PARTICLE3D:
		{
#ifndef GAME_OBJ_ECS
			sz.Build(100, 100);

			obj->AddSharedComp<n2::CompParticle3d>(particle3d::MAX_COMPONENTS);
			auto& cp3d = obj->GetSharedCompPtr<n2::CompParticle3d>();
			obj->AddUniqueComp<n2::CompParticle3dInst>(cp3d);
#endif // GAME_OBJ_ECS
		}
		break;

	case GAME_OBJ_SCENE2D:
		{
#ifndef GAME_OBJ_ECS
			obj->AddSharedComp<n2::CompComplex>();
#else
			world.AddComponent<e2::CompComplex>(obj);
#endif // GAME_OBJ_ECS
			sz.Build(100, 100);
		}
		break;
	}

	// transform
#ifndef GAME_OBJ_ECS
	obj->AddUniqueComp<n2::CompTransform>();
#else
	world.AddComponent<e2::CompLocalMat>(obj);
#endif // GAME_OBJ_ECS

	// aabb
#ifndef GAME_OBJ_ECS
	obj->AddUniqueComp<n2::CompBoundingBox>(sz);
#else
	world.AddComponent<e2::CompBoundingBox>(obj, sz);
#endif // GAME_OBJ_ECS

	// editor
#ifndef GAME_OBJ_ECS
	obj->AddUniqueComp<ee0::CompNodeEditor>();
#else
	world.AddComponent<ee0::CompEntityEditor>(obj);
#endif // GAME_OBJ_ECS

	return obj;
}

}