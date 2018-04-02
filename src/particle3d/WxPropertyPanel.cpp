#include "particle3d/WxPropertyPanel.h"
#include "particle3d/Languages.h"
#include "particle3d/config.h"
#include "particle3d/WxComponentPanel.h"

#include <ee0/WxSliderOneCtrl.h>
#include <ee0/WxSliderTwoCtrl.h>
#include <ee0/SliderItem.h>
#include <ee0/VariantSet.h>

#include <guard/check.h>
#include <ps_3d.h>
#include <sm_const.h>
#include <emitter/P3dTemplate.h>

#include <wx/sizer.h>
#include <wx/checkbox.h>
#include <wx/choice.h>
#include <wx/stattext.h>
#include <wx/statbox.h>
#include <wx/spinctrl.h>
#include <wx/button.h>

namespace
{

const float CAPACITY                  = 100;
const float COUNT                     = 20;
const float EMISSION_TIME             = 150;
const float LIFE_CENTER               = 800;
const float LIFE_OFFSET               = 500;
const float MIN_HORI                  = 0;
const float MAX_HORI                  = 360;
const float MIN_VERT                  = 60;
const float MAX_VERT                  = 90;
const float RADIAL_SPEED_CENTER	      = 1600;
const float RADIAL_SPEED_OFFSET	      = 400;
const float ANGULAR_SPEED_CENTER      = 0;
const float ANGULAR_SPEED_OFFSET      = 0;
const float DISTURBANCE_RADIUS_CENTER = 0;
const float DISTURBANCE_RADIUS_OFFSET = 0;
const float DISTURBANCE_SPD_CENTER    = 0;
const float DISTURBANCE_SPD_OFFSET    = 0;
const float GRAVITY                   = 1200;
const float LINEAR_ACC_CENTER         = 0;
const float LINEAR_ACC_OFFSET         = 0;
const float INERTIA                   = 4;
const float FADEOUT_TIME              = 300;
const float START_RADIUS              = 0;
const float START_HEIGHT              = 0;

const char* ITEM_ATTR_CENTER = "center";
const char* ITEM_ATTR_OFFSET = "offset";

const char* ITEM_ATTR_RADIUS = "radius";
const char* ITEM_ATTR_HEIGHT = "height";

}

namespace eone
{
namespace particle3d
{

WxPropertyPanel::WxPropertyPanel(wxWindow* parent, et::P3dTemplate& p3d)
	: wxPanel(parent)
	, m_p3d(p3d)
{
	InitLayout();
}

void WxPropertyPanel::SetValue(int key, const ee0::VariantSet& variants)
{
	auto cfg = m_p3d.GetCfg();
	switch (key)
	{
	case PS_COUNT:
		{
			auto var = variants.GetVariant("var");
			GD_ASSERT(var.m_type == ee0::VT_FLOAT, "err var");
			cfg->count = static_cast<int>(var.m_val.flt);
		}
		break;
	case PS_EMISSION_TIME:
		{
			auto var = variants.GetVariant("var");
			GD_ASSERT(var.m_type == ee0::VT_FLOAT, "err var");
			cfg->emission_time = var.m_val.flt * 0.001f;
		}
		break;
	case PS_LIFE_TIME:
		{
			auto var0 = variants.GetVariant("var0");
			GD_ASSERT(var0.m_type == ee0::VT_FLOAT, "err var");
			cfg->life = var0.m_val.flt * 0.001f;

			auto var1 = variants.GetVariant("var1");
			GD_ASSERT(var1.m_type == ee0::VT_FLOAT, "err var");
			cfg->life_var = var1.m_val.flt * 0.001f;
		}
		break;
	case PS_RADIAL_SPEED:
		{
			auto var0 = variants.GetVariant("var0");
			GD_ASSERT(var0.m_type == ee0::VT_FLOAT, "err var");
			cfg->radial_spd = var0.m_val.flt;

			auto var1 = variants.GetVariant("var1");
			GD_ASSERT(var1.m_type == ee0::VT_FLOAT, "err var");
			cfg->radial_spd_var = var1.m_val.flt;
		}
		break;
	case PS_TANGENTIAL_SPEED:
		{
			auto var0 = variants.GetVariant("var0");
			GD_ASSERT(var0.m_type == ee0::VT_FLOAT, "err var");
			cfg->tangential_spd = var0.m_val.flt;

			auto var1 = variants.GetVariant("var1");
			GD_ASSERT(var1.m_type == ee0::VT_FLOAT, "err var");
			cfg->tangential_spd_var = var1.m_val.flt;
		}
		break;
	case PS_ANGULAR_SPEED:
		{
			auto var0 = variants.GetVariant("var0");
			GD_ASSERT(var0.m_type == ee0::VT_FLOAT, "err var");
			cfg->angular_spd = var0.m_val.flt * SM_DEG_TO_RAD;

			auto var1 = variants.GetVariant("var1");
			GD_ASSERT(var1.m_type == ee0::VT_FLOAT, "err var");
			cfg->angular_spd_var = var1.m_val.flt * SM_DEG_TO_RAD;
		}
		break;
	case PS_DISTURBANCE_RADIUS:
		{
			auto var0 = variants.GetVariant("var0");
			GD_ASSERT(var0.m_type == ee0::VT_FLOAT, "err var");
			cfg->dis_region = var0.m_val.flt;

			auto var1 = variants.GetVariant("var1");
			GD_ASSERT(var1.m_type == ee0::VT_FLOAT, "err var");
			cfg->dis_region_var = var1.m_val.flt;
		}
		break;
	case PS_DISTURBANCE_SPD:
		{
			auto var0 = variants.GetVariant("var0");
			GD_ASSERT(var0.m_type == ee0::VT_FLOAT, "err var");
			cfg->dis_spd = var0.m_val.flt;

			auto var1 = variants.GetVariant("var1");
			GD_ASSERT(var1.m_type == ee0::VT_FLOAT, "err var");
			cfg->dis_spd_var = var1.m_val.flt;
		}
		break;
	case PS_GRAVITY:
		{
			auto var = variants.GetVariant("var");
			GD_ASSERT(var.m_type == ee0::VT_FLOAT, "err var");
			cfg->gravity = var.m_val.flt;
		}
		break;
	case PS_LINEAR_ACC:
		{
			auto var0 = variants.GetVariant("var0");
			GD_ASSERT(var0.m_type == ee0::VT_FLOAT, "err var");
			cfg->linear_acc = var0.m_val.flt;

			auto var1 = variants.GetVariant("var1");
			GD_ASSERT(var1.m_type == ee0::VT_FLOAT, "err var");
			cfg->linear_acc_var = var1.m_val.flt;
		}
		break;
	case PS_FADEOUT_TIME:
		{
			auto var = variants.GetVariant("var");
			GD_ASSERT(var.m_type == ee0::VT_FLOAT, "err var");
			cfg->fadeout_time = var.m_val.flt * 0.001f;
		}
		break;
	case PS_START_POS:
		{
			auto var0 = variants.GetVariant("var0");
			GD_ASSERT(var0.m_type == ee0::VT_FLOAT, "err var");
			cfg->start_radius = var0.m_val.flt;

			auto var1 = variants.GetVariant("var1");
			GD_ASSERT(var1.m_type == ee0::VT_FLOAT, "err var");
			cfg->start_height = var1.m_val.flt;
		}
		break;
	}
}

void WxPropertyPanel::GetValue(int key, ee0::VariantSet& variants) const
{
	auto cfg = m_p3d.GetCfg();
	switch (key)
	{
	case PS_COUNT:
		{
			ee0::Variant var;
			var.m_type = ee0::VT_FLOAT;
			var.m_val.flt = static_cast<float>(cfg->count);
			variants.SetVariant("var", var);
		}
		break;
	case PS_EMISSION_TIME:
		{
			ee0::Variant var;
			var.m_type = ee0::VT_FLOAT;
			var.m_val.flt = cfg->emission_time * 1000;
			variants.SetVariant("var", var);
		}
		break;
	case PS_LIFE_TIME:
		{
			ee0::Variant var0;
			var0.m_type = ee0::VT_FLOAT;
			var0.m_val.flt = cfg->life * 1000;
			variants.SetVariant("var0", var0);

			ee0::Variant var1;
			var1.m_type = ee0::VT_FLOAT;
			var1.m_val.flt = cfg->life_var * 1000;
			variants.SetVariant("var1", var1);
		}
		break;
	case PS_RADIAL_SPEED:
		{
			ee0::Variant var0;
			var0.m_type = ee0::VT_FLOAT;
			var0.m_val.flt = cfg->radial_spd;
			variants.SetVariant("var0", var0);

			ee0::Variant var1;
			var1.m_type = ee0::VT_FLOAT;
			var1.m_val.flt = cfg->radial_spd_var;
			variants.SetVariant("var1", var1);
		}
		break;
	case PS_TANGENTIAL_SPEED:
		{
			ee0::Variant var0;
			var0.m_type = ee0::VT_FLOAT;
			var0.m_val.flt = cfg->tangential_spd;
			variants.SetVariant("var0", var0);

			ee0::Variant var1;
			var1.m_type = ee0::VT_FLOAT;
			var1.m_val.flt = cfg->tangential_spd_var;
			variants.SetVariant("var1", var1);
		}
		break;
	case PS_ANGULAR_SPEED:
		{
			ee0::Variant var0;
			var0.m_type = ee0::VT_FLOAT;
			var0.m_val.flt = cfg->angular_spd * SM_RAD_TO_DEG;
			variants.SetVariant("var0", var0);

			ee0::Variant var1;
			var1.m_type = ee0::VT_FLOAT;
			var1.m_val.flt = cfg->angular_spd_var * SM_RAD_TO_DEG;
			variants.SetVariant("var1", var1);
		}
		break;
	case PS_DISTURBANCE_RADIUS:
		{
			ee0::Variant var0;
			var0.m_type = ee0::VT_FLOAT;
			var0.m_val.flt = cfg->dis_region;
			variants.SetVariant("var0", var0);

			ee0::Variant var1;
			var1.m_type = ee0::VT_FLOAT;
			var1.m_val.flt = cfg->dis_region_var;
			variants.SetVariant("var1", var1);
		}
		break;
	case PS_DISTURBANCE_SPD:
		{
			ee0::Variant var0;
			var0.m_type = ee0::VT_FLOAT;
			var0.m_val.flt = cfg->dis_spd;
			variants.SetVariant("var0", var0);

			ee0::Variant var1;
			var1.m_type = ee0::VT_FLOAT;
			var1.m_val.flt = cfg->dis_spd_var;
			variants.SetVariant("var1", var1);
		}
		break;
	case PS_GRAVITY:
		{
			ee0::Variant var;
			var.m_type = ee0::VT_FLOAT;
			var.m_val.flt = cfg->gravity;
			variants.SetVariant("var", var);
		}
		break;
	case PS_LINEAR_ACC:
		{
			ee0::Variant var0;
			var0.m_type = ee0::VT_FLOAT;
			var0.m_val.flt = cfg->linear_acc;
			variants.SetVariant("var0", var0);

			ee0::Variant var1;
			var1.m_type = ee0::VT_FLOAT;
			var1.m_val.flt = cfg->linear_acc_var;
			variants.SetVariant("var1", var1);
		}
		break;
	case PS_FADEOUT_TIME:
		{
			ee0::Variant var;
			var.m_type = ee0::VT_FLOAT;
			var.m_val.flt = cfg->fadeout_time * 1000;
			variants.SetVariant("var", var);
		}
		break;
	case PS_START_POS:
		{
			ee0::Variant var0;
			var0.m_type = ee0::VT_FLOAT;
			var0.m_val.flt = cfg->start_radius;
			variants.SetVariant("var0", var0);

			ee0::Variant var1;
			var1.m_type = ee0::VT_FLOAT;
			var1.m_val.flt = cfg->start_height;
			variants.SetVariant("var1", var1);
		}
		break;
	}
}

void WxPropertyPanel::StoreToEmitter()
{
	for (int i = 0, n = m_sliders.size(); i < n; ++i) {
		m_sliders[i]->StoreToCB();
	}

	m_p3d.SetHori(m_min_hori->GetValue(), m_max_hori->GetValue());
	m_p3d.SetVert(m_min_vert->GetValue(), m_max_vert->GetValue());
	m_p3d.SetGround(m_ground->GetSelection());
}

void WxPropertyPanel::InitLayout()
{
	wxSizer* top_sizer = new wxBoxSizer(wxVERTICAL);
	top_sizer->AddSpacer(10);

	// Mode
	m_static_mode = new wxCheckBox(this, wxID_ANY, LANG[LK_STATIC_MODE]);
	Connect(m_static_mode->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED, 
		wxCommandEventHandler(WxPropertyPanel::OnSetStaticMode));
	top_sizer->Add(m_static_mode);

	// Count
	auto s_count = new ee0::WxSliderOneCtrl(
		this, LANG[LK_COUNT], "count", *this, PS_COUNT, ee0::SliderItem("", "", COUNT, 1, 5000)
	);
	top_sizer->Add(s_count);
	m_sliders.push_back(s_count);
	m_count_ctrl = s_count;
	// Emission Time
	auto s_emission_time = new ee0::WxSliderOneCtrl(
		this, LANG[LK_EMISSION_TIME], "emission_time", *this, PS_EMISSION_TIME, 
		ee0::SliderItem("", "", EMISSION_TIME, 10, 5000)
	);
	top_sizer->Add(s_emission_time);
	top_sizer->AddSpacer(10);
	m_sliders.push_back(s_emission_time);
	m_time_ctrl = s_emission_time;

	// Life
	auto s_life = new ee0::WxSliderTwoCtrl(
		this, LANG[LK_LIFE], "life", *this, PS_LIFE_TIME, 
		ee0::SliderItem(LANG[LK_CENTER], ITEM_ATTR_CENTER, LIFE_CENTER, 0, 5000), 
		ee0::SliderItem(LANG[LK_OFFSET], ITEM_ATTR_OFFSET, LIFE_OFFSET, 0, 2500)
	);
	top_sizer->Add(s_life);
	top_sizer->AddSpacer(10);
	m_sliders.push_back(s_life);
	// Hori
	{
		wxStaticBox* bounding = new wxStaticBox(this, wxID_ANY, LANG[LK_HORI]);
		wxSizer* horiSizer = new wxStaticBoxSizer(bounding, wxHORIZONTAL);
		{
			wxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
			sizer->Add(new wxStaticText(this, wxID_ANY, LANG[LK_MIN]));	

			m_min_hori = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, 
				wxSize(70, -1), wxSP_ARROW_KEYS, -360, 360, MIN_HORI);
			Connect(m_min_hori->GetId(), wxEVT_COMMAND_SPINCTRL_UPDATED, 
				wxSpinEventHandler(WxPropertyPanel::OnSetHori));
			sizer->Add(m_min_hori);

			horiSizer->Add(sizer);
		}
		{
			wxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
			sizer->Add(new wxStaticText(this, wxID_ANY, LANG[LK_MAX]));	

			m_max_hori = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, 
				wxSize(70, -1), wxSP_ARROW_KEYS, -360, 360, MAX_HORI);
			Connect(m_max_hori->GetId(), wxEVT_COMMAND_SPINCTRL_UPDATED, 
				wxSpinEventHandler(WxPropertyPanel::OnSetHori));
			sizer->Add(m_max_hori);

			horiSizer->Add(sizer);
		}
		top_sizer->Add(horiSizer);
	}
	top_sizer->AddSpacer(10);
	// Vert
	{
		wxStaticBox* bounding = new wxStaticBox(this, wxID_ANY, LANG[LK_VERT]);
		wxSizer* vertSizer = new wxStaticBoxSizer(bounding, wxHORIZONTAL);
		{
			wxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
			sizer->Add(new wxStaticText(this, wxID_ANY, LANG[LK_MIN]));

			m_min_vert = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, 
				wxSize(70, -1), wxSP_ARROW_KEYS, -360, 360, MIN_VERT);
			Connect(m_min_vert->GetId(), wxEVT_COMMAND_SPINCTRL_UPDATED, 
				wxSpinEventHandler(WxPropertyPanel::OnSetVert));
			sizer->Add(m_min_vert);

			vertSizer->Add(sizer);
		}
		vertSizer->AddSpacer(10);
		{
			wxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
			sizer->Add(new wxStaticText(this, wxID_ANY, LANG[LK_MAX]));	

			m_max_vert = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, 
				wxSize(70, -1), wxSP_ARROW_KEYS, -360, 360, MAX_VERT);
			Connect(m_max_vert->GetId(), wxEVT_COMMAND_SPINCTRL_UPDATED, 
				wxSpinEventHandler(WxPropertyPanel::OnSetVert));
			sizer->Add(m_max_vert);

			vertSizer->Add(sizer);
		}
		top_sizer->Add(vertSizer);
	}
	top_sizer->AddSpacer(10);
	// Radial Speed
	auto s_r_spd = new ee0::WxSliderTwoCtrl(
		this, LANG[LK_RADIAL_SPEED], "radial_speed", *this, PS_RADIAL_SPEED, 
		ee0::SliderItem(LANG[LK_CENTER], ITEM_ATTR_CENTER, RADIAL_SPEED_CENTER, -40000, 40000), 
		ee0::SliderItem(LANG[LK_OFFSET], ITEM_ATTR_OFFSET, RADIAL_SPEED_OFFSET, 0, 20000)
	);
	top_sizer->Add(s_r_spd);
	top_sizer->AddSpacer(10);
	m_sliders.push_back(s_r_spd);
	// Tangential Speed
	auto s_t_spd = new ee0::WxSliderTwoCtrl(
		this, LANG[LK_TANGENTIAL_SPEED], "tangential_spd", *this, PS_TANGENTIAL_SPEED, 
		ee0::SliderItem(LANG[LK_CENTER], ITEM_ATTR_CENTER, 0, -400, 400), 
		ee0::SliderItem(LANG[LK_OFFSET], ITEM_ATTR_OFFSET, 0, 0, 200)
	);
	top_sizer->Add(s_t_spd);
	top_sizer->AddSpacer(10);
	m_sliders.push_back(s_t_spd);
	// Angular Speed
	auto s_a_spd = new ee0::WxSliderTwoCtrl(
		this, LANG[LK_ANGULAR_SPEED], "angular_speed", *this, PS_ANGULAR_SPEED, 
		ee0::SliderItem(LANG[LK_CENTER], ITEM_ATTR_CENTER, ANGULAR_SPEED_CENTER, -3600, 3600), 
		ee0::SliderItem(LANG[LK_OFFSET], ITEM_ATTR_OFFSET, ANGULAR_SPEED_OFFSET, 0, 360)
	);
	top_sizer->Add(s_a_spd);
	top_sizer->AddSpacer(10);
	m_sliders.push_back(s_a_spd);
	// Disturbance 
	{
		wxStaticBox* bounding = new wxStaticBox(this, wxID_ANY, LANG[LK_DISTURBANCE]);
		wxSizer* sizer = new wxStaticBoxSizer(bounding, wxVERTICAL);

		auto s_dis_r = new ee0::WxSliderTwoCtrl(
			this, LANG[LK_RANGE], "disturbance_radius", *this, PS_DISTURBANCE_RADIUS, 
			ee0::SliderItem(LANG[LK_CENTER], ITEM_ATTR_CENTER, DISTURBANCE_RADIUS_CENTER, 0, 9999), 
			ee0::SliderItem(LANG[LK_OFFSET], ITEM_ATTR_OFFSET, DISTURBANCE_RADIUS_OFFSET, 0, 1000)
		);
		sizer->Add(s_dis_r);
		m_sliders.push_back(s_dis_r);

		auto s_dis_spd = new ee0::WxSliderTwoCtrl(
			this, LANG[LK_SPEED], "disturbance_spd", *this, PS_DISTURBANCE_SPD, 
			ee0::SliderItem(LANG[LK_CENTER], ITEM_ATTR_CENTER, DISTURBANCE_SPD_CENTER, 0, 9999), 
			ee0::SliderItem(LANG[LK_OFFSET], ITEM_ATTR_OFFSET, DISTURBANCE_SPD_OFFSET, 0, 1000)
		);
		sizer->Add(s_dis_spd);
		m_sliders.push_back(s_dis_spd);

		top_sizer->Add(sizer);
		top_sizer->AddSpacer(10);
	}
	// Gravity
	auto s_gravity = new ee0::WxSliderOneCtrl(this, LANG[LK_GRAVITY], "gravity", *this, 
		PS_GRAVITY, ee0::SliderItem("", "", GRAVITY, -5000, 25000));
	top_sizer->Add(s_gravity);
	top_sizer->AddSpacer(10);
	m_sliders.push_back(s_gravity);
	// Linear Acc
	auto s_lacc = new ee0::WxSliderTwoCtrl(
		this, LANG[LK_LINEAR_ACC], "linear_acc", *this, PS_LINEAR_ACC, 
		ee0::SliderItem(LANG[LK_CENTER], ITEM_ATTR_CENTER, LINEAR_ACC_CENTER, -9999, 9999), 
		ee0::SliderItem(LANG[LK_OFFSET], ITEM_ATTR_OFFSET, LINEAR_ACC_OFFSET, 0, 999)
	);
	top_sizer->Add(s_lacc);
	top_sizer->AddSpacer(10);
	m_sliders.push_back(s_lacc);
	// Fadeout Time
	auto s_fadeout = new ee0::WxSliderOneCtrl(this, LANG[LK_FADEOUT_TIME], 
		"fadeout_time", *this, PS_FADEOUT_TIME, ee0::SliderItem("", "", FADEOUT_TIME, 10, 2500));
	top_sizer->Add(s_fadeout);
	top_sizer->AddSpacer(10);
	m_sliders.push_back(s_fadeout);
	// Ground
	{
		const int size = 3;
		wxString choices[size];
		choices[0] = LANG[LK_NO_GROUND];
		choices[1] = LANG[LK_GROUND_WITH_BOUNCE];
		choices[2] = LANG[LK_GROUND_NO_BOUNCE];
		m_ground = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, size, choices);
		m_ground->SetSelection(1);
		Connect(m_ground->GetId(), wxEVT_COMMAND_CHOICE_SELECTED, 
			wxCommandEventHandler(WxPropertyPanel::OnSetGround));
		top_sizer->Add(m_ground);
	}
	top_sizer->AddSpacer(10);
	// Start Position
	auto s_start_pos = new ee0::WxSliderTwoCtrl(this, LANG[LK_START_POSITION], "start_pos", *this, PS_START_POS, 
		ee0::SliderItem(LANG[LK_RADIUS], ITEM_ATTR_RADIUS, START_RADIUS, 0, 5000), 
		ee0::SliderItem(LANG[LK_HEIGHT], ITEM_ATTR_HEIGHT, START_HEIGHT, -25000, 25000));
	top_sizer->Add(s_start_pos);
	top_sizer->AddSpacer(10);
	m_sliders.push_back(s_start_pos);
	// orient_to_movement
	{
		m_orient_to_movement = new wxCheckBox(this, wxID_ANY, LANG[LK_ORIENT_MOVEMENT]);	
		Connect(m_orient_to_movement->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(WxPropertyPanel::OnSetOrientToMovement));
		top_sizer->Add(m_orient_to_movement);
	}
	top_sizer->AddSpacer(10);
	// 	// orient_to_parent
	// 	{
	// 		m_orient_to_parent = new wxCheckBox(this, wxID_ANY, LANG[LK_ORIENT_PARENT]);
	// 		leftSizer->Add(m_orient_to_parent);
	// 	}
	// 	leftSizer->AddSpacer(10);

	// Blend
	{
		wxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);

		sizer->Add(new wxStaticText(this, wxID_ANY, LANG[LK_ORIENT_MOVEMENT]));

		const int size = 3;
		wxString choices[size];
		choices[0] = LANG[LK_BLEND_NULL];
		choices[1] = LANG[LK_BLEND_ADD];
		choices[2] = LANG[LK_BLEND_SUBTRACT];
		m_blend = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, size, choices);
		m_blend->SetSelection(0);
		Connect(m_blend->GetId(), wxEVT_COMMAND_CHOICE_SELECTED, 
			wxCommandEventHandler(WxPropertyPanel::OnSetBlend));
		sizer->Add(m_blend);

		top_sizer->Add(sizer);
	}

	SetSizer(top_sizer);
}

void WxPropertyPanel::OnSetStaticMode(wxCommandEvent& event)
{
	bool static_mode = event.IsChecked();
//	OnSetStaticMode(static_mode);
}

void WxPropertyPanel::OnSetLoop(wxCommandEvent& event)
{
//	m_stage->m_ps->SetLoop(event.IsChecked());
}

void WxPropertyPanel::OnSetLocalModeDraw(wxCommandEvent& event)
{
//	m_stage->m_ps->SetLocal(event.IsChecked());
}

void WxPropertyPanel::OnSetHori(wxSpinEvent& event)
{
//	m_stage->m_ps->SetHori(m_min_hori->GetValue(), m_max_hori->GetValue());
}

void WxPropertyPanel::OnSetVert(wxSpinEvent& event)
{
//	m_stage->m_ps->SetVert(m_min_vert->GetValue(), m_max_vert->GetValue());
}

void WxPropertyPanel::OnSetGround(wxCommandEvent& event)
{
	int ground = m_ground->GetSelection();
//	m_stage->m_ps->SetGround(ground);
}

void WxPropertyPanel::OnSetOrientToMovement(wxCommandEvent& event)
{
//	m_stage->m_ps->SetOrientToMovement(event.IsChecked());
}

void WxPropertyPanel::OnSetBlend(wxCommandEvent& event)
{
	int blend = m_blend->GetSelection();
//	m_stage->m_ps->SetBlend(blend);
}

}
}