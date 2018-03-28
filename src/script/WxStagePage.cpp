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
	: eone::WxStagePage(parent, node, SUB_WND_PREVIEW)
{
}

void WxStagePage::Traverse(std::function<bool(const n0::SceneNodePtr&)> func,
	                       const ee0::VariantSet& variants, bool inverse) const
{
	auto var = variants.GetVariant("type");
	if (var.m_type == ee0::VT_EMPTY) {
		m_node->GetSharedComp<n2::CompComplex>().Traverse(func, inverse);
		return;
	}

	GD_ASSERT(var.m_type == ee0::VT_LONG, "err type");
	switch (var.m_val.l)
	{
	case TRAV_DRAW_PREVIEW:
		func(m_node);
		break;
	default:
		m_node->GetSharedComp<n2::CompComplex>().Traverse(func, inverse);
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

}
}