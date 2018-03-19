#include "frame/WxDetailPanel.h"
#include "frame/WxStagePage.h"

#include <ee0/WxListSelectDlg.h>
#include <ee0/SubjectMgr.h>
#include <ee0/VariantSet.h>
#include <ee0/WxCompPanel.h>
#include <ee0/WxCompNodeEditorPanel.h>
#include <ee2/WxCompTransformPanel.h>
#include <ee2/WxCompColComPanel.h>
#include <ee2/WxCompColMapPanel.h>
#include <ee2/WxCompImagePanel.h>
#include <ee2/WxCompTextPanel.h>
#include <ee2/WxCompMaskPanel.h>
#include <ee2/WxCompMeshPanel.h>
#include <ee2/WxCompScale9Panel.h>
#include <ee2/WxCompSprite2Panel.h>
#include <ee2/WxCompScissorPanel.h>
#include <ee2/WxCompScriptPanel.h>
#include <ee3/WxCompTransformPanel.h>

#include <guard/check.h>
#include <node0/SceneNode.h>
#include <dust/LuaVM.h>
#include <node2/CompScale9.h>
#include <node2/CompScissor.h>
#include <node2/CompScript.h>

#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/treectrl.h>
#include <wx/dialog.h>

namespace
{

enum CompType
{
	COMP_UNKNOWN = 0,

	COMP_COLOR_COMMON = 1,
	COMP_COLOR_MAP,
	COMP_SCISSOR,
	COMP_SCRIPT,
};

static const std::vector<std::pair<uint32_t, std::string>> COMP_LIST =
{
	std::make_pair(COMP_COLOR_COMMON, "ColorCommon"),
	std::make_pair(COMP_COLOR_MAP,    "ColorMap"),
	std::make_pair(COMP_SCISSOR,      "Scissor"),
	std::make_pair(COMP_SCRIPT,       "Script"),
};

}

namespace eone
{

WxDetailPanel::WxDetailPanel(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr,
	                         const n0::SceneNodePtr& root_node, const dust::LuaVMPtr& lua)
	: wxPanel(parent, wxID_ANY)
	, m_sub_mgr(sub_mgr)
	, m_root_node(root_node)
	, m_lua_vm(lua)
{
	SetBackgroundColour(wxColour(229, 229, 229));

	InitLayout();
	RegisterMsg(*m_sub_mgr);
	InitComponents();
}

void WxDetailPanel::OnNotify(ee0::MessageID msg, const ee0::VariantSet& variants)
{
	switch (msg)
	{
	case ee0::MSG_NODE_SELECTION_INSERT:
		ClearComponents();
		InitComponents(variants);
		break;
	case ee0::MSG_DELETE_SCENE_NODE:
	case ee0::MSG_CLEAR_SCENE_NODE:
	case ee0::MSG_NODE_SELECTION_CLEAR:
		ClearComponents();
		InitComponents();
		break;
	case ee0::MSG_UPDATE_COMPONENTS:
		UpdateComponents();
		break;
	case ee0::MSG_STAGE_PAGE_CHANGED:
		ClearComponents();
		StagePageChanged(variants);
		InitComponents();
		break;
	}
}

void WxDetailPanel::InitLayout()
{
	wxSizer* top_sizer = new wxBoxSizer(wxVERTICAL);
	{
		m_comp_sizer = new wxBoxSizer(wxVERTICAL);
		top_sizer->Add(m_comp_sizer);
	}
	top_sizer->AddSpacer(100);
	{
		m_add_btn = new wxButton(this, wxID_ANY, "Add Component");
		Connect(m_add_btn->GetId(), wxEVT_COMMAND_BUTTON_CLICKED,
			wxCommandEventHandler(WxDetailPanel::OnAddPress));
		top_sizer->Add(m_add_btn, 0, wxALIGN_CENTER_HORIZONTAL);
	}
	SetSizer(top_sizer);
}

void WxDetailPanel::RegisterMsg(ee0::SubjectMgr& sub_mgr)
{
	sub_mgr.RegisterObserver(ee0::MSG_DELETE_SCENE_NODE, this);
	sub_mgr.RegisterObserver(ee0::MSG_CLEAR_SCENE_NODE, this);

	sub_mgr.RegisterObserver(ee0::MSG_NODE_SELECTION_INSERT, this);
	sub_mgr.RegisterObserver(ee0::MSG_NODE_SELECTION_CLEAR, this);

	sub_mgr.RegisterObserver(ee0::MSG_UPDATE_COMPONENTS, this);
	sub_mgr.RegisterObserver(ee0::MSG_STAGE_PAGE_CHANGED, this);
}

void WxDetailPanel::InitComponents(const ee0::VariantSet& variants)
{
	n0::SceneNodePtr node, root;
	size_t node_id = 0;

	auto var_node = variants.GetVariant("node");
	GD_ASSERT(var_node.m_type == ee0::VT_PVOID, "no var in vars: node");
	node = *static_cast<n0::SceneNodePtr*>(var_node.m_val.pv);
	GD_ASSERT(node, "err scene node");

	auto var_root = variants.GetVariant("root");
	if (var_root.m_type != ee0::VT_EMPTY) {
		GD_ASSERT(var_root.m_type == ee0::VT_PVOID, "no var in vars: node");
		root = *static_cast<n0::SceneNodePtr*>(var_root.m_val.pv);
	}

	auto var_id = variants.GetVariant("id");
	if (var_id.m_type != ee0::VT_EMPTY) {
		GD_ASSERT(var_id.m_type == ee0::VT_ULONG, "no var in vars: node");
		node_id = var_id.m_val.ul;
	}

	m_nwp.Init(node, root, node_id);

	InitComponents(node);
}

void WxDetailPanel::InitComponents(const n0::SceneNodePtr& node)
{
	if (m_nwp.GetNode()->HasUniqueComp<ee0::CompNodeEditor>())
	{
		auto& comp = m_nwp.GetNode()->GetUniqueComp<ee0::CompNodeEditor>();
		auto panel = new ee0::WxCompNodeEditorPanel(this, comp, m_sub_mgr, m_nwp.GetNode());
		m_comp_sizer->Insert(m_components.size(), panel);
		m_components.push_back(panel);
	}

	if (m_nwp.GetNode()->HasUniqueComp<n3::CompTransform>())
	{
		auto& comp = m_nwp.GetNode()->GetUniqueComp<n3::CompTransform>();
		auto panel = new ee3::WxCompTransformPanel(this, comp, m_sub_mgr);
		m_comp_sizer->Insert(m_components.size(), panel);
		m_components.push_back(panel);
	}

	if (m_nwp.GetNode()->HasUniqueComp<n2::CompTransform>())
	{
		auto& comp = m_nwp.GetNode()->GetUniqueComp<n2::CompTransform>();
		auto panel = new ee2::WxCompTransformPanel(this, comp, m_sub_mgr, m_nwp);
		m_comp_sizer->Insert(m_components.size(), panel);
		m_components.push_back(panel);
	}
	if (m_nwp.GetNode()->HasUniqueComp<n2::CompColorCommon>())
	{
		auto& comp = m_nwp.GetNode()->GetUniqueComp<n2::CompColorCommon>();
		auto panel = new ee2::WxCompColComPanel(this, comp, m_sub_mgr);
		m_comp_sizer->Insert(m_components.size(), panel);
		m_components.push_back(panel);
	}
	if (m_nwp.GetNode()->HasUniqueComp<n2::CompColorMap>())
	{
		auto& comp = m_nwp.GetNode()->GetUniqueComp<n2::CompColorMap>();
		auto panel = new ee2::WxCompColMapPanel(this, comp, m_sub_mgr);
		m_comp_sizer->Insert(m_components.size(), panel);
		m_components.push_back(panel);
	}

	if (m_nwp.GetNode()->HasSharedComp<n2::CompImage>())
	{
		auto& comp = m_nwp.GetNode()->GetSharedComp<n2::CompImage>();
		auto panel = new ee2::WxCompImagePanel(this, comp);
		m_comp_sizer->Insert(m_components.size(), panel);
		m_components.push_back(panel);
	}
	if (m_nwp.GetNode()->HasSharedComp<n2::CompText>())
	{
		auto& comp = m_nwp.GetNode()->GetSharedComp<n2::CompText>();
		auto panel = new ee2::WxCompTextPanel(this, comp, m_sub_mgr);
		m_comp_sizer->Insert(m_components.size(), panel);
		m_components.push_back(panel);
	}
	if (m_nwp.GetNode()->HasSharedComp<n2::CompMask>())
	{
		auto& comp = m_nwp.GetNode()->GetSharedComp<n2::CompMask>();
		auto panel = new ee2::WxCompMaskPanel(this, comp, *m_nwp.GetNode());
		m_comp_sizer->Insert(m_components.size(), panel);
		m_components.push_back(panel);
	}
	if (m_nwp.GetNode()->HasSharedComp<n2::CompMesh>())
	{
		auto& comp = m_nwp.GetNode()->GetSharedComp<n2::CompMesh>();
		auto panel = new ee2::WxCompMeshPanel(this, comp, *m_nwp.GetNode());
		m_comp_sizer->Insert(m_components.size(), panel);
		m_components.push_back(panel);
	}
	if (m_nwp.GetNode()->HasSharedComp<n2::CompScale9>())
	{
		auto& comp = m_nwp.GetNode()->GetSharedComp<n2::CompScale9>();
		auto panel = new ee2::WxCompScale9Panel(this, comp);
		m_comp_sizer->Insert(m_components.size(), panel);
		m_components.push_back(panel);
	}

	if (m_nwp.GetNode()->HasSharedComp<n2::CompSprite2>())
	{
		auto& comp = m_nwp.GetNode()->GetSharedComp<n2::CompSprite2>();
		auto panel = new ee2::WxCompSprite2Panel(this, comp);
		m_comp_sizer->Insert(m_components.size(), panel);
		m_components.push_back(panel);
	}

	if (m_nwp.GetNode()->HasUniqueComp<n2::CompScissor>())
	{
		auto& comp = m_nwp.GetNode()->GetUniqueComp<n2::CompScissor>();
		auto panel = new ee2::WxCompScissorPanel(this, comp, m_sub_mgr);
		m_comp_sizer->Insert(m_components.size(), panel);
		m_components.push_back(panel);
	}

	if (m_nwp.GetNode()->HasUniqueComp<n2::CompScript>())
	{
		auto& comp = m_nwp.GetNode()->GetUniqueComp<n2::CompScript>();
		auto panel = new ee2::WxCompScriptPanel(this, comp, m_sub_mgr, m_nwp.GetNode());
		m_comp_sizer->Insert(m_components.size(), panel);
		m_components.push_back(panel);
	}

	Layout();
}

void WxDetailPanel::InitComponents()
{
	m_nwp.Init(m_root_node, m_root_node, 0);
	InitComponents(m_root_node);
}

void WxDetailPanel::ClearComponents()
{
	m_nwp.Reset();
	if (m_children.empty()) {
		return;
	}
	for (int i = 0, n = m_components.size(); i < n; ++i) {
		m_comp_sizer->Detach(m_components[i]);
		delete m_components[i];
	}
	m_components.clear();

	Layout();
}

void WxDetailPanel::UpdateComponents()
{
	if (!m_nwp.GetNode()) {
		return;
	}
	for (auto& comp : m_components) {
		comp->RefreshNodeComp();
	}
}

void WxDetailPanel::StagePageChanged(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("new_page");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: new_page");
	GD_ASSERT(var.m_val.pv, "err new_page");

	auto new_page = static_cast<WxStagePage*>(var.m_val.pv);

	m_sub_mgr = new_page->GetSubjectMgr();
	RegisterMsg(*m_sub_mgr);

	m_root_node = new_page->GetEditedNode();

	m_lua_vm = new_page->GetLuaVM();
}

void WxDetailPanel::OnAddPress(wxCommandEvent& event)
{
	ee0::WxListSelectDlg dlg(this, "Create component",
		COMP_LIST, m_add_btn->GetScreenPosition());
	if (dlg.ShowModal() != wxID_OK) {
		return;
	}

	auto id = dlg.GetSelectedID();
	switch (id)
	{
	case CompType::COMP_COLOR_COMMON:
		{
			auto& comp = m_nwp.GetNode()->AddUniqueComp<n2::CompColorCommon>();
			auto panel = new ee2::WxCompColComPanel(this, comp, m_sub_mgr);
			m_comp_sizer->Insert(m_components.size(), panel);
			m_components.push_back(panel);
		}
		break;
	case CompType::COMP_COLOR_MAP:
		{
			auto& comp = m_nwp.GetNode()->AddUniqueComp<n2::CompColorMap>();
			auto panel = new ee2::WxCompColMapPanel(this, comp, m_sub_mgr);
			m_comp_sizer->Insert(m_components.size(), panel);
			m_components.push_back(panel);
		}
		break;
	case CompType::COMP_SCISSOR:
		{
			auto& comp = m_nwp.GetNode()->AddUniqueComp<n2::CompScissor>();
			comp.SetRect(sm::rect(100, 100));
			auto panel = new ee2::WxCompScissorPanel(this, comp, m_sub_mgr);
			m_comp_sizer->Insert(m_components.size(), panel);
			m_components.push_back(panel);

			m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
		}
		break;
	case CompType::COMP_SCRIPT:
		{
			auto& comp = m_nwp.GetNode()->AddUniqueComp<n2::CompScript>();
			auto panel = new ee2::WxCompScriptPanel(this, comp, m_sub_mgr, m_nwp.GetNode());
			m_comp_sizer->Insert(m_components.size(), panel);
			m_components.push_back(panel);
		}
		break;
	}

	Layout();
}

}