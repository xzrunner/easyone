#include "frame/StagePageFactory.h"
#include "frame/NodeFactory.h"
#include "frame/StagePageType.h"
#include "frame/WxStagePanel.h"
#include "frame/NodeSelectOP.h"
#include "frame/Blackboard.h"
#include "frame/Application.h"
#include "frame/WxPreviewPanel.h"
#include "frame/WxPreviewCanvas.h"

#include "scene2d/WxStagePage.h"
#include "scene3d/WxStagePage.h"
#include "scale9/WxStagePage.h"
#include "scale9/WxStageCanvas.h"
#include "scale9/ResizeScale9OP.h"
#include "mask/WxStagePage.h"

#include <ee0/WxListSelectDlg.h>
#include <ee0/MsgHelper.h>
#include <ee2/WxStageCanvas.h>
#include <ee3/WxStageCanvas.h>
#include <ee3/NodeArrangeOP.h>
#include <ee3/CamControlOP.h>

#include <node0/SceneNode.h>
#include <node2/CompMask.h>
#include <node2/CompScale9.h>

namespace eone
{

WxStagePage* StagePageFactory::Create(int page_type, WxStagePanel* stage_panel)
{
	WxStagePage* page = nullptr;

	auto bb = Blackboard::Instance();
	auto frame = bb->GetFrame();
	auto library = bb->GetLiraryPanel();
	auto& rc = bb->GetRenderContext();
	auto& wc = bb->GetWindowContext();

	switch (page_type)
	{
	case PAGE_SCENE2D:
		{
			auto node = NodeFactory::Create(NODE_SCENE2D);
			page = new scene2d::WxStagePage(frame, library, node);
			auto canvas = std::make_shared<ee2::WxStageCanvas>(page, &rc, nullptr);
			page->GetImpl().SetCanvas(canvas);
			page->GetImpl().SetEditOP(std::make_shared<NodeSelectOP>(*page, rc, wc));

			stage_panel->AddNewPage(page, "Scene2D");
		}
		break;
	case PAGE_SCENE3D:
		{
			auto node = NodeFactory::Create(NODE_SCENE3D);
			page = new scene3d::WxStagePage(frame, library, node);
			auto canvas = std::make_shared<ee3::WxStageCanvas>(page, &rc, nullptr);
			page->GetImpl().SetCanvas(canvas);
			page->GetImpl().SetEditOP(std::make_shared<ee3::NodeArrangeOP>(*page));

			stage_panel->AddNewPage(page, "Scene3d");
		}
		break;
	case PAGE_SCALE9:
		{
			auto node = NodeFactory::Create(NODE_SCALE9);
			page = new scale9::WxStagePage(frame, library, node);
			auto canvas = std::make_shared<scale9::WxStageCanvas>(page, rc);
			page->GetImpl().SetCanvas(canvas);
			page->GetImpl().SetEditOP(std::make_shared<NodeSelectOP>(*page, rc, wc));

			stage_panel->AddNewPage(page, "Scale9");
		}
		break;
	case PAGE_MASK:
		{
			auto node = NodeFactory::Create(NODE_MASK);
			page = new mask::WxStagePage(frame, library, node);
			auto canvas = std::make_shared<ee2::WxStageCanvas>(page, &rc, nullptr);
			page->GetImpl().SetCanvas(canvas);
			page->GetImpl().SetEditOP(std::make_shared<NodeSelectOP>(*page, rc, wc));

			stage_panel->AddNewPage(page, "Mask");
		}
		break;
	}

	return page;
}

void StagePageFactory::CreatePreviewOP()
{
	auto bb = Blackboard::Instance();
	auto preview = bb->GetPreviewPanel();
	auto canvas = std::dynamic_pointer_cast<WxPreviewCanvas>(
		preview->GetImpl().GetCanvas());
	auto curr_page = bb->GetStagePanel()->GetCurrentStagePage();
	auto& sub_mgr = curr_page->GetSubjectMgr();

	switch (curr_page->GetPageType())
	{
	case PAGE_SCENE2D:
		{
			auto preview_op = std::make_shared<ee2::CamControlOP>(
				*canvas->GetCamera(), sub_mgr);
			preview->GetImpl().SetEditOP(preview_op);
		}
		break;
	case PAGE_SCENE3D:
		{
			//auto preview_op = std::make_shared<ee3::CamControlOP>(
			//	canvas->GetCamera(), canvas->GetViewport(), page->GetSubjectMgr());
			//preview->GetImpl().SetEditOP(preview_op);
		}
		break;
	case PAGE_SCALE9:
		{
			auto preview_op = std::make_shared<scale9::ResizeScale9OP>(
				preview, curr_page->GetEditedNode());
			preview->GetImpl().SetEditOP(preview_op);
		}
		break;
	case PAGE_MASK:
		{
			auto preview_op = std::make_shared<ee2::CamControlOP>(
				*canvas->GetCamera(), sub_mgr);
			preview->GetImpl().SetEditOP(preview_op);
		}
		break;
	}
}

}