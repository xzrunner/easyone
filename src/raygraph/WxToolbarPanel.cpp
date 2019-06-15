#include "raygraph/WxToolbarPanel.h"

#ifdef MODULE_RAYGRAPH

//#include "raygraph/WxPreviewPanel.h"
#include "raygraph/WxStagePage.h"

#include "frame/WxStagePage.h"

#include <ee0/SubjectMgr.h>
#include <blueprint/CompNode.h>
#include <blueprint/MessageID.h>
#include <raylab/WxPreviewPanel.h>
#include <raylab/WxNodeProperty.h>

#include <guard/check.h>
#include <node0/SceneNode.h>

#include <wx/sizer.h>

namespace eone
{
namespace raygraph
{

WxToolbarPanel::WxToolbarPanel(wxWindow* parent, eone::WxStagePage* stage_page)
	: wxPanel(parent)
    , m_stage_page(stage_page)
{
	InitLayout();

    stage_page->GetSubjectMgr()->RegisterObserver(ee0::MSG_NODE_SELECTION_INSERT, this);
}

void WxToolbarPanel::OnNotify(uint32_t msg, const ee0::VariantSet& variants)
{
	switch (msg)
	{
	case ee0::MSG_NODE_SELECTION_INSERT:
		OnSelected(variants);
		break;
	}
}

void WxToolbarPanel::InitLayout()
{
    auto sub_mgr = m_stage_page->GetSubjectMgr();

	auto sizer = new wxBoxSizer(wxVERTICAL);
    // preview
    sizer->Add(m_preview = new raylab::WxPreviewPanel(this, wxSize(300, 300), m_stage_page)/*, 1, wxEXPAND*/);
    sizer->AddSpacer(10);
    // property
	sizer->Add(m_prop = new raylab::WxNodeProperty(this, sub_mgr)/*, 1, wxEXPAND*/);

	SetSizer(sizer);
}

void WxToolbarPanel::OnSelected(const ee0::VariantSet& variants)
{
	auto var_obj = variants.GetVariant("obj");
	GD_ASSERT(var_obj.m_type == ee0::VT_PVOID, "no var in vars: obj");
    const ee0::GameObj obj = *static_cast<const ee0::GameObj*>(var_obj.m_val.pv);
	GD_ASSERT(GAME_OBJ_VALID(obj), "err scene obj");

	auto& cnode = obj->GetUniqueComp<bp::CompNode>();
	m_prop->LoadFromNode(obj, cnode.GetNode());
}

}
}

#endif // MODULE_RAYGRAPH