#include "WxDetailPanel.h"

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
#include <ee3/WxCompTransformPanel.h>

#include <node2/CompScale9.h>

#include <guard/check.h>
#include <node0/SceneNode.h>

#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/treectrl.h>
#include <wx/dialog.h>

namespace
{

static const std::string COMP_COLOR_COMMON_STR = "ColorCommon";
static const std::string COMP_COLOR_MAP_STR    = "ColorMap";

class AddDialog : public wxDialog
{
public:
	AddDialog(wxWindow* parent, const wxPoint& pos)
		: wxDialog(parent, wxID_ANY, "Add component", pos)
	{
		InitLayout();
	}

	std::string GetSelectedName() const
	{
		return m_tree->GetItemText(m_tree->GetSelection()).ToStdString();
	}

private:
	void InitLayout()
	{
		wxSizer* sizer = new wxBoxSizer(wxVERTICAL);

		m_tree = new wxTreeCtrl(this, wxID_ANY, wxDefaultPosition, wxSize(200, 400), 
			wxTR_HIDE_ROOT | /*wxTR_NO_LINES | */wxTR_DEFAULT_STYLE);
		Bind(wxEVT_TREE_SEL_CHANGED, &AddDialog::OnSelChanged, this, m_tree->GetId());

		auto root = m_tree->AddRoot("ROOT");

		m_tree->InsertItem(root, -1, COMP_COLOR_COMMON_STR);
		m_tree->InsertItem(root, -1, COMP_COLOR_MAP_STR);

		sizer->Add(m_tree);

		wxBoxSizer* btn_sizer = new wxBoxSizer(wxHORIZONTAL);
		btn_sizer->Add(new wxButton(this, wxID_OK), wxALL, 5);
		btn_sizer->Add(new wxButton(this, wxID_CANCEL), wxALL, 5);
		sizer->Add(btn_sizer, 0, wxALL, 5);

		SetSizer(sizer);
		sizer->Fit(this);
	}

	void OnSelChanged(wxTreeEvent& event)
	{
		auto id = event.GetItem();
		if (!id.IsOk()) {
			return;
		}

		auto text = m_tree->GetItemText(id);
	}

private:
	wxTreeCtrl* m_tree;

}; // AddDialog

}

namespace eone
{

WxDetailPanel::WxDetailPanel(wxWindow* parent, ee0::SubjectMgr& sub_mgr)
	: wxPanel(parent, wxID_ANY)
	, m_sub_mgr(&sub_mgr)
{
	SetBackgroundColour(wxColour(229, 229, 229));

	InitLayout();
	RegisterMsg(*m_sub_mgr);
}

void WxDetailPanel::OnNotify(ee0::MessageID msg, const ee0::VariantSet& variants)
{
	switch (msg)
	{
	case ee0::MSG_NODE_SELECTION_INSERT:
		m_add_btn->Show(true);
		ClearComponents();
		InitComponents(variants);
		break;
	case ee0::MSG_DELETE_SCENE_NODE:
	case ee0::MSG_CLEAR_SCENE_NODE:
	case ee0::MSG_NODE_SELECTION_CLEAR:
		m_add_btn->Show(false);
		ClearComponents();
		break;
	case ee0::MSG_UPDATE_COMPONENTS:
		UpdateComponents();
		break;
	case ee0::MSG_STAGE_PAGE_CHANGING:
		m_add_btn->Show(false);
		ClearComponents();
		StagePageChanging(variants);
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
		m_add_btn->Show(false);
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
	sub_mgr.RegisterObserver(ee0::MSG_STAGE_PAGE_CHANGING, this);
}

void WxDetailPanel::InitComponents(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("node");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: node");
	GD_ASSERT(var.m_val.pv, "err scene node");
	m_node = *static_cast<n0::SceneNodePtr*>(var.m_val.pv);

	if (m_node->HasComponent<ee0::CompNodeEditor>())
	{
		auto& comp = m_node->GetComponent<ee0::CompNodeEditor>();
		auto panel = new ee0::WxCompNodeEditorPanel(this, comp, *m_sub_mgr);
		m_comp_sizer->Insert(m_components.size(), panel);
		m_components.push_back(panel);
	}

	if (m_node->HasComponent<n3::CompTransform>())
	{
		auto& comp = m_node->GetComponent<n3::CompTransform>();
		auto panel = new ee3::WxCompTransformPanel(this, comp, *m_sub_mgr);
		m_comp_sizer->Insert(m_components.size(), panel);
		m_components.push_back(panel);
	}

	if (m_node->HasComponent<n2::CompTransform>())
	{
		auto& comp = m_node->GetComponent<n2::CompTransform>();
		auto panel = new ee2::WxCompTransformPanel(this, comp, *m_sub_mgr, *m_node);
		m_comp_sizer->Insert(m_components.size(), panel);
		m_components.push_back(panel);
	}
	if (m_node->HasComponent<n2::CompColorCommon>())
	{
		auto& comp = m_node->GetComponent<n2::CompColorCommon>();
		auto panel = new ee2::WxCompColComPanel(this, comp, *m_sub_mgr);
		m_comp_sizer->Insert(m_components.size(), panel);
		m_components.push_back(panel);
	}
	if (m_node->HasComponent<n2::CompColorMap>())
	{
		auto& comp = m_node->GetComponent<n2::CompColorMap>();
		auto panel = new ee2::WxCompColMapPanel(this, comp, *m_sub_mgr);
		m_comp_sizer->Insert(m_components.size(), panel);
		m_components.push_back(panel);
	}

	if (m_node->HasComponent<n2::CompImage>())
	{
		auto& comp = m_node->GetComponent<n2::CompImage>();
		auto panel = new ee2::WxCompImagePanel(this, comp, *m_sub_mgr);
		m_comp_sizer->Insert(m_components.size(), panel);
		m_components.push_back(panel);
	}
	if (m_node->HasComponent<n2::CompText>())
	{
		auto& comp = m_node->GetComponent<n2::CompText>();
		auto panel = new ee2::WxCompTextPanel(this, comp, *m_sub_mgr);
		m_comp_sizer->Insert(m_components.size(), panel);
		m_components.push_back(panel);
	}
	if (m_node->HasComponent<n2::CompMask>())
	{
		auto& comp = m_node->GetComponent<n2::CompMask>();
		auto panel = new ee2::WxCompMaskPanel(this, comp, *m_sub_mgr, *m_node);
		m_comp_sizer->Insert(m_components.size(), panel);
		m_components.push_back(panel);
	}
	if (m_node->HasComponent<n2::CompMesh>())
	{
		auto& comp = m_node->GetComponent<n2::CompMesh>();
		auto panel = new ee2::WxCompMeshPanel(this, comp, *m_sub_mgr, *m_node);
		m_comp_sizer->Insert(m_components.size(), panel);
		m_components.push_back(panel);
	}
	if (m_node->HasComponent<n2::CompScale9>())
	{
		auto& comp = m_node->GetComponent<n2::CompScale9>();
		auto panel = new ee2::WxCompScale9Panel(this, comp, *m_sub_mgr);
		m_comp_sizer->Insert(m_components.size(), panel);
		m_components.push_back(panel);
	}

	if (m_node->HasComponent<n2::CompSprite2>())
	{
		auto& comp = m_node->GetComponent<n2::CompSprite2>();
		auto panel = new ee2::WxCompSprite2Panel(this, comp, *m_sub_mgr);
		m_comp_sizer->Insert(m_components.size(), panel);
		m_components.push_back(panel);
	}

	Layout();
}

void WxDetailPanel::ClearComponents()
{
	m_node.reset();
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
	if (!m_node) {
		return;
	}
	for (auto& comp : m_components) {
		comp->RefreshNodeComp();
	}
}

void WxDetailPanel::StagePageChanging(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("sub_mgr");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: sub_mgr");
	GD_ASSERT(var.m_val.pv, "err scene node");
	m_sub_mgr = static_cast<ee0::SubjectMgr*>(var.m_val.pv);

	RegisterMsg(*m_sub_mgr);
}

void WxDetailPanel::OnAddPress(wxCommandEvent& event)
{
	AddDialog dlg(this, m_add_btn->GetScreenPosition());
	if (dlg.ShowModal() != wxID_OK) {
		return;
	}

	auto name = dlg.GetSelectedName();
	if (name == COMP_COLOR_COMMON_STR)
	{
		auto& comp = m_node->AddComponent<n2::CompColorCommon>();
		auto panel = new ee2::WxCompColComPanel(this, comp, *m_sub_mgr);
		m_comp_sizer->Insert(m_components.size(), panel);
		m_components.push_back(panel);
	}
	else if (name == COMP_COLOR_MAP_STR)
	{
		auto& comp = m_node->AddComponent<n2::CompColorMap>();
		auto panel = new ee2::WxCompColMapPanel(this, comp, *m_sub_mgr);
		m_comp_sizer->Insert(m_components.size(), panel);
		m_components.push_back(panel);
	}

	Layout();
}

}