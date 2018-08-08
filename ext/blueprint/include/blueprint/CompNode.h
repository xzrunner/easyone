#pragma once

#include "blueprint/Node.h"

#include <node0/NodeComp.h>

namespace bp
{

class CompNode : public n0::NodeComp
{
public:
	virtual const char* Type() const override { return TYPE_NAME; }
	virtual n0::CompID TypeID() const override {
		return n0::GetCompTypeID<CompNode>();
	}
	virtual std::unique_ptr<n0::NodeComp> Clone(const n0::SceneNode& node) const override;

	Node& GetNode() { return m_node; }
	const Node& GetNode() const { return m_node; }

	static const char* const TYPE_NAME;

private:
	Node m_node;

}; // CompNode

}