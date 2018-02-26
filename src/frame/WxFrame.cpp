#include "frame/WxFrame.h"
#include "frame/Application.h"
#include "frame/WxStagePanel.h"
#include "frame/NodeSelectOP.h"

#include <ee0/WxListSelectDlg.h>
#include <ee2/WxStagePage.h>
#include <ee2/WxStageCanvas.h>
#include <ee3/WxStagePage.h>
#include <ee3/WxStageCanvas.h>
#include <ee3/NodeArrangeOP.h>

namespace
{

enum NewType
{
	NEW_SCENE2D = 0,
	NEW_SCENE3D,

	NEW_SCALE9,
	NEW_MASK,
	NEW_MESH,
};

static const std::vector<std::pair<uint32_t, std::string>> NEW_LIST =
{
	std::make_pair(NEW_SCENE2D, "Scene2D"),
	std::make_pair(NEW_SCENE3D, "Scene3D"),

	std::make_pair(NEW_SCALE9,  "Scale9"),
	std::make_pair(NEW_MASK,    "Mask"),
	std::make_pair(NEW_MESH,    "Mesh"),
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
	ee0::WxListSelectDlg dlg(this, "Create node", NEW_LIST);
	if (dlg.ShowModal() != wxID_OK) {
		return;
	}

	auto app = std::static_pointer_cast<Application>(m_app);
	auto id = dlg.GetSelectedID();
	switch (id)
	{
	case NEW_SCENE2D:
		{
			auto page = new ee2::WxStagePage(app->m_frame, app->m_library);
			auto canvas = std::make_shared<ee2::WxStageCanvas>(page, app->m_gl_ctx);
			page->GetImpl().SetCanvas(canvas);
			page->GetImpl().SetEditOP(std::make_shared<NodeSelectOP>(*page));

			app->m_stage->AddPage(page, ("Scene2D"));
		}
		break;
	case NEW_SCENE3D:
		{
			auto page = new ee3::WxStagePage(app->m_frame, app->m_library);
			auto canvas = std::make_shared<ee3::WxStageCanvas>(page, app->m_gl_ctx);
			page->GetImpl().SetCanvas(canvas);
			page->GetImpl().SetEditOP(std::make_shared<ee3::NodeArrangeOP>(*page));

			app->m_stage->AddPage(page, ("Scene3d"));
		}
		break;
	}
}

}