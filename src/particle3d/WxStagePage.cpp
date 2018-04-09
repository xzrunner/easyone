#include "particle3d/WxStagePage.h"
#include "particle3d/WxEmitterPanel.h"

#include "frame/Blackboard.h"
#include "frame/WxToolbarPanel.h"

#include <ee2/WxStageDropTarget.h>

#include <node0/SceneNode.h>
#include <node2/CompParticle3d.h>
#include <node2/CompParticle3dInst.h>

namespace eone
{
namespace particle3d 
{

WxStagePage::WxStagePage(wxWindow* parent, ee0::WxLibraryPanel* library, const ee0::GameObj& obj)
	: eone::WxStagePage(parent, obj, LAYOUT_TOOLBAR)
	, m_library(library)
{
	if (library) {
		SetDropTarget(new ee2::WxStageDropTarget(library, this));
	}
}

void WxStagePage::OnNotify(uint32_t msg, const ee0::VariantSet& variants)
{
	eone::WxStagePage::OnNotify(msg, variants);

}

void WxStagePage::Traverse(std::function<bool(const ee0::GameObj&)> func,
		                   const ee0::VariantSet& variants, bool inverse) const
{
	auto var = variants.GetVariant("type");
	if (var.m_type == ee0::VT_EMPTY) {
		m_obj->GetSharedComp<n2::CompParticle3d>().Traverse(func, inverse);
		return;
	}

	GD_ASSERT(var.m_type == ee0::VT_LONG, "err type");
	switch (var.m_val.l)
	{
	case TRAV_DRAW:
		func(m_obj);
		break;
	case TRAV_DRAW_PREVIEW:
		func(m_obj);
		break;
	default:
		m_obj->GetSharedComp<n2::CompParticle3d>().Traverse(func, inverse);
	}
}

void WxStagePage::OnPageInit()
{
	auto panel = Blackboard::Instance()->GetToolbarPanel();
	auto sizer = panel->GetSizer();
	if (sizer) {
		sizer->Clear(true);
	} else {
		sizer = new wxBoxSizer(wxVERTICAL);
	}
	auto& cp3d = m_obj->GetSharedComp<n2::CompParticle3d>();
	auto& cp3d_inst = m_obj->GetUniqueComp<n2::CompParticle3dInst>();
	sizer->Add(new WxEmitterPanel(panel, m_library, cp3d, cp3d_inst), 0, wxEXPAND);
	panel->SetSizer(sizer);
}

const n0::NodeSharedComp& WxStagePage::GetEditedObjComp() const
{
	return m_obj->GetSharedComp<n2::CompParticle3d>();
}

}
}