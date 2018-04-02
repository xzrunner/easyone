#pragma once

#include <ee0/GuiCallback.h>

#include <wx/panel.h>

struct p3d_symbol;
class wxTextCtrl;

namespace ee0 { class WxSliderCtrl; }

namespace eone
{
namespace particle3d
{

class WxEmitterPanel;

class WxComponentPanel : public wxPanel, public ee0::GuiCallback
{
public:
	WxComponentPanel(wxWindow* parent, p3d_symbol* sym, 
		const std::string& filepath, WxEmitterPanel* et_panel);

	virtual void SetValue(int key, const ee0::VariantSet& variants) override;
	virtual void GetValue(int key, ee0::VariantSet& variants) const override;

	void UpdateBtnColor();

	void SetCount(int count);

private:
	void InitLayout();
	void InitLayout(wxSizer* sizer);

	void OnDelete(wxCommandEvent& event);

	void OnSetCount(wxSpinEvent& event);

	void OnSetBeginMulCol(wxCommandEvent& event);
	void OnSetEndMulCol(wxCommandEvent& event);
	void OnSetBeginAddCol(wxCommandEvent& event);
	void OnSetEndAddCol(wxCommandEvent& event);

	void ChangeColor(uint8_t rgba[4]);

private:
	p3d_symbol* m_sym;
	std::string m_filepath;

	WxEmitterPanel* m_et_panel;

	std::vector<ee0::WxSliderCtrl*> m_sliders;

	wxTextCtrl* m_name;

	wxSpinCtrl* m_count;

	wxButton *m_begin_mul_col_btn, *m_end_mul_col_btn;
	wxButton *m_begin_add_col_btn, *m_end_add_col_btn;

}; // WxComponentPanel

}
}