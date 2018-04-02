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

n0::SceneNodePtr NodeFactory::Create(NodeType type)
{
	if (type == NODE_UNKNOWN) {
		return nullptr;
	}

	n0::SceneNodePtr node = std::make_shared<n0::SceneNode>();
	sm::rect sz;

	switch (type)
	{
	case NODE_IMAGE:
		{
			auto& cimage = node->AddSharedComp<n2::CompImage>();
			sz.Build(100, 100);
		}
		break;
	case NODE_TEXT:
		{
			auto& ctext = node->AddSharedComp<n2::CompText>();
			auto& tb = ctext.GetText().tb;
			sz.Build(static_cast<float>(tb.width), static_cast<float>(tb.height));
		}
		break;
	case NODE_MASK:
		{
			node->AddSharedComp<n2::CompMask>();
			sz.Build(100, 100);
		}
		break;
	case NODE_MESH:
		{
			node->AddSharedComp<n2::CompMesh>();
			sz.Build(100, 100);
		}
		break;
	case NODE_SCALE9:
		{
			node->AddSharedComp<n2::CompScale9>();
			sz.Build(100, 100);
		}
		break;
	case NODE_ANIM:
		{
			sz.Build(100, 100);

			auto layer = std::make_unique<anim::Layer>();
			layer->SetName("Layer0");

			layer->AddKeyFrame(std::make_unique<anim::KeyFrame>(0));

			auto& canim = node->AddSharedComp<n2::CompAnim>();
			canim.AddLayer(layer);

			node->AddUniqueComp<n2::CompAnimInst>(canim.GetAnimTemplate());
		}
		break;
	case NODE_PARTICLE3D:
		{
			sz.Build(100, 100);

			node->AddSharedComp<n2::CompParticle3d>(particle3d::MAX_COMPONENTS);
			auto& cp3d = node->GetSharedCompPtr<n2::CompParticle3d>();
			node->AddUniqueComp<n2::CompParticle3dInst>(cp3d);
		}
		break;

	case NODE_SCENE2D:
		{
			node->AddSharedComp<n2::CompComplex>();
			sz.Build(100, 100);
		}
		break;
	}

	// transform
	auto& ctrans = node->AddUniqueComp<n2::CompTransform>();

	// aabb
	node->AddUniqueComp<n2::CompBoundingBox>(sz);

	// editor
	node->AddUniqueComp<ee0::CompNodeEditor>();

	return node;
}

}