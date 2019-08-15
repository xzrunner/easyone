#include "shape2d/WxStageCanvas.h"

#ifdef MODULE_SHAPE2D

#include "frame/WxStagePage.h"

#include <ee2/CamControlOP.h>

#include <tessellation/Painter.h>
#include <painting2/RenderSystem.h>
#include <painting2/Blackboard.h>
#include <painting2/WindowContext.h>
#include <easygui/ImGui.h>
#include <facade/EasyGUI.h>
#include <facade/Facade.h>
#include <geoshape/config.h>
#include <geoshape/Point2D.h>
#include <geoshape/Rect.h>
#include <geoshape/Circle.h>
#include <geoshape/Polyline2D.h>
#include <drawing2/EditShapeOP.h>
#include <drawing2/RenderStyle.h>

namespace eone
{
namespace shape2d
{

WxStageCanvas::WxStageCanvas(eone::WxStagePage* stage,
	                         ECS_WORLD_PARAM
	                         const ee0::RenderContext& rc)
	: ee2::WxStageCanvas(stage, ECS_WORLD_VAR &rc)
	, m_edit_view(*stage, stage->GetSubjectMgr())
{
	InitGui();

	auto cam_op = std::make_shared<ee2::CamControlOP>(GetCamera(), stage->GetSubjectMgr());
	m_edit_shape_op = std::make_shared<dw2::EditShapeOP>(
		GetCamera(), m_edit_view, dw2::NODE_RADIUS, rttr::type::get<gs::Point2D>().get_id()
	);
	m_edit_shape_op->SetPrevEditOP(cam_op);
	m_stage->GetImpl().SetEditOP(m_edit_shape_op);
	m_edit_shape_op->ChangeEditState(rttr::type::get<gs::Point2D>().get_id());
}

void WxStageCanvas::OnDrawGUI() const
{
	auto& wc = pt2::Blackboard::Instance()->GetWindowContext();
	sm::ivec2 sz(wc->GetScreenWidth(), wc->GetScreenHeight());
	PrepareDrawGui(sz.x, sz.y);

	auto& ctx = GetWidnowContext().egui;

	int uid = 1;

	ctx->BeginDraw();

	bool dirty = facade::Facade::Instance()->IsLastFrameDirty();
	if (m_last_screen_sz != sz) {
		dirty = true;
		m_last_screen_sz = sz;
	}

	uint32_t shape_type = m_edit_shape_op->GetEditShapeType();
	if (m_last_shape_type != shape_type) {
		m_last_shape_type = shape_type;
		dirty = true;
	}

	float x = sz.x * 0.5f - 200;
	float y = sz.y * 0.5f - 140;
	float h = 25;
	if (egui::radio_button(uid++, "POINT", shape_type == rttr::type::get<gs::Point2D>().get_id(), x, y, *ctx, dirty)) {
		m_edit_shape_op->ChangeEditState(rttr::type::get<gs::Point2D>().get_id());
	}
	y -= h;
	if (egui::radio_button(uid++, "RECT", shape_type == rttr::type::get<gs::Rect>().get_id(), x, y, *ctx, dirty)) {
		m_edit_shape_op->ChangeEditState(rttr::type::get<gs::Rect>().get_id());
	}
	y -= h;
	if (egui::radio_button(uid++, "CIRCLE", shape_type == rttr::type::get<gs::Circle>().get_id(), x, y, *ctx, dirty)) {
		m_edit_shape_op->ChangeEditState(rttr::type::get<gs::Circle>().get_id());
	}
	y -= h;
	if (egui::radio_button(uid++, "POLYLINE", shape_type == rttr::type::get<gs::Polyline2D>().get_id(), x, y, *ctx, dirty)) {
		m_edit_shape_op->ChangeEditState(rttr::type::get<gs::Polyline2D>().get_id());
	}
	y -= h;

	ctx->EndDraw();

	// todo
	const float dt = 0.033f;
	ctx->Update(dt);
}

bool WxStageCanvas::OnUpdate()
{
	ee2::WxStageCanvas::OnUpdate();
	return true;
}

void WxStageCanvas::DrawBackground() const
{
}

}
}

#endif // MODULE_SHAPE2D