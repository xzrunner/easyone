#pragma once

#include <ee2/NodeSelectOP.h>

namespace ee0 { class RenderContext; class WindowContext; }

namespace eone
{

class WxStagePage;

class NodeSelectOP : public ee2::NodeSelectOP
{
public:
	NodeSelectOP(WxStagePage& stage, const ee0::RenderContext& rc,
		const ee0::WindowContext& wc);
	
	virtual bool OnMouseLeftDClick(int x, int y);

private:
	const ee0::RenderContext& m_rc;
	const ee0::WindowContext& m_wc;

}; // NodeSelectOP

}