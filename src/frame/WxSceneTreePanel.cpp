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

static const std::vector<std::pair<uint32_t, std::string>> NODE_LIST =
{
	std::make_pair(eone::NODE_IMAGE,   "Image"),
	std::make_pair(eone::NODE_TEXT,    "Text"),
	std::make_pair(eone::NODE_MASK,    "Mask"),
	std::make_pair(eone::NODE_MESH,    "Mesh"),
	std::make_pair(eone::NODE_SCALE9,  "Scale9"),

	std::make_pair(eone::NODE_SPRITE2, "Sprite2"),
};

}

namespace eone
{

WxSceneTreePanel::WxSceneTreePanel(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr,
	                               const n0::SceneNodePtr& root_node)
	: wxPanel(parent, wxID_ANY)
	, m_sub_mgr(sub_mgr)
{
	InitLayout(root_node);
}

void WxSceneTreePanel::InitLayout(const n0::SceneNodePtr& root_node)
{
	wxSizer* top_sizer = new wxBoxSizer(wxVERTICAL);
	{
		m_create_btn = new wxButton(this, wxID_ANY, "Create Node");
		Connect(m_create_btn->GetId(), wxEVT_COMMAND_BUTTON_CLICKED,
			wxCommandEventHandler(WxSceneTreePanel::OnCreatePress));
		top_sizer->Add(m_create_btn, 0, wxALIGN_CENTER_HORIZONTAL);
	}
	{
		top_sizer->Add(new WxSceneTreeCtrl(this, m_sub_mgr, root_node), 1, wxEXPAND);
	}
	SetSizer(top_sizer);
}

void WxSceneTreePanel::OnCreatePress(wxCommandEvent& event)
{
	ee0::WxListSelectDlg dlg(this, "Create node", 
		NODE_LIST, m_create_btn->GetScreenPosition());
	if (dlg.ShowModal() != wxID_OK) {
		return;
	}

	n0::SceneNodePtr node = nullptr;

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
				auto img = gum::ResPool::Instance().Fetch<gum::Image>(path.ToStdString());

				node = NodeFactory::Create(NODE_IMAGE);
				auto& cimage = node->GetSharedComp<n2::CompImage>();
				cimage.SetFilepath(path.ToStdString());
				cimage.SetTexture(img->GetTexture());

				auto& cbb = node->GetUniqueComp<n2::CompBoundingBox>();
				cbb.SetSize(*node, sm::rect(img->GetWidth(), img->GetHeight()));
			}
		}
		break;
	case NodeType::NODE_TEXT:
		node = NodeFactory::Create(NODE_TEXT);
		break;
	case NodeType::NODE_MASK:
		node = NodeFactory::Create(NODE_MASK);
		break;
	case NodeType::NODE_MESH:
		node = NodeFactory::Create(NODE_MESH);
		break;
	case NodeType::NODE_SCALE9:
		node = NodeFactory::Create(NODE_SCALE9);
		break;

	case NodeType::NODE_SPRITE2:
		{
			wxFileDialog dlg(this, wxT("Choose sprite2"), wxEmptyString, "*.json");
			if (dlg.ShowModal() == wxID_OK)
			{
				auto& path = dlg.GetPath();
				auto sym = gum::SymbolPool::Instance()->Fetch(path.ToStdString().c_str());

				node = NodeFactory::Create(NODE_SPRITE2);
				auto& csprite2 = node->GetSharedComp<n2::CompSprite2>();
				csprite2.SetFilepath(path.ToStdString());
				csprite2.SetSymbol(sym);

				auto& cbb = node->GetUniqueComp<n2::CompBoundingBox>();
				cbb.SetSize(*node, sym->GetBounding());
			}
		}
		break;
	default:
		return;
	}

	if (!node) {
		return;
	}

	ee0::MsgHelper::InsertNode(*m_sub_mgr, node, true);
}

}