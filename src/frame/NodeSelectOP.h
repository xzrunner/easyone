#pragma once

#include <ee2/NodeSelectOP.h>

namespace ee0 { class RenderContext; class WindowContext; }

namespace eone
{

class WxStagePage;

class NodeSelectOP : public ee2::NodeSelectOP
{
public:
	NodeSelectOP(
#ifdef GAME_OBJ_ECS
		ecsx::World& world,
#endif // GAME_OBJ_ECS
		WxStagePage& stage, 
		const ee0::RenderContext& rc,
		const ee0::WindowContext& wc);
	
	virtual bool OnMouseLeftDClick(int x, int y);

private:
#ifdef GAME_OBJ_ECS
	ecsx::World& m_world;
#endif // GAME_OBJ_ECS

	const ee0::RenderContext& m_rc;
	const ee0::WindowContext& m_wc;

}; // NodeSelectOP

}