#include "anim/WxLayersToolbar.h"
#include "anim/config.h"
#include "anim/MessageHelper.h"

#include <node2/CompAnim.h>
#include <anim/KeyFrame.h>
#include <anim/Layer.h>

#include <wx/sizer.h>
#include <wx/button.h>

namespace eone
{
namespace anim
{

WxLayersToolbar::WxLayersToolbar(wxWindow* parent, n2::CompAnim& canim,
	                             const ee0::SubjectMgrPtr& tl_sub_mgr)
	: wxPanel(parent)
	, m_canim(canim)
	, m_tl_sub_mgr(tl_sub_mgr)
{
	SetBackgroundColour(MEDIUM_GRAY);

	InitLayout();
}

void WxLayersToolbar::InitLayout()
{
	wxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);

	wxButton* btn_add = new wxButton(this, wxID_ANY, "+", wxDefaultPosition, wxSize(25, 25));
	Connect(btn_add->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, 
		wxCommandEventHandler(WxLayersToolbar::OnAddLayer));
	sizer->Add(btn_add, 0, wxLEFT | wxRIGHT, 5);

	wxButton* btn_del = new wxButton(this, wxID_ANY, "-", wxDefaultPosition, wxSize(25, 25));
	Connect(btn_del->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, 
		wxCommandEventHandler(WxLayersToolbar::OnDelLayer));
	sizer->Add(btn_del, 0, wxLEFT | wxRIGHT, 5);

	SetSizer(sizer);
}

void WxLayersToolbar::OnAddLayer(wxCommandEvent& event)
{
	int count = m_canim.GetAllLayers().size();

	auto layer = std::make_unique<::anim::Layer>();
	layer->SetName("Layer" + std::to_string(count));

	layer->AddKeyFrame(std::make_unique<::anim::KeyFrame>(0));

	m_canim.AddLayer(layer);

	MessageHelper::SetCurrFrame(*m_tl_sub_mgr, count, 0);
}

void WxLayersToolbar::OnDelLayer(wxCommandEvent& event)
{

}

}
}