#include "anim/WxStagePage.h"
#include "anim/WxTimelinePanel.h"

#include "frame/WxStagePage.h"
#include "frame/Blackboard.h"
#include "frame/Application.h"
#include "frame/typedef.h"
#include "frame/WxStageExtPanel.h"

#include <ee0/SubjectMgr.h>
#include <ee2/WxStageDropTarget.h>

#include <guard/check.h>
#include <node0/SceneNode.h>
#include <node2/CompAnim.h>

#include <wx/aui/framemanager.h>

namespace eone
{
namespace anim
{

WxStagePage::WxStagePage(wxWindow* parent, ee0::WxLibraryPanel* library, const n0::SceneNodePtr& node)
	: eone::WxStagePage(parent, node)
{
	m_sub_mgr->RegisterObserver(ee0::MSG_INSERT_SCENE_NODE, this);
	m_sub_mgr->RegisterObserver(ee0::MSG_DELETE_SCENE_NODE, this);
	m_sub_mgr->RegisterObserver(ee0::MSG_CLEAR_SCENE_NODE, this);
	m_sub_mgr->RegisterObserver(ee0::MSG_REORDER_SCENE_NODE, this);

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
	case ee0::MSG_INSERT_SCENE_NODE:
		dirty = InsertSceneNode(variants);
		break;
	case ee0::MSG_DELETE_SCENE_NODE:
		dirty = DeleteSceneNode(variants);
		break;
	case ee0::MSG_CLEAR_SCENE_NODE:
		dirty = ClearSceneNode();
		break;
	case ee0::MSG_REORDER_SCENE_NODE:
		dirty = ReorderSceneNode(variants);
		break;

	case ee0::MSG_STAGE_PAGE_ON_SHOW:
		StagePageOnShow();
		break;
	}

	if (dirty) {
		m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
	}
}

void WxStagePage::Traverse(std::function<bool(const n0::SceneNodePtr&)> func,
	                       const ee0::VariantSet& variants, bool inverse) const
{
	auto var = variants.GetVariant("preview");
	if (var.m_type == ee0::VT_EMPTY) {
		auto& canim = m_node->GetSharedComp<n2::CompAnim>();
		canim.Traverse(func, inverse);
	} else {
		func(m_node);
	}
}

const n0::NodeSharedComp& WxStagePage::GetEditedNodeComp() const 
{
	return m_node->GetSharedComp<n2::CompAnim>();
}

void WxStagePage::StoreToJsonExt(const std::string& dir, rapidjson::Value& val, 
	                             rapidjson::MemoryPoolAllocator<>& alloc) const
{
}

bool WxStagePage::InsertSceneNode(const ee0::VariantSet& variants)
{
	return false;
}

bool WxStagePage::DeleteSceneNode(const ee0::VariantSet& variants)
{
	return false;
}

bool WxStagePage::ClearSceneNode()
{
	return false;
}

bool WxStagePage::ReorderSceneNode(const ee0::VariantSet& variants)
{
	return false;
}

void WxStagePage::StagePageOnShow()
{
	auto bb = Blackboard::Instance();
	auto& ui_mgr = bb->GetApp()->GetUIManager();
	ui_mgr.GetPane(STR_PREVIEW_PANEL).Hide();
	ui_mgr.GetPane(STR_STAGE_EXT_PANEL).Show();
	ui_mgr.Update();

	auto panel = bb->GetStageExtPanel();
	auto sizer = panel->GetSizer();
	if (sizer) {
		sizer->Clear(true);
	} else {
		sizer = new wxBoxSizer(wxVERTICAL);
	}
	auto& canim = m_node->GetSharedComp<n2::CompAnim>();
	sizer->Add(new WxTimelinePanel(panel, canim), 0, wxEXPAND);
	panel->SetSizer(sizer);
}

}
}