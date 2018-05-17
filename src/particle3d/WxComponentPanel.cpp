#include "particle3d/WxComponentPanel.h"
#include "particle3d/Languages.h"
#include "particle3d/config.h"

#include <ee0/WxSliderTwoCtrl.h>
#include <ee0/SliderItem.h>
#include <ee0/WxImagePanel.h>
#include <ee0/VariantSet.h>

#include <guard/check.h>
#include <sm_const.h>
#include <ps_3d.h>
#include <sx/ResFileHelper.h>

#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/button.h>
#include <wx/spinctrl.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/colordlg.h>

#include <boost/filesystem.hpp>

namespace
{

const float SCALE_START = 100;
const float SCALE_END = 100;
const float ROTATE_MIN = 0;
const float ROTATE_MAX = 0;

}

namespace eone
{
namespace particle3d
{

WxComponentPanel::WxComponentPanel(wxWindow* parent, p3d_symbol* sym, 
	                               const std::string& filepath,
	                               WxEmitterPanel::WxChildrenPanel* children_panel)
	: wxPanel(parent)
	, m_sym(sym)
	, m_filepath(filepath)
	, m_children_panel(children_panel)
{
	InitLayout();
	for (auto& slider : m_sliders) {
		slider->StoreToCB();
	}
	UpdateBtnColor();
}

void WxComponentPanel::SetValue(int key, const ee0::VariantSet& variants)
{
	switch (key)
	{
	case PS_SCALE:
		{
			auto var0 = variants.GetVariant("var0");
			GD_ASSERT(var0.m_type == ee0::VT_FLOAT, "err var");
			m_sym->scale_start = var0.m_val.flt * 0.01f;

			auto var1 = variants.GetVariant("var1");
			GD_ASSERT(var1.m_type == ee0::VT_FLOAT, "err var");
			m_sym->scale_end = var1.m_val.flt * 0.01f;
		}
		break;
	case PS_ROTATE:
		{
			auto var0 = variants.GetVariant("var0");
			auto var1 = variants.GetVariant("var1");
			GD_ASSERT(var0.m_type == ee0::VT_FLOAT && var1.m_type == ee0::VT_FLOAT, "err var");
			m_sym->angle     = (var0.m_val.flt + var1.m_val.flt) * 0.5f * SM_DEG_TO_RAD;
			m_sym->angle_var = (var1.m_val.flt - var0.m_val.flt) * 0.5f * SM_DEG_TO_RAD;
		}
		break;
	case PS_ALPHA:
		{
			auto var0 = variants.GetVariant("var0");
			GD_ASSERT(var0.m_type == ee0::VT_FLOAT, "err var");
			m_sym->mul_col_begin.a = var0.m_val.flt;

			auto var1 = variants.GetVariant("var1");
			GD_ASSERT(var1.m_type == ee0::VT_FLOAT, "err var");
			m_sym->mul_col_end.a = var1.m_val.flt;
		}
		break;
	}
}

void WxComponentPanel::GetValue(int key, ee0::VariantSet& variants) const
{
	switch (key)
	{
	case PS_SCALE:
		{
			ee0::Variant var0;
			var0.m_type = ee0::VT_FLOAT;
			var0.m_val.flt = m_sym->scale_start * 100;
			variants.SetVariant("var0", var0);

			ee0::Variant var1;
			var1.m_type = ee0::VT_FLOAT;
			var1.m_val.flt = m_sym->scale_end * 100;
			variants.SetVariant("var1", var1);
		}
		break;
	case PS_ROTATE:
		{
			ee0::Variant var0, var1;
			var0.m_type = ee0::VT_FLOAT;
			var1.m_type = ee0::VT_FLOAT;
			var0.m_val.flt = (m_sym->angle + m_sym->angle_var) * SM_RAD_TO_DEG;
			var1.m_val.flt = (m_sym->angle - m_sym->angle_var) * SM_RAD_TO_DEG;
		}
		break;
	case PS_ALPHA:
		{
			ee0::Variant var0;
			var0.m_type = ee0::VT_FLOAT;
			var0.m_val.flt = m_sym->mul_col_begin.a;
			variants.SetVariant("var0", var0);

			ee0::Variant var1;
			var1.m_type = ee0::VT_FLOAT;
			var1.m_val.flt = m_sym->mul_col_end.a;
			variants.SetVariant("var1", var1);
		}
		break;
	}
}

void WxComponentPanel::UpdateBtnColor()
{
	m_begin_mul_col_btn->SetBackgroundColour(
		wxColour(m_sym->mul_col_begin.r, m_sym->mul_col_begin.g, m_sym->mul_col_begin.b));
	m_end_mul_col_btn->SetBackgroundColour(
		wxColour(m_sym->mul_col_end.r, m_sym->mul_col_end.g, m_sym->mul_col_end.b));
	m_begin_add_col_btn->SetBackgroundColour(
		wxColour(m_sym->add_col_begin.r, m_sym->add_col_begin.g, m_sym->add_col_begin.b));
	m_end_add_col_btn->SetBackgroundColour(
		wxColour(m_sym->add_col_end.r, m_sym->add_col_end.g, m_sym->add_col_end.b));
}

void WxComponentPanel::SetCount(int count)
{
	m_sym->count = count;
	m_count->SetValue(count);
}

void WxComponentPanel::InitLayout()
{
	wxSizer* top_sizer = new wxBoxSizer(wxVERTICAL);

	auto name = boost::filesystem::path(m_filepath).stem().string();
	wxStaticBox* bounding = new wxStaticBox(this, wxID_ANY, name); 
	wxSizer* sizer = new wxStaticBoxSizer(bounding, wxVERTICAL);
	InitLayout(sizer);
	top_sizer->Add(sizer);

	top_sizer->AddSpacer(20);

	SetSizer(top_sizer);
}

void WxComponentPanel::InitLayout(wxSizer* top_sizer)
{
	// top
	{
		wxSizer* hori_sizer = new wxBoxSizer(wxHORIZONTAL);
		// Left
		{
			wxSizer* vert_sizer = new wxBoxSizer(wxVERTICAL);
			// Del
			{
//				wxButton* btn = new wxButton(this, wxID_ANY, LANG[LK_REMOVE], wxDefaultPosition, wxSize(50, 50));
				wxButton* btn = new wxButton(this, wxID_ANY, LANG[LK_REMOVE]);
				Connect(btn->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(WxComponentPanel::OnDelete));
				vert_sizer->Add(btn);
			}
			vert_sizer->AddSpacer(8);
			// Name
			{
				wxSizer* sz = new wxBoxSizer(wxHORIZONTAL);
				sz->Add(new wxStaticText(this, wxID_ANY, LANG[LK_NAME]));
				sz->Add(m_name = new wxTextCtrl(this, wxID_ANY));
				vert_sizer->Add(sz);
			}
			vert_sizer->AddSpacer(8);
			// Count
			{
				wxSizer* sz = new wxBoxSizer(wxHORIZONTAL);
				sz->Add(new wxStaticText(this, wxID_ANY, LANG[LK_CONST_COUNT]));

				m_count = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(50, -1), wxSP_ARROW_KEYS, 0, 100, 0);
				Connect(m_count->GetId(), wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(WxComponentPanel::OnSetCount));
				sz->Add(m_count);

				vert_sizer->Add(sz);
			}
			hori_sizer->Add(vert_sizer);
		}
		hori_sizer->AddSpacer(20);
		// Right Icon
		{
			auto type = sx::ResFileHelper::Type(m_filepath);
			switch (type)
			{
			case sx::RES_FILE_IMAGE:
				hori_sizer->Add(new ee0::WxImagePanel(this, m_filepath, 100));
				break;
			case sx::RES_FILE_JSON:
				break;
			}
		}	
		top_sizer->Add(hori_sizer);
	}
	// Scale
	ee0::WxSliderTwoCtrl* s_scale = new ee0::WxSliderTwoCtrl(this, LANG[LK_SCALE], "scale", *this, PS_SCALE, 
		ee0::SliderItem(LANG[LK_START], "start", SCALE_START, 0, 2000), ee0::SliderItem(LANG[LK_END], "end", SCALE_END, 0, 2000));
	top_sizer->Add(s_scale);
	top_sizer->AddSpacer(10);
	m_sliders.push_back(s_scale);
	// Rotate
	ee0::WxSliderTwoCtrl* s_rotate = new ee0::WxSliderTwoCtrl(this, LANG[LK_ROTATE], "rotate", *this, PS_ROTATE, 
		ee0::SliderItem(LANG[LK_MIN], "min", ROTATE_MIN, -180, 180), ee0::SliderItem(LANG[LK_MAX], "max", ROTATE_MAX, -180, 180));
	top_sizer->Add(s_rotate);
	top_sizer->AddSpacer(10);
	m_sliders.push_back(s_rotate);
	// Mul Color
	{
		wxStaticBox* bounding = new wxStaticBox(this, wxID_ANY, "��ɫ"); 
		wxSizer* hori_sizer = new wxStaticBoxSizer(bounding, wxHORIZONTAL);

		hori_sizer->Add(new wxStaticText(this, wxID_ANY, "��ʼ"));
		hori_sizer->AddSpacer(5);
		m_begin_mul_col_btn = new wxButton(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, 20));
		Connect(m_begin_mul_col_btn->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(WxComponentPanel::OnSetBeginMulCol));
		hori_sizer->Add(m_begin_mul_col_btn);

		hori_sizer->AddSpacer(20);

		hori_sizer->Add(new wxStaticText(this, wxID_ANY, "����"));
		hori_sizer->AddSpacer(5);
		m_end_mul_col_btn = new wxButton(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, 20));
		Connect(m_end_mul_col_btn->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(WxComponentPanel::OnSetEndMulCol));
		hori_sizer->Add(m_end_mul_col_btn);

		top_sizer->Add(hori_sizer);
	}
	// Add Color
	{
		wxStaticBox* bounding = new wxStaticBox(this, wxID_ANY, "��ɫ"); 
		wxSizer* hori_sizer = new wxStaticBoxSizer(bounding, wxHORIZONTAL);

		hori_sizer->Add(new wxStaticText(this, wxID_ANY, "��ʼ"));
		hori_sizer->AddSpacer(5);
		m_begin_add_col_btn = new wxButton(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, 20));
		Connect(m_begin_add_col_btn->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(WxComponentPanel::OnSetBeginAddCol));
		hori_sizer->Add(m_begin_add_col_btn);

		hori_sizer->AddSpacer(20);

		hori_sizer->Add(new wxStaticText(this, wxID_ANY, "����"));
		hori_sizer->AddSpacer(5);
		m_end_add_col_btn = new wxButton(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, 20));
		Connect(m_end_add_col_btn->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(WxComponentPanel::OnSetEndAddCol));
		hori_sizer->Add(m_end_add_col_btn);

		top_sizer->Add(hori_sizer);
	}
	// Alpha
	ee0::WxSliderTwoCtrl* s_alpha = new ee0::WxSliderTwoCtrl(this, LANG[LK_ALPHA], "alpha2", *this, PS_ALPHA, 
	ee0::SliderItem(LANG[LK_START], "start", 255, 0, 255), ee0::SliderItem(LANG[LK_END], "end", 255, 0, 255));
	top_sizer->Add(s_alpha);
	top_sizer->AddSpacer(10);
	m_sliders.push_back(s_alpha);
}

void WxComponentPanel::OnDelete(wxCommandEvent& event)
{
	m_children_panel->RemoveChild(this);
}

void WxComponentPanel::OnSetCount(wxSpinEvent& event)
{
	m_sym->count = event.GetValue();
}

void WxComponentPanel::OnSetBeginMulCol(wxCommandEvent& event)
{
	ChangeColor(m_sym->mul_col_begin.rgba);
}

void WxComponentPanel::OnSetEndMulCol(wxCommandEvent& event)
{
	ChangeColor(m_sym->mul_col_end.rgba);
}

void WxComponentPanel::OnSetBeginAddCol(wxCommandEvent& event)
{
	ChangeColor(m_sym->add_col_begin.rgba);
}

void WxComponentPanel::OnSetEndAddCol(wxCommandEvent& event)
{
	ChangeColor(m_sym->add_col_end.rgba);
}

void WxComponentPanel::ChangeColor(uint8_t rgba[4])
{
	wxColourData data;
	data.SetColour(wxColour(rgba[0], rgba[1], rgba[2]));
	wxColourDialog dlg(m_parent, &data);

	dlg.SetTitle(wxT("Set Color"));

	wxPoint pos = wxGetMousePosition();
	pos.x -= 400;
	dlg.SetPosition(pos);

	if (dlg.ShowModal() == wxID_OK)
	{
		const wxColor& col = dlg.GetColourData().GetColour();
		rgba[0] = col.Red();
		rgba[1] = col.Green();
		rgba[2] = col.Blue();
		UpdateBtnColor();
	}
}

}
}