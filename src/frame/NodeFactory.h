#pragma once

#include <ee0/GameObj.h>

#include "frame/NodeType.h"

namespace eone
{

class NodeFactory
{
public:
	static ee0::GameObj Create(NodeType type);

}; // NodeFactory

}