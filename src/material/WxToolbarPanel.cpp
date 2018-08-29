#include "material/WxToolbarPanel.h"

#include <ee3/WxMaterialPreview.h>

#include <wx/sizer.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>

namespace eone
{
namespace material
{

WxToolbarPanel::WxToolbarPanel(wxWindow* parent)
	: wxPanel(parent)
{
	InitLayout();
}

void WxToolbarPanel::InitLayout()
{
	auto sizer = new wxBoxSizer(wxVERTICAL);

	sizer->Add(m_preview = new ee3::WxMaterialPreview(this, sm::ivec2(300, 300)));

	sizer->AddSpacer(10);

	m_pg = new wxPropertyGrid(this, -1, wxDefaultPosition, wxDefaultSize,
		wxPG_SPLITTER_AUTO_CENTER | wxPG_BOLD_MODIFIED
	);
	Connect(m_pg->GetId(), wxEVT_PG_CHANGED, wxPropertyGridEventHandler(
		WxToolbarPanel::OnPropertyGridChange));
	sizer->Add(m_pg, 1, wxEXPAND);

	SetSizer(sizer);
}

void WxToolbarPanel::OnPropertyGridChange(wxPropertyGridEvent& event)
{
}

}
}