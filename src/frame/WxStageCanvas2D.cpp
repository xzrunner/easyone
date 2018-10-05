#include "frame/WxStageCanvas2D.h"
#include "frame/WxStagePage.h"
#include "frame/Blackboard.h"

#include <ee0/CameraHelper.h>
#include <ee0/SubjectMgr.h>

#ifndef GAME_OBJ_ECS
#include <node0/SceneNode.h>
#include <node2/CompScissor.h>
#include <node2/RenderSystem.h>
#include <node2/UpdateSystem.h>
#else
#endif // GAME_OBJ_ECS
#include <moon/Blackboard.h>

#include <wx/frame.h>

namespace eone
{

WxStageCanvas2D::WxStageCanvas2D(WxStagePage* stage, ECS_WORLD_PARAM
	                             const ee0::RenderContext& rc)
	: ee2::WxStageCanvas(stage, ECS_WORLD_VAR &rc)
{
}

void WxStageCanvas2D::DrawForeground() const
{
	ee2::WxStageCanvas::DrawForeground();

#ifndef GAME_OBJ_ECS
	auto obj = static_cast<WxStagePage*>(m_stage)->GetEditedObj();
	if (obj->HasUniqueComp<n2::CompScissor>())
	{
		auto& cscissor = obj->GetUniqueComp<n2::CompScissor>();
		n2::RenderSystem::Instance()->DrawScissorRect(cscissor.GetRect(), sm::Matrix2D());
	}
#else
	// todo ecs
#endif // GAME_OBJ_ECS
}

bool WxStageCanvas2D::OnUpdate()
{
	bool dirty = false;
	m_stage->Traverse([&](const ee0::GameObj& obj)->bool
	{
		if (n2::UpdateSystem::Instance()->Update(obj)) {
			dirty = true;
		}
		return true;
	});

	if (dirty) {
		m_stage->GetSubjectMgr()->NotifyObservers(ee0::MSG_UPDATE_NODES);
	}
	return dirty;
}

void WxStageCanvas2D::OnMouseImpl(wxMouseEvent& event)
{
	const wxPoint pt = wxGetMousePosition();
	int mouse_x = pt.x - m_stage->GetScreenPosition().x;
	int mouse_y = pt.y - m_stage->GetScreenPosition().y;

	sm::vec2 pos = ee0::CameraHelper::TransPosScreenToProject(
		*GetCamera(), mouse_x, mouse_y);

	wxString msg;
	msg.Printf("Mouse: %.1f, %.1f", pos.x, pos.y);
	Blackboard::Instance()->GetFrame()->SetStatusText(msg);
}

}