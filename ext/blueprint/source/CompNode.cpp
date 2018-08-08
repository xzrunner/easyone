#include "blueprint/CompNode.h"

namespace bp
{

const char* const CompNode::TYPE_NAME = "bp_node";

std::unique_ptr<n0::NodeComp> CompNode::Clone(const n0::SceneNode& node) const
{
	auto comp = std::make_unique<CompNode>();
	comp->m_node = m_node;
	return comp;
}

}