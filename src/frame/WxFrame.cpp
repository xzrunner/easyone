#include "frame/WxFrame.h"
#include "frame/Application.h"
#include "frame/StagePageType.h"
#include "frame/StagePageFactory.h"

#include <ee0/WxListSelectDlg.h>

namespace
{

static const std::vector<std::pair<uint32_t, std::string>> PAGE_LIST =
{
	std::make_pair(eone::PAGE_SCENE2D, "Scene2D"),
	std::make_pair(eone::PAGE_SCENE3D, "Scene3D"),

	std::make_pair(eone::PAGE_SCALE9,  "Scale9"),
	std::make_pair(eone::PAGE_MASK,    "Mask"),
	std::make_pair(eone::PAGE_MESH,    "Mesh"),
};

}

namespace eone
{

WxFrame::WxFrame()
	: ee0::WxFrame("EasyOne", true)
{
}

void WxFrame::OnNew(wxCommandEvent& event)
{
	ee0::WxListSelectDlg dlg(this, "New page", PAGE_LIST, wxGetMousePosition());
	if (dlg.ShowModal() != wxID_OK) {
		return;
	}

	auto app = std::static_pointer_cast<Application>(m_app);
	auto type = dlg.GetSelectedID();
	StagePageFactory::Create(type, app->GetStagePanel());
}

}