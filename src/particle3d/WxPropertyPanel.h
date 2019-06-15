#pragma once

#include "frame/config.h"

#ifdef MODULE_PARTICLE3D

#include <ee0/GuiCallback.h>

#include <wx/panel.h>

namespace ee0 { class WxSliderCtrl; }
namespace et { class P3dTemplate; class P3dInstance; }

class wxCheckBox;
class wxChoice;

namespace eone
{
namespace particle3d
{

class WxComponentPanel;

class WxPropertyPanel : public wxPanel, public ee0::GuiCallback
{
public:
	WxPropertyPanel(wxWindow* parent, et::P3dTemplate& p3d,
		et::P3dInstance& p3d_inst);

	virtual void SetValue(int key, const ee0::VariantSet& variants) override;
	virtual void GetValue(int key, ee0::VariantSet& variants) const override;

	void StoreToEmitter();

private:
	void InitLayout();

	void OnSetStaticMode(wxCommandEvent& event);

	void OnSetLoop(wxCommandEvent& event);
	void OnSetLocalModeDraw(wxCommandEvent& event);
	void OnSet(wxCommandEvent& event);
	void OnSetHori(wxSpinEvent& event);
	void OnSetVert(wxSpinEvent& event);
	void OnSetGround(wxCommandEvent& event);
	void OnSetOrientToMovement(wxCommandEvent& event);
	void OnSetBlend(wxCommandEvent& event);

private:
	et::P3dTemplate& m_p3d;
	et::P3dInstance& m_p3d_inst;

	std::vector<ee0::WxSliderCtrl*> m_sliders;

	wxCheckBox *m_loop, *m_local;

	wxCheckBox* m_static_mode;

	wxPanel *m_count_ctrl, *m_time_ctrl;

	wxSpinCtrl  *m_min_hori, *m_max_hori;
	wxSpinCtrl  *m_min_vert, *m_max_vert;
	wxChoice*   m_ground;
	wxCheckBox* m_orient_to_movement;
	wxChoice*   m_blend;

}; // WxPropertyPanel

}
}

#endif // MODULE_PARTICLE3D