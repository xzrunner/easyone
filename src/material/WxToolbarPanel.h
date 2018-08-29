#pragma once

#include <wx/panel.h>

class wxPropertyGrid;
class wxPropertyGridEvent;

namespace ee3 { class WxMaterialPreview; }

namespace eone
{
namespace material
{

class WxToolbarPanel : public wxPanel
{
public:
	WxToolbarPanel(wxWindow* parent);

private:
	void InitLayout();

	void OnPropertyGridChange(wxPropertyGridEvent& event);

private:
	ee3::WxMaterialPreview* m_preview;

	wxPropertyGrid* m_pg;

}; // WxToolbarPanel

}
}