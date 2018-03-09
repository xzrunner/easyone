#include "frame/WxStagePage.h"
#include "frame/Blackboard.h"
#include "frame/WxStagePanel.h"
#include "frame/StagePageType.h"

#include <node0/CompAsset.h>
#include <node0/SceneNode.h>
#include <node2/CompBoundingBox.h>
#include <ns/CompSerializer.h>

#include <guard/check.h>

namespace eone
{

WxStagePage::WxStagePage(wxWindow* parent, const n0::SceneNodePtr& node)
	: ee0::WxStagePage(parent)
	, m_node(node)
{
	m_sub_mgr.RegisterObserver(ee0::MSG_SET_EDITOR_DIRTY, this);
}

void WxStagePage::OnNotify(ee0::MessageID msg, const ee0::VariantSet& variants)
{
	ee0::WxStagePage::OnNotify(msg, variants);

	switch (msg)
	{
	case ee0::MSG_SET_EDITOR_DIRTY:
		SetEditorDirty(variants);
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