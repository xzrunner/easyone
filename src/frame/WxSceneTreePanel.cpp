#include "frame/WxSceneTreePanel.h"
#include "frame/WxSceneTreeCtrl.h"
#include "frame/GameObjFactory.h"
#include "frame/GameObjType.h"

#include <ee0/MessageID.h>
#include <ee0/VariantSet.h>
#include <ee0/SubjectMgr.h>
#include <ee0/WxListSelectDlg.h>
#include <ee0/MsgHelper.h>

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
#include <facade/Texture.h>

#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/filedlg.h>

namespace
{

static const std::vector<std::pair<uint32_t, std::string>> GAME_OBJ_LIST =
{
	std::make_pair(eone::GAME_OBJ_IMAGE,      "Image"),
	std::make_pair(eone::GAME_OBJ_TEXT,       "Text"),
	std::make_pair(eone::GAME_OBJ_MASK,       "Mask"),
	std::make_pair(eone::GAME_OBJ_MESH,       "Mesh"),
	std::make_pair(eone::GAME_OBJ_SCALE9,     "Scale9"),
	std::make_pair(eone::GAME_OBJ_ANIM,       "Anim"),
	std::make_pair(eone::GAME_OBJ_PARTICLE3D, "Particle3d"),
};

}

namespace eone
{

WxSceneTreePanel::WxSceneTreePanel(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr,
	                               ECS_WORLD_PARAM const ee0::GameObj& root_obj)
	: wxPanel(parent, wxID_ANY)
	, m_sub_mgr(sub_mgr)
	ECS_WORLD_SELF_ASSIGN
{
	InitLayout(root_obj);
}

void WxSceneTreePanel::InitLayout(const ee0::GameObj& root_obj)
{
	wxSizer* top_sizer = new wxBoxSizer(wxVERTICAL);
	{
		m_create_btn = new wxButton(this, wxID_ANY, "Create Node");
		Connect(m_create_btn->GetId(), wxEVT_COMMAND_BUTTON_CLICKED,
			wxCommandEventHandler(WxSceneTreePanel::OnCreatePress));
		top_sizer->Add(m_create_btn, 0, wxALIGN_CENTER_HORIZONTAL);
	}
	{
		auto ctrl = new WxSceneTreeCtrl(this, m_sub_mgr, ECS_WORLD_SELF_VAR root_obj);
		top_sizer->Add(ctrl, 1, wxEXPAND);
	}
	SetSizer(top_sizer);
}

void WxSceneTreePanel::OnCreatePress(wxCommandEvent& event)
{
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

	auto id = dlg.GetSelectedID();
	switch (id)
	{
	case GameObjType::GAME_OBJ_IMAGE:
		{
			std::string filter = "*.png;*.jpg;*.bmp;*.pvr;*.pkm";
			wxFileDialog dlg(this, wxT("Choose image"), wxEmptyString, filter);
			if (dlg.ShowModal() == wxID_OK)
			{
				auto& path = dlg.GetPath();
				auto img = facade::ResPool::Instance().Fetch<facade::Image>(path.ToStdString());

				obj = GameObjFactory::Create(
#ifdef GAME_OBJ_ECS
					m_world,
#endif // GAME_OBJ_ECS
					GAME_OBJ_IMAGE
				);

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
		obj = GameObjFactory::Create(
#ifdef GAME_OBJ_ECS
			m_world,
#endif // GAME_OBJ_ECS
			GAME_OBJ_TEXT
		);
		break;
	case GameObjType::GAME_OBJ_MASK:
		obj = GameObjFactory::Create(
#ifdef GAME_OBJ_ECS
			m_world,
#endif // GAME_OBJ_ECS
			GAME_OBJ_MASK
		);
		break;
	case GameObjType::GAME_OBJ_MESH:
		obj = GameObjFactory::Create(
#ifdef GAME_OBJ_ECS
			m_world,
#endif // GAME_OBJ_ECS
			GAME_OBJ_MESH
		);
		break;
	case GameObjType::GAME_OBJ_SCALE9:
		obj = GameObjFactory::Create(
#ifdef GAME_OBJ_ECS
			m_world,
#endif // GAME_OBJ_ECS
			GAME_OBJ_SCALE9
		);
		break;
	case GameObjType::GAME_OBJ_ANIM:
		obj = GameObjFactory::Create(
#ifdef GAME_OBJ_ECS
			m_world,
#endif // GAME_OBJ_ECS
			GAME_OBJ_ANIM
		);
		break;
	case GameObjType::GAME_OBJ_PARTICLE3D:
		obj = GameObjFactory::Create(
#ifdef GAME_OBJ_ECS
			m_world,
#endif // GAME_OBJ_ECS
			GAME_OBJ_PARTICLE3D
		);
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

}