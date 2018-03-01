#include "frame/WxFrame.h"
#include "frame/Application.h"
#include "frame/WxStagePanel.h"
#include "frame/NodeSelectOP.h"
#include "frame/WxPreviewPanel.h"

#include "scale9/WxStagePage.h"
#include "scale9/WxStageCanvas.h"
#include "mask/WxStagePage.h"

#include <ee0/WxListSelectDlg.h>
#include <ee0/MsgHelper.h>
#include <ee2/WxStagePage.h>
#include <ee2/WxStageCanvas.h>
#include <ee2/NodeFactory.h>
#include <ee3/WxStagePage.h>
#include <ee3/WxStageCanvas.h>
#include <ee3/NodeArrangeOP.h>

#include <node0/SceneNode.h>
#include <node2/CompMask.h>
#include <node2/CompScale9.h>

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
			auto canvas = std::make_shared<ee2::WxStageCanvas>(page, &app->m_rc, nullptr);
			page->GetImpl().SetCanvas(canvas);
			page->GetImpl().SetEditOP(std::make_shared<NodeSelectOP>(*page, app->m_rc, app->m_wc));

			app->m_stage->AddNewPage(page, "Scene2D");
		}
		break;
	case NEW_SCENE3D:
		{
			auto page = new ee3::WxStagePage(app->m_frame, app->m_library);
			auto canvas = std::make_shared<ee3::WxStageCanvas>(page, &app->m_rc, nullptr);
			page->GetImpl().SetCanvas(canvas);
			page->GetImpl().SetEditOP(std::make_shared<ee3::NodeArrangeOP>(*page));

			app->m_stage->AddNewPage(page, "Scene3d");
		}
		break;
	case NEW_SCALE9:
		{
			//auto& preview_sub_mgr = app->m_preview->GetSubjectMgr();
			//preview_sub_mgr.NotifyObservers(ee0::MSG_CLEAR_SCENE_NODE);
			//auto node = ee2::NodeFactory::Instance()->Create(ee2::NODE_SCALE9);
			//ee0::MsgHelper::InsertNode(preview_sub_mgr, node);

			// todo
			auto node = ee2::NodeFactory::Instance()->Create(ee2::NODE_SCALE9);

			auto page = new scale9::WxStagePage(app->m_frame, app->m_library, node->GetComponent<n2::CompScale9>());
			auto canvas = std::make_shared<scale9::WxStageCanvas>(page, app->m_rc);
			page->GetImpl().SetCanvas(canvas);
			page->GetImpl().SetEditOP(std::make_shared<NodeSelectOP>(*page, app->m_rc, app->m_wc));

			app->m_stage->AddNewPage(page, "Scale9");
		}
		break;
	case NEW_MASK:
		{
			//auto& preview_sub_mgr = app->m_preview->GetSubjectMgr();
			//preview_sub_mgr.NotifyObservers(ee0::MSG_CLEAR_SCENE_NODE);
			//auto node = ee2::NodeFactory::Instance()->Create(ee2::NODE_MASK);
			//ee0::MsgHelper::InsertNode(preview_sub_mgr, node);

			// todo
			auto node = ee2::NodeFactory::Instance()->Create(ee2::NODE_MASK);

			auto page = new mask::WxStagePage(app->m_frame, app->m_library, node->GetComponent<n2::CompMask>());
			auto canvas = std::make_shared<ee2::WxStageCanvas>(page, &app->m_rc, nullptr);
			page->GetImpl().SetCanvas(canvas);
			page->GetImpl().SetEditOP(std::make_shared<NodeSelectOP>(*page, app->m_rc, app->m_wc));

			app->m_stage->AddNewPage(page, "Mask");
		}
		break;
	}
}

}