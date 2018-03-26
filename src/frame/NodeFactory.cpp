#include "frame/NodeFactory.h"

#include <ee0/CompNodeEditor.h>

#include <node0/SceneNode.h>
#include <node2/CompComplex.h>
#include <node2/CompImage.h>
#include <node2/CompText.h>
#include <node2/CompMask.h>
#include <node2/CompMesh.h>
#include <node2/CompScale9.h>
#include <node2/CompAnim.h>
#include <node2/CompSprite2.h>
#include <node2/CompBoundingBox.h>
#include <node2/CompTransform.h>

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

			auto layer = std::make_unique<n2::CompAnim::Layer>();
			layer->name = "Layer0";

			auto frame = std::make_unique<n2::CompAnim::Frame>();
			frame->index = 0;
			layer->frames.push_back(std::move(frame));

			frame = std::make_unique<n2::CompAnim::Frame>();
			frame->index = 10;
			layer->frames.push_back(std::move(frame));

			auto& canim = node->AddSharedComp<n2::CompAnim>();
			canim.AddLayer(layer);
		}
		break;

	case NODE_SPRITE2:
		{
			node->AddSharedComp<n2::CompSprite2>();
			sz.Build(100, 100);
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