#include "frame/WxStageCanvas.h"
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

WxStageCanvas::WxStageCanvas(WxStagePage* stage, ECS_WORLD_PARAM
	                         const ee0::RenderContext& rc)
	: ee2::WxStageCanvas(stage, ECS_WORLD_VAR &rc)
{
}

void WxStageCanvas::DrawNodes() const
{	
	ee2::WxStageCanvas::DrawNodes();

#ifndef GAME_OBJ_ECS
	auto obj = static_cast<WxStagePage*>(m_stage)->GetEditedObj();
	if (obj->HasUniqueComp<n2::CompScissor>()) 
	{
		auto& cscissor = obj->GetUniqueComp<n2::CompScissor>();
		n2::RenderSystem::DrawScissorRect(cscissor.GetRect(), sm::Matrix2D());
	}
#else
	// todo ecs
#endif // GAME_OBJ_ECS
}

bool WxStageCanvas::OnUpdate()
{
	auto obj = static_cast<WxStagePage*>(m_stage)->GetEditedObj();
#ifndef GAME_OBJ_ECS
	bool dirty = n2::UpdateSystem::Update(obj);
#else
	// todo ecs
	bool dirty = false;
#endif // GAME_OBJ_ECS
	if (dirty) {
		m_stage->GetSubjectMgr()->NotifyObservers(ee0::MSG_UPDATE_NODES);
	}
	return dirty;
}

void WxStageCanvas::OnMouseImpl(wxMouseEvent& event)
{
	sm::vec2 pos = ee0::CameraHelper::TransPosScreenToProject(
		*GetCamera(), event.GetX(), event.GetY());

	wxString msg;
	msg.Printf("Mouse: %.1f, %.1f", pos.x, pos.y);
	Blackboard::Instance()->GetFrame()->SetStatusText(msg);

	moon::Blackboard::Instance()->SetMousePos(pos);
}

}