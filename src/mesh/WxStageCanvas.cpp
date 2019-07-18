#include "mesh/WxStageCanvas.h"

#ifdef MODULE_MESH

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
#include <geoshape/Polyline.h>
#include <geoshape/Polygon.h>
#include <drawing2/EditShapeOP.h>
#include <drawing2/RenderStyle.h>
#include <node0/SceneNode.h>
#include <node2/CompMesh.h>
#include <node2/RenderSystem.h>
#include <polymesh2/PointsMesh.h>

namespace eone
{
namespace mesh
{

WxStageCanvas::WxStageCanvas(eone::WxStagePage* stage,
	                         ECS_WORLD_PARAM
	                         const ee0::RenderContext& rc)
	: ee2::WxStageCanvas(stage, ECS_WORLD_VAR &rc)
	, m_edit_view(stage->GetEditedObj()->GetSharedComp<n2::CompMesh>())
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
	float y = sz.y * 0.5f - 40;
	float h = 25;

	if (egui::radio_button(uid++, "BUILD", m_edit_view.GetMode() == EditPointsMeshView::Mode::BUILD, x, y, *ctx, dirty)) {
		m_edit_view.SetMode(EditPointsMeshView::Mode::BUILD);
	}
	y -= h;
	if (egui::radio_button(uid++, "MODEFY", m_edit_view.GetMode() == EditPointsMeshView::Mode::MODIFY, x, y, *ctx, dirty)) {
		m_edit_view.SetMode(EditPointsMeshView::Mode::MODIFY);
	}
	y -= h * 2;

	if (egui::radio_button(uid++, "POINT", shape_type == rttr::type::get<gs::Point2D>().get_id(), x, y, *ctx, dirty)) {
		m_edit_shape_op->ChangeEditState(rttr::type::get<gs::Point2D>().get_id());
	}
	y -= h;
	if (egui::radio_button(uid++, "POLYGON", shape_type == rttr::type::get<gs::Polygon>().get_id(), x, y, *ctx, dirty)) {
		m_edit_shape_op->ChangeEditState(rttr::type::get<gs::Polygon>().get_id());
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

void WxStageCanvas::DrawForeground() const
{
	auto obj = static_cast<WxStagePage*>(m_stage)->GetEditedObj();
	if (!obj->HasSharedComp<n2::CompMesh>()) {
		return;
	}

	auto& cmesh = obj->GetSharedComp<n2::CompMesh>();
	auto& mesh = cmesh.GetMesh();
	if (!mesh) {
		return;
	}

	auto& base = mesh->GetBaseSymbol();
	n2::RenderSystem::Instance()->Draw(base);

	tess::Painter pt;

	auto draw_shape = [&pt](const std::shared_ptr<gs::Shape>& shape)->bool
	{
		pt2::RenderSystem::DrawShape(pt, *shape, 0xff000000);
		return true;
	};
	m_edit_view.Traverse(draw_shape);

	auto pmesh = static_cast<pm2::PointsMesh*>(mesh->GetMesh().get());
	if (pmesh)
	{
		auto& tris = pmesh->GetMeshData();
		for (int i = 0; i < tris->tri_num; )
		{
			pt.AddTriangle(
				tris->vertices[tris->triangles[i++]].xy,
				tris->vertices[tris->triangles[i++]].xy,
				tris->vertices[tris->triangles[i++]].xy,
				0xff000000
			);
		}
	}

	pt2::RenderSystem::DrawPainter(pt, sm::mat4());


}

}
}

#endif // MODULE_MESH