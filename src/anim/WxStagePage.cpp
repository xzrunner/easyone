#include "anim/WxStagePage.h"
#include "anim/WxTimelinePanel.h"
#include "anim/MessageID.h"
#include "anim/AnimHelper.h"

#include "frame/WxStagePage.h"
#include "frame/Blackboard.h"
#include "frame/Application.h"
#include "frame/typedef.h"
#include "frame/WxStageExtPanel.h"

#include <ee0/SubjectMgr.h>
#include <ee2/WxStageDropTarget.h>

#include <guard/check.h>
#include <logger.h>
#include <node0/SceneNode.h>
#include <node2/CompAnim.h>
#include <node2/CompAnimInst.h>

#include <wx/aui/framemanager.h>

namespace eone
{
namespace anim
{

WxStagePage::WxStagePage(wxWindow* parent, ee0::WxLibraryPanel* library, const n0::SceneNodePtr& node)
	: eone::WxStagePage(parent, node, SUB_WND_STAGE_EXT)
{
	auto& canim_inst = node->GetUniqueComp<n2::CompAnimInst>();
	canim_inst.GetPlayCtrl().SetActive(false);

	m_messages.push_back(MSG_SET_CURR_FRAME);
	m_messages.push_back(MSG_REFRESH_ANIM_COMP);

	if (library) {
		SetDropTarget(new ee2::WxStageDropTarget(library, this));
	}
}

void WxStagePage::OnNotify(uint32_t msg, const ee0::VariantSet& variants)
{
	eone::WxStagePage::OnNotify(msg, variants);

	bool dirty = false;
	switch (msg)
	{
	case MSG_SET_CURR_FRAME:
		dirty = OnSetCurrFrame(variants);
		break;
	case MSG_REFRESH_ANIM_COMP:
		dirty = OnRefreshAnimComp();
		break;
	}

	if (dirty) {
		m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
	}
}

void WxStagePage::Traverse(std::function<bool(const n0::SceneNodePtr&)> func,
	                       const ee0::VariantSet& variants, bool inverse) const
{
	auto var = variants.GetVariant("type");
	if (var.m_type == ee0::VT_EMPTY) {
		m_node->GetSharedComp<n2::CompAnim>().Traverse(func, inverse);
		return;
	}

	GD_ASSERT(var.m_type == ee0::VT_LONG, "err type");
	switch (var.m_val.l)
	{
	case TRAV_DRAW:
		func(m_node);
		break;
	case TRAV_DRAW_PREVIEW:
		func(m_node);
		break;
	default:
		m_node->GetSharedComp<n2::CompAnim>().Traverse(func, inverse);
	}
}

void WxStagePage::OnPageInit()
{
	auto panel = Blackboard::Instance()->GetStageExtPanel();
	auto sizer = panel->GetSizer();
	if (sizer) {
		sizer->Clear(true);
	} else {
		sizer = new wxBoxSizer(wxVERTICAL);
	}
	auto& canim = m_node->GetSharedComp<n2::CompAnim>();
	auto& canim_inst = m_node->GetUniqueComp<n2::CompAnimInst>();
	sizer->Add(new WxTimelinePanel(panel, canim, canim_inst, m_sub_mgr), 0, wxEXPAND);
	panel->SetSizer(sizer);
}

const n0::NodeSharedComp& WxStagePage::GetEditedNodeComp() const 
{
	return m_node->GetSharedComp<n2::CompAnim>();
}

void WxStagePage::LoadFromJsonExt(const std::string& dir, const rapidjson::Value& val)
{
	m_sub_mgr->NotifyObservers(MSG_REFRESH_ANIM_COMP);
}

bool WxStagePage::OnSetCurrFrame(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("frame");
	GD_ASSERT(var.m_type == ee0::VT_INT, "err frame");
	int frame = var.m_val.l; 

	auto& canim = m_node->GetSharedComp<n2::CompAnim>();
	frame = std::min(frame, canim.GetAnimTemplate()->GetMaxFrameIdx());
	
	auto& canim_inst = m_node->GetUniqueComp<n2::CompAnimInst>();
	bool ret = canim_inst.SetFrame(frame);
	AnimHelper::UpdateTreePanael(*m_sub_mgr, canim_inst);
	return ret;
}

bool WxStagePage::OnRefreshAnimComp()
{
	LOGI("refresh anim comp");
	auto& canim = m_node->GetSharedCompPtr<n2::CompAnim>();
	canim->GetAnimTemplate()->Build(canim->GetAllLayers());
	return true;
}

}
}