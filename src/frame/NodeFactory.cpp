#include "frame/NodeFactory.h"

#include <ee0/CompNodeEditor.h>

#include <node0/SceneNode.h>
#include <node0/CompComplex.h>
#include <node2/CompImage.h>
#include <node2/CompText.h>
#include <node2/CompMask.h>
#include <node2/CompMesh.h>
#include <node2/CompScale9.h>
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
			auto& cimage = node->AddComponent<n2::CompImage>();
			sz.Build(100, 100);
		}
		break;
	case NODE_TEXT:
		{
			auto& ctext = node->AddComponent<n2::CompText>();
			auto& tb = ctext.GetText().tb;
			sz.Build(static_cast<float>(tb.width), static_cast<float>(tb.height));
		}
		break;
	case NODE_MASK:
		{
			node->AddComponent<n2::CompMask>();
			sz.Build(100, 100);
		}
		break;
	case NODE_MESH:
		{
			node->AddComponent<n2::CompMesh>();
			sz.Build(100, 100);
		}
		break;
	case NODE_SCALE9:
		{
			node->AddComponent<n2::CompScale9>();
			sz.Build(100, 100);
		}
		break;

	case NODE_SPRITE2:
		{
			node->AddComponent<n2::CompSprite2>();
			sz.Build(100, 100);
		}
		break;
	case NODE_SCENE2D:
		{
			node->AddComponent<n0::CompComplex>();
			sz.Build(100, 100);
		}
		break;
	}

	// transform
	auto& ctrans = node->AddComponent<n2::CompTransform>();

	// aabb
	auto& cbounding = node->AddComponent<n2::CompBoundingBox>(sz);
	cbounding.Build(ctrans.GetTrans().GetSRT());

	// editor
	node->AddComponent<ee0::CompNodeEditor>();

	return node;
}

}