#pragma once

#include <ee2/NodeSelectOP.h>

namespace eone
{

class NodeSelectOP : public ee2::NodeSelectOP
{
public:
	NodeSelectOP(ee0::WxStagePage& stage);
	
	virtual bool OnMouseLeftDClick(int x, int y);

}; // NodeSelectOP

}