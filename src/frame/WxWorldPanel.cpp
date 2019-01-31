#include "frame/WxWorldPanel.h"
#include "frame/WxSceneTreeCtrl.h"
#include "frame/WxItemsListCtrl.h"
#include "frame/GameObjFactory.h"
#include "frame/GameObjType.h"

#include <ee0/WxListSelectDlg.h>
#include <ee0/MsgHelper.h>
#include <ee0/SubjectMgr.h>

#include <guard/check.h>
#ifndef GAME_OBJ_ECS
#include <node0/SceneNode.h>
#include <node2/CompImage.h>
#include <node2/CompTransform.h>
#include <node2/CompBoundingBox.h>
#else
#include <entity0/World.h>
#include <entity2/CompImage.h>
#include <entity2/CompBoundingBox.h>
#endif // GAME_OBJ_ECS
#include <facade/ResPool.h>
#include <facade/Image.h>

#include <wx/sizer.h>
#include <wx/notebook.h>
#include <wx/button.h>
#include <wx/srchctrl.h>
#include <wx/filedlg.h>

namespace eone
{

WxWorldPanel::WxWorldPanel(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr,
	                       ECS_WORLD_PARAM const ee0::GameObj& root_obj)
	: wxPanel(parent, wxID_ANY)
	, m_sub_mgr(sub_mgr)
{
	InitLayout(sub_mgr, ECS_WORLD_VAR root_obj);
}

void WxWorldPanel::InitLayout(const ee0::SubjectMgrPtr& sub_mgr,
	                          ECS_WORLD_PARAM const ee0::GameObj& root_obj)
{
	wxSizer* top_sizer = new wxBoxSizer(wxVERTICAL);

	// header
	{
		wxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);

		m_create_btn = new wxButton(this, wxID_ANY, "+", wxDefaultPosition, wxSize(20, 20));
		Connect(m_create_btn->GetId(), wxEVT_COMMAND_BUTTON_CLICKED,
			wxCommandEventHandler(WxWorldPanel::OnAddPress));
		sizer->Add(m_create_btn, 0, wxLEFT | wxRIGHT, 5);

		m_search_ctrl = new wxSearchCtrl(this, wxID_ANY, "",
			wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
		Connect(m_search_ctrl->GetId(), wxEVT_SEARCHCTRL_SEARCH_BTN,
			wxCommandEventHandler(WxWorldPanel::OnSearch));
		Connect(m_search_ctrl->GetId(), wxEVT_TEXT_ENTER,
			wxCommandEventHandler(WxWorldPanel::OnSearch));
		sizer->Add(m_search_ctrl);

		top_sizer->Add(sizer);
	}

	m_notebook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_BOTTOM);
	//Connect(m_notebook->GetId(), wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED,
	//	wxBookCtrlEventHandler(WxWorldPanel::OnPageChanged));
	//Connect(m_notebook->GetId(), wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING,
	//	wxBookCtrlEventHandler(WxWorldPanel::OnPageChanging));

	top_sizer->Add(m_notebook, 1, wxEXPAND);

	m_notebook->AddPage(m_tree_page = new WxSceneTreeCtrl(
		m_notebook, sub_mgr, ECS_WORLD_VAR root_obj), "Tree");
	m_notebook->AddPage(m_list_page = new WxItemsListCtrl(
		m_notebook, sub_mgr), "List");

	SetSizer(top_sizer);
}

void WxWorldPanel::OnAddPress(wxCommandEvent& event)
{
	struct ObjItemData : public wxTreeItemData
	{
		ObjItemData(int type)
			: type(type) {}

		int type;

	}; // ObjItemData

	const std::vector<std::pair<std::string, wxTreeItemData*>> GAME_OBJ_LIST =
	{
		{ "Image",      new ObjItemData(eone::GAME_OBJ_IMAGE) },
		{ "Text",       new ObjItemData(eone::GAME_OBJ_TEXT) },
		{ "Mask",       new ObjItemData(eone::GAME_OBJ_MASK) },
		{ "Mesh",       new ObjItemData(eone::GAME_OBJ_MESH) },
		{ "Scale9",     new ObjItemData(eone::GAME_OBJ_SCALE9) },
		{ "Anim",       new ObjItemData(eone::GAME_OBJ_ANIM) },
		{ "Particle3d", new ObjItemData(eone::GAME_OBJ_PARTICLE3D) },
        { "Light",      new ObjItemData(eone::GAME_OBJ_LIGHT) },
	};

	ee0::WxListSelectDlg dlg(this, "Create obj",
		GAME_OBJ_LIST, m_create_btn->GetScreenPosition());
	if (dlg.ShowModal() != wxID_OK) {
		return;
	}

#ifndef GAME_OBJ_ECS
	ee0::GameObj obj = nullptr;
#else
	ee0::GameObj obj;
#endif // GAME_OBJ_ECS

	auto type = static_cast<ObjItemData*>(dlg.GetSelected())->type;
	switch (type)
	{
	case GameObjType::GAME_OBJ_IMAGE:
		{
			std::string filter = "*.png;*.jpg;*.bmp;*.pvr;*.pkm";
			wxFileDialog dlg(this, wxT("Choose image"), wxEmptyString, filter);
			if (dlg.ShowModal() == wxID_OK)
			{
				auto& path = dlg.GetPath();
				auto img = facade::ResPool::Instance().Fetch<facade::Image>(path.ToStdString());

				obj = GameObjFactory::Create(ECS_WORLD_SELF_VAR GAME_OBJ_IMAGE);

#ifndef GAME_OBJ_ECS
				auto& cimage = obj->GetSharedComp<n2::CompImage>();
				cimage.SetTexture(img->GetTexture());

				auto& cbb = obj->GetUniqueComp<n2::CompBoundingBox>();
				cbb.SetSize(*obj, sm::rect(img->GetWidth(), img->GetHeight()));
#else
				auto& cimage = m_world.GetComponent<e2::CompImage>(obj);
				cimage.tex = img->GetTexture();

				auto& cbb = m_world.GetComponent<e2::CompBoundingBox>(obj);
				cbb.rect = sm::rect(img->GetWidth(), img->GetHeight());
#endif // GAME_OBJ_ECS
			}
		}
		break;
	case GameObjType::GAME_OBJ_TEXT:
		obj = GameObjFactory::Create(ECS_WORLD_SELF_VAR GAME_OBJ_TEXT);
		break;
	case GameObjType::GAME_OBJ_MASK:
		obj = GameObjFactory::Create(ECS_WORLD_SELF_VAR GAME_OBJ_MASK);
		break;
	case GameObjType::GAME_OBJ_MESH:
		obj = GameObjFactory::Create(ECS_WORLD_SELF_VAR GAME_OBJ_MESH);
		break;
	case GameObjType::GAME_OBJ_SCALE9:
		obj = GameObjFactory::Create(ECS_WORLD_SELF_VAR GAME_OBJ_SCALE9);
		break;
	case GameObjType::GAME_OBJ_ANIM:
		obj = GameObjFactory::Create(ECS_WORLD_SELF_VAR GAME_OBJ_ANIM);
		break;
	case GameObjType::GAME_OBJ_PARTICLE3D:
		obj = GameObjFactory::Create(ECS_WORLD_SELF_VAR GAME_OBJ_PARTICLE3D);
		break;
    case GameObjType::GAME_OBJ_LIGHT:
        obj = GameObjFactory::Create(ECS_WORLD_SELF_VAR GAME_OBJ_LIGHT);
        break;
	default:
		return;
	}

	if (!GAME_OBJ_VALID(obj)) {
		return;
	}

	ee0::MsgHelper::InsertNode(*m_sub_mgr, obj, true);
	m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
}

void WxWorldPanel::OnSearch(wxCommandEvent& event)
{
	auto str = event.GetString().ToStdString();
	m_tree_page->OnSearch(str);
	m_list_page->OnSearch(str);
}

}