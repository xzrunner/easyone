#include "SceneTreePanel.h"
#include "SceneTreeCtrl.h"

#include <ee0/MessageID.h>
#include <ee0/VariantSet.h>
#include <ee0/SubjectMgr.h>
#include <ee2/NodeFactory.h>

#include <guard/check.h>
#include <node0/SceneNode.h>
#include <node2/CompImage.h>
#include <node2/CompSprite2.h>
#include <node2/CompTransform.h>
#include <node2/CompBoundingBox.h>
#include <gum/ResPool.h>
#include <gum/SymbolPool.h>
#include <gum/Image.h>
#include <gum/Texture.h>

#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/filedlg.h>

namespace
{

static const std::string NODE_IMAGE_STR   = "Image";
static const std::string NODE_TEXT_STR    = "Text";
static const std::string NODE_SPRITE2_STR = "Sprite2";

class CreateNodeDialog : public wxDialog
{
public:
	CreateNodeDialog(wxWindow* parent, const wxPoint& pos)
		: wxDialog(parent, wxID_ANY, "Create node", pos)
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

		m_tree = new wxTreeCtrl(this, wxID_ANY, wxDefaultPosition, wxSize(200, 200), 
			wxTR_HIDE_ROOT | /*wxTR_NO_LINES | */wxTR_DEFAULT_STYLE);
		Bind(wxEVT_TREE_SEL_CHANGED, &CreateNodeDialog::OnSelChanged, this, m_tree->GetId());

		auto root = m_tree->AddRoot("ROOT");

		m_tree->InsertItem(root, -1, NODE_IMAGE_STR);
		m_tree->InsertItem(root, -1, NODE_TEXT_STR);
		m_tree->InsertItem(root, -1, NODE_SPRITE2_STR);

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

}; // CreateNodeDialog

}

namespace eone
{

SceneTreePanel::SceneTreePanel(wxWindow* parent, ee0::SubjectMgr& sub_mgr)
	: wxPanel(parent, wxID_ANY)
	, m_sub_mgr(sub_mgr)
{
	InitLayout();
}

void SceneTreePanel::InitLayout()
{
	wxSizer* top_sizer = new wxBoxSizer(wxVERTICAL);
	{
		m_create_btn = new wxButton(this, wxID_ANY, "Create Node");
		Connect(m_create_btn->GetId(), wxEVT_COMMAND_BUTTON_CLICKED,
			wxCommandEventHandler(SceneTreePanel::OnCreatePress));
		top_sizer->Add(m_create_btn, 0, wxALIGN_CENTER_HORIZONTAL);
	}
	{
		top_sizer->Add(new SceneTreeCtrl(this, m_sub_mgr), 1, wxEXPAND);
	}
	SetSizer(top_sizer);
}

void SceneTreePanel::OnCreatePress(wxCommandEvent& event)
{
	CreateNodeDialog dlg(this, m_create_btn->GetScreenPosition());
	if (dlg.ShowModal() != wxID_OK) {
		return;
	}

	n0::SceneNodePtr node = nullptr;

	auto name = dlg.GetSelectedName();
	if (name == NODE_IMAGE_STR) 
	{
		std::string filter = "*.png;*.jpg;*.bmp;*.pvr;*.pkm";
		wxFileDialog dlg(this, wxT("Choose image"), wxEmptyString, filter);
		if (dlg.ShowModal() == wxID_OK)
		{
			auto& path = dlg.GetPath();
			auto img = gum::ResPool::Instance().Fetch<gum::Image>(path.ToStdString());

			node = ee2::NodeFactory::Instance()->Create(ee2::NODE_IMAGE);
			auto& cimage = node->GetComponent<n2::CompImage>();
			cimage.SetFilepath(path.ToStdString());
			cimage.SetTexture(img->GetTexture());

			auto& cbounding = node->GetComponent<n2::CompBoundingBox>();
			cbounding.SetSize(sm::rect(img->GetWidth(), img->GetHeight()));
			auto& ctrans = node->GetComponent<n2::CompTransform>();
			cbounding.Build(ctrans.GetTrans().GetSRT());
		}
	} 
	else if (name == NODE_TEXT_STR) 
	{
		node = ee2::NodeFactory::Instance()->Create(ee2::NODE_TEXT);
	}
	else if (name == NODE_SPRITE2_STR)
	{
		wxFileDialog dlg(this, wxT("Choose sprite2"), wxEmptyString, "*.json");
		if (dlg.ShowModal() == wxID_OK)
		{
			auto& path = dlg.GetPath();
			auto sym = gum::SymbolPool::Instance()->Fetch(path.ToStdString().c_str());

			node = ee2::NodeFactory::Instance()->Create(ee2::NODE_SPRITE2);
			auto& csprite2 = node->GetComponent<n2::CompSprite2>();
			csprite2.SetFilepath(path.ToStdString());
			csprite2.SetSymbol(sym);

			auto& cbounding = node->GetComponent<n2::CompBoundingBox>();
			cbounding.SetSize(sym->GetBounding());
			auto& ctrans = node->GetComponent<n2::CompTransform>();
			cbounding.Build(ctrans.GetTrans().GetSRT());
		}
	}
	if (!node) {
		return;
	}
	
	// insert node

	ee0::VariantSet vars;
	ee0::Variant var;
	var.m_type = ee0::VT_PVOID;
	var.m_val.pv = &std::const_pointer_cast<n0::SceneNode>(node);
	vars.SetVariant("node", var);

	bool succ = m_sub_mgr.NotifyObservers(ee0::MSG_INSERT_SCENE_NODE, vars);
	GD_ASSERT(succ, "no MSG_INSERT_SCENE_NODE");
}

}