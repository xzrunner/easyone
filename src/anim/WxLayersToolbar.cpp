#include "anim/WxLayersToolbar.h"
#include "anim/config.h"
#include "anim/MessageHelper.h"
#include "anim/MessageID.h"

#include <ee0/SubjectMgr.h>

#include <node2/CompAnim.h>
#include <node2/CompAnimInst.h>
#include <anim/KeyFrame.h>
#include <anim/Layer.h>

#include <wx/sizer.h>
#include <wx/button.h>

namespace eone
{
namespace anim
{

WxLayersToolbar::WxLayersToolbar(wxWindow* parent, n2::CompAnim& canim,
	                             n2::CompAnimInst& canim_inst,
	                             const ee0::SubjectMgrPtr& sub_mgr)
	: wxPanel(parent)
	, m_canim(canim)
	, m_canim_inst(canim_inst)
	, m_sub_mgr(sub_mgr)
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

	sizer->AddSpacer(20);

	m_btn_play = new wxButton(this, wxID_ANY, ">", wxDefaultPosition, wxSize(25, 25));
	Connect(m_btn_play->GetId(), wxEVT_COMMAND_BUTTON_CLICKED,
		wxCommandEventHandler(WxLayersToolbar::OnPressPlay));
	sizer->Add(m_btn_play, 0, wxLEFT | wxRIGHT, 5);

	SetSizer(sizer);
}

void WxLayersToolbar::OnAddLayer(wxCommandEvent& event)
{
	int count = m_canim.GetAllLayers().size();

	auto layer = std::make_unique<::anim::Layer>();
	layer->SetName("Layer" + std::to_string(count));

	layer->AddKeyFrame(std::make_unique<::anim::KeyFrame>(0));

	m_canim.AddLayer(layer);
	m_sub_mgr->NotifyObservers(MSG_REFRESH_COMP_INST);

	MessageHelper::SetCurrFrame(*m_sub_mgr, count, 0);
}

void WxLayersToolbar::OnDelLayer(wxCommandEvent& event)
{

}

void WxLayersToolbar::OnPressPlay(wxCommandEvent& event)
{
	auto& ctrl = m_canim_inst.GetPlayCtrl();
	ctrl.SetActive(!ctrl.IsActive());
	m_btn_play->SetLabelText(ctrl.IsActive() ? "||" : ">");
}

}
}