#include "model/WxToolbarPanel.h"
#include "model/WxStagePage.h"

#include <ee0/SubjectMgr.h>
#include <ee0/WxImageVList.h>
#include <ee0/WxLibraryItem.h>
#include <ee3/WxSkeletalTreeCtrl.h>

#include <node0/SceneNode.h>
#include <node3/CompModelInst.h>
#include <model/Model.h>
#include <model/ModelInstance.h>

#include <wx/radiobox.h>
#include <wx/notebook.h>

namespace eone
{
namespace model
{

WxToolbarPanel::WxToolbarPanel(wxWindow* parent, WxStagePage* stage)
	: wxPanel(parent)
	, m_stage(stage)
{
	InitLayout();
}

void WxToolbarPanel::LoadModel(const ::model::Model& model)
{
	if (model.ext->Type() == ::model::EXT_SKELETAL) {
		auto& skeletal = *static_cast<::model::SkeletalAnim*>(model.ext.get());
		m_tree_page->m_tree->LoadFromSkeletal(skeletal);
	}

	m_texture_page->Clear();
	for (auto& tex : model.textures) {
		auto item = std::make_shared<ee0::WxLibraryItem>(tex.first);
		m_texture_page->Insert(item);
	}


}

void WxToolbarPanel::InitLayout()
{
	wxSizer* top_sizer = new wxBoxSizer(wxVERTICAL);

	m_notebook = new wxNotebook(this, wxID_ANY);

	// edit
	{
		m_edit_page = new wxPanel(m_notebook);

		wxArrayString choices;
		choices.Add("Rotate Joint");
		choices.Add("Translate Joint");
		choices.Add("IK");
		auto radio = new wxRadioBox(m_edit_page, wxID_ANY, "", wxDefaultPosition,
			wxDefaultSize, choices, 1, wxRA_SPECIFY_COLS);
		Connect(radio->GetId(), wxEVT_COMMAND_RADIOBOX_SELECTED,
			wxCommandEventHandler(WxToolbarPanel::OnChangeEditType));

		auto sizer = new wxBoxSizer(wxVERTICAL);
		sizer->Add(radio);
		m_edit_page->SetSizer(sizer);

		m_notebook->AddPage(m_edit_page, "Edit");
	}
	// tree
	{
		m_tree_page = new WxTreeScrolled(m_notebook, m_stage->GetSubjectMgr());
		m_notebook->AddPage(m_tree_page, "Tree");
	}
	// texture
	{
		m_texture_page = new ee0::WxImageVList(m_notebook, "");
		m_notebook->AddPage(m_texture_page, "Textures");
	}

	top_sizer->Add(m_notebook, 1, wxEXPAND);
	SetSizer(top_sizer);
}

void WxToolbarPanel::OnChangeEditType(wxCommandEvent& event)
{
	int sel = event.GetSelection();
	switch (sel)
	{
	case 0:
		m_stage->SetEditOp(WxStagePage::OP_ROTATE_JOINT);
		break;
	case 1:
		m_stage->SetEditOp(WxStagePage::OP_TRANSLATE_JOINT);
		break;
	case 2:
		m_stage->SetEditOp(WxStagePage::OP_IK);
		break;
	}
	m_stage->GetSubjectMgr()->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
}

//////////////////////////////////////////////////////////////////////////
// class WxToolbarPanel::WxTreeScrolled
//////////////////////////////////////////////////////////////////////////

WxToolbarPanel::WxTreeScrolled::WxTreeScrolled(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr)
	: wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxSize(300, 800))
{
	SetScrollbars(0, 1, 0, 10, 0, 0);

	auto sizer = new wxBoxSizer(wxVERTICAL);
	m_tree = new ee3::WxSkeletalTreeCtrl(this, sub_mgr);
	sizer->Add(m_tree, 1, wxEXPAND);
	SetSizer(sizer);
}

}
}