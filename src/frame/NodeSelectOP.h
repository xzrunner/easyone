#pragma once

#include <ee2/NodeSelectOP.h>

namespace ee0 { class RenderContext; }

namespace eone
{

class NodeSelectOP : public ee2::NodeSelectOP
{
public:
	NodeSelectOP(ee0::WxStagePage& stage, 
		const std::shared_ptr<ee0::RenderContext>& preview_rc);
	
	virtual bool OnMouseLeftDClick(int x, int y);

private:
	std::shared_ptr<ee0::RenderContext> m_preview_rc;

}; // NodeSelectOP

}