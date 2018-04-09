#include "frame/NodeFactory.h"

#include "particle3d/config.h"

#include <ee0/CompNodeEditor.h>

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

#include <anim/KeyFrame.h>
#include <anim/Layer.h>
#include <emitter/P3dTemplate.h>

namespace eone
{

ee0::GameObj NodeFactory::Create(NodeType type)
{
	if (type == NODE_UNKNOWN) {
		return nullptr;
	}

	ee0::GameObj obj = std::make_shared<n0::SceneNode>();
	sm::rect sz;

	switch (type)
	{
	case NODE_IMAGE:
		{
			auto& cimage = obj->AddSharedComp<n2::CompImage>();
			sz.Build(100, 100);
		}
		break;
	case NODE_TEXT:
		{
			auto& ctext = obj->AddSharedComp<n2::CompText>();
			auto& tb = ctext.GetText().tb;
			sz.Build(static_cast<float>(tb.width), static_cast<float>(tb.height));
		}
		break;
	case NODE_MASK:
		{
			obj->AddSharedComp<n2::CompMask>();
			sz.Build(100, 100);
		}
		break;
	case NODE_MESH:
		{
			obj->AddSharedComp<n2::CompMesh>();
			sz.Build(100, 100);
		}
		break;
	case NODE_SCALE9:
		{
			obj->AddSharedComp<n2::CompScale9>();
			sz.Build(100, 100);
		}
		break;
	case NODE_ANIM:
		{
			sz.Build(100, 100);

			auto layer = std::make_unique<anim::Layer>();
			layer->SetName("Layer0");

			layer->AddKeyFrame(std::make_unique<anim::KeyFrame>(0));

			auto& canim = obj->AddSharedComp<n2::CompAnim>();
			canim.AddLayer(layer);

			obj->AddUniqueComp<n2::CompAnimInst>(canim.GetAnimTemplate());
		}
		break;
	case NODE_PARTICLE3D:
		{
			sz.Build(100, 100);

			obj->AddSharedComp<n2::CompParticle3d>(particle3d::MAX_COMPONENTS);
			auto& cp3d = obj->GetSharedCompPtr<n2::CompParticle3d>();
			obj->AddUniqueComp<n2::CompParticle3dInst>(cp3d);
		}
		break;

	case NODE_SCENE2D:
		{
			obj->AddSharedComp<n2::CompComplex>();
			sz.Build(100, 100);
		}
		break;
	}

	// transform
	auto& ctrans = obj->AddUniqueComp<n2::CompTransform>();

	// aabb
	obj->AddUniqueComp<n2::CompBoundingBox>(sz);

	// editor
	obj->AddUniqueComp<ee0::CompNodeEditor>();

	return obj;
}

}