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

WxStagePage::WxStagePage(wxWindow* parent, ee0::WxLibraryPanel* library, const ee0::GameObj& obj)
	: eone::WxStagePage(parent, obj, LAYOUT_PREVIEW)
{
}

void WxStagePage::Traverse(std::function<bool(const ee0::GameObj&)> func,
	                       const ee0::VariantSet& variants, bool inverse) const
{
	auto var = variants.GetVariant("type");
	if (var.m_type == ee0::VT_EMPTY) {
		m_obj->GetSharedComp<n2::CompComplex>().Traverse(func, inverse);
		return;
	}

	GD_ASSERT(var.m_type == ee0::VT_LONG, "err type");
	switch (var.m_val.l)
	{
	case TRAV_DRAW_PREVIEW:
		func(m_obj);
		break;
	default:
		m_obj->GetSharedComp<n2::CompComplex>().Traverse(func, inverse);
	}
}

const n0::NodeSharedComp& WxStagePage::GetEditedObjComp() const 
{
	return m_obj->GetSharedComp<n2::CompComplex>();
}

void WxStagePage::StoreToJsonExt(const std::string& dir, rapidjson::Value& val, 
	                             rapidjson::MemoryPoolAllocator<>& alloc) const
{
//	val.AddMember("camera", "2d", alloc);
}

}
}