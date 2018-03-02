#pragma once

#include "frame/NodeType.h"

#include <node0/typedef.h>

#include <rapidjson/document.h>

namespace eone
{

class NodeFactory
{
public:
	static n0::SceneNodePtr Create(NodeType type);

}; // NodeFactory

}