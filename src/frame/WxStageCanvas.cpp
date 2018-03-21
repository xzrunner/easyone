#include "frame/WxStageCanvas.h"
#include "frame/WxStagePage.h"
#include "frame/Blackboard.h"

#include <ee0/CameraHelper.h>

#include <node0/SceneNode.h>
#include <node2/CompScissor.h>
#include <node2/RenderSystem.h>

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

	auto node = static_cast<WxStagePage*>(m_stage)->GetEditedNode();
	if (node->HasUniqueComp<n2::CompScissor>()) 
	{
		auto& cscissor = node->GetUniqueComp<n2::CompScissor>();
		n2::RenderSystem::DrawScissorRect(cscissor.GetRect(), sm::Matrix2D());
	}
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