#include "blueprint/CompGraph.h"

namespace bp
{

const char* const CompGraph::TYPE_NAME = "bp_graph";

std::unique_ptr<n0::NodeComp> CompGraph::Clone(const n0::SceneNode& node) const
{
	auto comp = std::make_unique<CompGraph>();
	comp->m_graph = m_graph;
	return comp;
}

}