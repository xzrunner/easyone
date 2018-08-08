#pragma once

#include <vector>
#include <memory>

namespace bp
{

class Node;

class Graph
{
public:
	auto& GetAllNodes() const { return m_nodes; }

private:
	std::vector<std::shared_ptr<Node>> m_nodes;

}; // Graph

}