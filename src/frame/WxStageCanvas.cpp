#include "frame/WxStageCanvas.h"
#include "frame/WxStagePage.h"
#include "frame/Blackboard.h"

#include <ee0/CameraHelper.h>
#include <ee0/SubjectMgr.h>

#include <node0/SceneNode.h>
#include <node2/CompScissor.h>
#include <node2/RenderSystem.h>
#include <node2/UpdateSystem.h>

#include <wx/frame.h>

namespace eone
{

WxStageCanvas::WxStageCanvas(WxStagePage* stage,
	                         const ee0::RenderContext& rc)
	: ee2::WxStageCanvas(stage, &rc)
{
}

void WxStageCanvas::DrawNodes() const
{	
	ee2::WxStageCanvas::DrawNodes();

	auto obj = static_cast<WxStagePage*>(m_stage)->GetEditedObj();
	if (obj->HasUniqueComp<n2::CompScissor>()) 
	{
		auto& cscissor = obj->GetUniqueComp<n2::CompScissor>();
		n2::RenderSystem::DrawScissorRect(cscissor.GetRect(), sm::Matrix2D());
	}
}

bool WxStageCanvas::OnUpdate()
{
	auto obj = static_cast<WxStagePage*>(m_stage)->GetEditedObj();
	bool dirty = n2::UpdateSystem::Update(obj);
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
}

}