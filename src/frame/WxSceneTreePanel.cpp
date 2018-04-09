#include "frame/WxSceneTreePanel.h"
#include "frame/WxSceneTreeCtrl.h"
#include "frame/NodeFactory.h"
#include "frame/NodeType.h"

#include <ee0/MessageID.h>
#include <ee0/VariantSet.h>
#include <ee0/SubjectMgr.h>
#include <ee0/WxListSelectDlg.h>
#include <ee0/MsgHelper.h>

#include <guard/check.h>
#include <node0/SceneNode.h>
#include <node2/CompImage.h>
#include <node2/CompTransform.h>
#include <node2/CompBoundingBox.h>
#include <facade/ResPool.h>
#include <facade/Image.h>
#include <facade/Texture.h>

#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/filedlg.h>

namespace
{

static const std::vector<std::pair<uint32_t, std::string>> NODE_LIST =
{
	std::make_pair(eone::NODE_IMAGE,      "Image"),
	std::make_pair(eone::NODE_TEXT,       "Text"),
	std::make_pair(eone::NODE_MASK,       "Mask"),
	std::make_pair(eone::NODE_MESH,       "Mesh"),
	std::make_pair(eone::NODE_SCALE9,     "Scale9"),
	std::make_pair(eone::NODE_ANIM,       "Anim"),
	std::make_pair(eone::NODE_PARTICLE3D, "Particle3d"),
};

}

namespace eone
{

WxSceneTreePanel::WxSceneTreePanel(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr,
	                               const ee0::GameObj& root_obj)
	: wxPanel(parent, wxID_ANY)
	, m_sub_mgr(sub_mgr)
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
		top_sizer->Add(new WxSceneTreeCtrl(this, m_sub_mgr, root_obj), 1, wxEXPAND);
	}
	SetSizer(top_sizer);
}

void WxSceneTreePanel::OnCreatePress(wxCommandEvent& event)
{
	ee0::WxListSelectDlg dlg(this, "Create obj", 
		NODE_LIST, m_create_btn->GetScreenPosition());
	if (dlg.ShowModal() != wxID_OK) {
		return;
	}

	ee0::GameObj obj = nullptr;

	auto id = dlg.GetSelectedID();
	switch (id)
	{
	case NodeType::NODE_IMAGE:
		{
			std::string filter = "*.png;*.jpg;*.bmp;*.pvr;*.pkm";
			wxFileDialog dlg(this, wxT("Choose image"), wxEmptyString, filter);
			if (dlg.ShowModal() == wxID_OK)
			{
				auto& path = dlg.GetPath();
				auto img = facade::ResPool::Instance().Fetch<facade::Image>(path.ToStdString());

				obj = NodeFactory::Create(NODE_IMAGE);
				auto& cimage = obj->GetSharedComp<n2::CompImage>();
				cimage.SetFilepath(path.ToStdString());
				cimage.SetTexture(img->GetTexture());

				auto& cbb = obj->GetUniqueComp<n2::CompBoundingBox>();
				cbb.SetSize(*obj, sm::rect(img->GetWidth(), img->GetHeight()));
			}
		}
		break;
	case NodeType::NODE_TEXT:
		obj = NodeFactory::Create(NODE_TEXT);
		break;
	case NodeType::NODE_MASK:
		obj = NodeFactory::Create(NODE_MASK);
		break;
	case NodeType::NODE_MESH:
		obj = NodeFactory::Create(NODE_MESH);
		break;
	case NodeType::NODE_SCALE9:
		obj = NodeFactory::Create(NODE_SCALE9);
		break;
	case NodeType::NODE_ANIM:
		obj = NodeFactory::Create(NODE_ANIM);
		break;
	case NodeType::NODE_PARTICLE3D:
		obj = NodeFactory::Create(NODE_PARTICLE3D);
		break;
	default:
		return;
	}

	if (!obj) {
		return;
	}

	ee0::MsgHelper::InsertNode(*m_sub_mgr, obj, true);
}

}