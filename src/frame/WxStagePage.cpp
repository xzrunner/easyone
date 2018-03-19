#include "frame/WxStagePage.h"
#include "frame/Blackboard.h"
#include "frame/WxStagePanel.h"
#include "frame/StagePageType.h"

#include <ee0/SubjectMgr.h>

#include <guard/check.h>
#include <node0/CompAsset.h>
#include <node0/SceneNode.h>
#include <node2/CompBoundingBox.h>
#include <ns/CompSerializer.h>
#include <dust/Blackboard.h>

namespace eone
{

WxStagePage::WxStagePage(wxWindow* parent, const n0::SceneNodePtr& node)
	: ee0::WxStagePage(parent)
	, m_node(node)
{
	m_sub_mgr->RegisterObserver(ee0::MSG_SET_EDITOR_DIRTY, this);
	m_sub_mgr->RegisterObserver(ee0::MSG_STAGE_PAGE_ON_SHOW, this);
}

void WxStagePage::OnNotify(ee0::MessageID msg, const ee0::VariantSet& variants)
{
	ee0::WxStagePage::OnNotify(msg, variants);

	switch (msg)
	{
	case ee0::MSG_SET_EDITOR_DIRTY:
		SetEditorDirty(variants);
		break;

	case ee0::MSG_STAGE_PAGE_ON_SHOW:
		dust::Blackboard::Instance()->ctx = GetDustCtx();
		break;
	}
}

void WxStagePage::StoreToJson(const std::string& dir, rapidjson::Value& val,
	                          rapidjson::MemoryPoolAllocator<>& alloc) const
{
	ns::CompSerializer::Instance()->ToJson(GetEditedNodeComp(), dir, val, alloc);
	StoreToJsonExt(dir, val, alloc);
}

void WxStagePage::LoadFromJson(const std::string& dir, const rapidjson::Value& val)
{
	ns::CompSerializer::Instance()->FromJson(m_node, dir, val);
	LoadFromJsonExt(dir, val);

	auto& casset = m_node->GetSharedComp<n0::CompAsset>();
	auto& cbb = m_node->GetUniqueComp<n2::CompBoundingBox>();
	cbb.SetSize(*m_node, casset.GetBounding());

	casset.Traverse([&](const n0::SceneNodePtr& node)->bool
	{
		ee0::VariantSet vars;

		ee0::Variant var_skip;
		var_skip.m_type = ee0::VT_PVOID;
		var_skip.m_val.pv = static_cast<Observer*>(this);
		vars.SetVariant("skip_observer", var_skip);

		ee0::Variant var_node;
		var_node.m_type = ee0::VT_PVOID;
		var_node.m_val.pv = &std::const_pointer_cast<n0::SceneNode>(node);
		vars.SetVariant("node", var_node);

		m_sub_mgr->NotifyObservers(ee0::MSG_INSERT_SCENE_NODE, vars);

		return true;
	});

	m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
}

void WxStagePage::SetEditorDirty(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("dirty");
	GD_ASSERT(var.m_type == ee0::VT_BOOL, "err val");
	auto stage_panel = Blackboard::Instance()->GetStagePanel();
	auto stage_page = stage_panel->GetPage(stage_panel->GetSelection());
	std::string page_name = GetPageName(static_cast<WxStagePage*>(stage_page)->GetPageType());
	if (var.m_val.bl) {
		page_name += "*";
	}
	stage_panel->SetPageText(stage_panel->GetSelection(), page_name);
}

}