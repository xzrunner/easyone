#include "script/WxStagePage.h"

#include "frame/WxStagePage.h"
#include "frame/Blackboard.h"
#include "frame/Application.h"
#include "frame/typedef.h"

#include <ee0/SubjectMgr.h>
#include <ee2/WxStageDropTarget.h>

#include <guard/check.h>
#include <node0/SceneNode.h>
#include <node2/CompComplex.h>

namespace eone
{
namespace script
{

WxStagePage::WxStagePage(wxWindow* parent, ee0::WxLibraryPanel* library, const n0::SceneNodePtr& node)
	: eone::WxStagePage(parent, node)
{
}

void WxStagePage::OnNotify(uint32_t msg, const ee0::VariantSet& variants)
{
	eone::WxStagePage::OnNotify(msg, variants);

	switch (msg)
	{
	case ee0::MSG_STAGE_PAGE_ON_SHOW:
		StagePageOnShow();
		break;
	}
}

void WxStagePage::Traverse(std::function<bool(const n0::SceneNodePtr&)> func,
	                       const ee0::VariantSet& variants, bool inverse) const
{
	auto var = variants.GetVariant("preview");
	if (var.m_type == ee0::VT_EMPTY) {
		auto& ccomplex = m_node->GetSharedComp<n2::CompComplex>();
		ccomplex.Traverse(func, inverse);
	} else {
		func(m_node);
	}
}

const n0::NodeSharedComp& WxStagePage::GetEditedNodeComp() const 
{
	return m_node->GetSharedComp<n2::CompComplex>();
}

void WxStagePage::StoreToJsonExt(const std::string& dir, rapidjson::Value& val, 
	                             rapidjson::MemoryPoolAllocator<>& alloc) const
{
//	val.AddMember("camera", "2d", alloc);
}

void WxStagePage::StagePageOnShow()
{
	auto& ui_mgr = Blackboard::Instance()->GetApp()->GetUIManager();
	ui_mgr.GetPane(STR_PREVIEW_PANEL).Show();
	ui_mgr.GetPane(STR_STAGE_EXT_PANEL).Hide();
	ui_mgr.Update();
}

}
}