#pragma once

#include <ee0/EditOP.h>

namespace ee0 { class RenderContext; class WindowContext; class WxStagePage; }

namespace eone
{

class LeftDClickOP : public ee0::EditOP
{
public:
	LeftDClickOP(const std::shared_ptr<pt0::Camera>& camera, ee0::WxStagePage& stage,
		const ee0::RenderContext& rc, const ee0::WindowContext& wc);

	virtual bool OnMouseLeftDClick(int x, int y);

private:
    ee0::WxStagePage& m_stage;

	const ee0::RenderContext& m_rc;
	const ee0::WindowContext& m_wc;

}; // LeftDClickOP

}