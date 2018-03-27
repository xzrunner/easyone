#include "anim/WxLayersScrolled.h"
#include "anim/MessageID.h"
#include "anim/WxLayersPanel.h"

#include <guard/check.h>
#include <ee0/SubjectMgr.h>

#include <wx/sizer.h>

namespace eone
{
namespace anim
{

WxLayersScrolled::WxLayersScrolled(wxWindow* parent, const n2::CompAnim& canim,
	                               const ee0::SubjectMgrPtr& tl_sub_mgr)
	: wxScrolledWindow(parent)
{
	ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_NEVER);

	SetScrollbars(0, 1, 0, 10, 0, 0);
	InitLayout(canim, tl_sub_mgr);
		
	tl_sub_mgr->RegisterObserver(MSG_WND_SCROLL, this);
}

void WxLayersScrolled::OnNotify(uint32_t msg, const ee0::VariantSet& variants)
{
	switch (msg)
	{
	case MSG_WND_SCROLL:
		{
			auto var = variants.GetVariant("y");
			GD_ASSERT(var.m_type == ee0::VT_LONG, "err val");
			Scroll(0, var.m_val.l);
		}
		break;
	}
}

void WxLayersScrolled::InitLayout(const n2::CompAnim& canim, const ee0::SubjectMgrPtr& tl_sub_mgr)
{
	auto sizer = new wxBoxSizer(wxVERTICAL);
	auto layers = new WxLayersPanel(this, canim, tl_sub_mgr);
	sizer->Add(layers, 1, wxEXPAND);
	SetSizer(sizer);
}

}
}