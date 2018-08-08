#pragma once

#include "blueprint/Graph.h"

#include <node0/NodeComp.h>

namespace bp
{

class CompGraph : public n0::NodeComp
{
public:
	virtual const char* Type() const override { return TYPE_NAME; }
	virtual n0::CompID TypeID() const override {
		return n0::GetCompTypeID<CompGraph>();
	}
	virtual std::unique_ptr<n0::NodeComp> Clone(const n0::SceneNode& node) const override;
	
	const Graph& GetGraph() const { return m_graph; }

	static const char* const TYPE_NAME;

private:
	Graph m_graph;

}; // CompGraph

}