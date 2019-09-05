#include "ittgraph/WxToolbarPanel.h"

#ifdef MODULE_ITTGRAPH

#include "ittgraph/WxStagePage.h"

#include "frame/WxStagePage.h"

#include <ee0/SubjectMgr.h>
#include <ee0/WxNavigationBar.h>
#include <blueprint/CompNode.h>
#include <blueprint/MessageID.h>
#include <intention/WxNodeProperty.h>
#include <intention/MessageID.h>
#include <intention/Node.h>

#include <guard/check.h>
#include <node0/SceneNode.h>

#include <wx/sizer.h>

namespace eone
{
namespace ittgraph
{

WxToolbarPanel::WxToolbarPanel(wxWindow* parent, eone::WxStagePage* stage_page)
	: wxPanel(parent)
    , m_stage_page(stage_page)
{
	InitLayout();

    auto& sub_mgr = stage_page->GetSubjectMgr();
    sub_mgr->RegisterObserver(ee0::MSG_NODE_SELECTION_INSERT, this);
    sub_mgr->RegisterObserver(ee0::MSG_NODE_SELECTION_CLEAR, this);
    sub_mgr->RegisterObserver(itt::MSG_SCENE_ROOT_TO_NEXT_LEVEL, this);
}

void WxToolbarPanel::OnNotify(uint32_t msg, const ee0::VariantSet& variants)
{
	switch (msg)
	{
	case ee0::MSG_NODE_SELECTION_INSERT:
		OnSelectionInsert(variants);
		break;
    case ee0::MSG_NODE_SELECTION_CLEAR:
        OnSelectionClear(variants);
        break;
    case itt::MSG_SCENE_ROOT_TO_NEXT_LEVEL:
        OnRootToNextLevel(variants);
        break;
	}
}

void WxToolbarPanel::InitLayout()
{
    auto sub_mgr = m_stage_page->GetSubjectMgr();

	auto sizer = new wxBoxSizer(wxVERTICAL);
    // property
	sizer->Add(m_prop = new itt::WxNodeProperty(this, sub_mgr), wxEXPAND);
    // nav bar
    m_nav_bar = new ee0::WxNavigationBar(this);
    m_nav_bar->SetSeekCallback([&](size_t depth)
    {
        ee0::VariantSet vars;

        ee0::Variant var;
        var.m_type = ee0::VT_ULONG;
        var.m_val.ul = depth;
        vars.SetVariant("depth", var);

        m_stage_page->GetSubjectMgr()->NotifyObservers(itt::MSG_SCENE_ROOT_SEEK_TO_PREV_LEVEL, vars);
    });
    sizer->Add(m_nav_bar);

	SetSizer(sizer);
}

void WxToolbarPanel::OnSelectionInsert(const ee0::VariantSet& variants)
{
	auto var_obj = variants.GetVariant("obj");
	GD_ASSERT(var_obj.m_type == ee0::VT_PVOID, "no var in vars: obj");
    const ee0::GameObj obj = *static_cast<const ee0::GameObj*>(var_obj.m_val.pv);
	GD_ASSERT(GAME_OBJ_VALID(obj), "err scene obj");

	auto& cnode = obj->GetUniqueComp<bp::CompNode>();
	m_prop->LoadFromNode(obj, cnode.GetNode());
}

void WxToolbarPanel::OnSelectionClear(const ee0::VariantSet& variants)
{
    m_prop->Clear();
}

void WxToolbarPanel::OnRootToNextLevel(const ee0::VariantSet& variants)
{
    auto var_obj = variants.GetVariant("obj");
    GD_ASSERT(var_obj.m_type == ee0::VT_PVOID, "no var in vars: obj");
    const ee0::GameObj obj = *static_cast<const ee0::GameObj*>(var_obj.m_val.pv);
    GD_ASSERT(GAME_OBJ_VALID(obj), "err scene obj");

    if (!obj->HasUniqueComp<bp::CompNode>()) {
        return;
    }
    auto& cnode = obj->GetUniqueComp<bp::CompNode>();
    auto& bp_node = cnode.GetNode();
    if (!bp_node->get_type().is_derived_from<itt::Node>()) {
        return;
    }

    auto itt_node = std::static_pointer_cast<itt::Node>(bp_node);
    m_nav_bar->Push(itt_node->GetName());
}

}
}

#endif // MODULE_ITTGRAPH