#include "frame/StagePageFactory.h"
#include "frame/GameObjFactory.h"
#include "frame/StagePageType.h"
#include "frame/WxStagePanel.h"
#include "frame/WxStageCanvas2D.h"
#include "frame/WxStageCanvas3D.h"
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
#include "script/WxStagePage.h"
#include "script/WxStageCanvas.h"
#include "anim/WxStagePage.h"
#include "particle3d/WxStagePage.h"
#include "particle3d/PlayParticlesOP.h"
#include "model/WxStagePage.h"
#include "model/WxStageCanvas.h"
#include "anim3/WxStagePage.h"
#include "anim3/WxStageCanvas.h"
#include "sgraph/WxStagePage.h"
#include "bprint/WxStagePage.h"
#include "quake/WxStagePage.h"
#include "quake/WxStageCanvas.h"

#include <ee0/WxListSelectDlg.h>
#include <ee0/MsgHelper.h>
#include <ee2/WxStageCanvas.h>
#include <ee2/ArrangeNodeOP.h>
#include <ee3/WxStageCanvas.h>
#include <ee3/NodeArrangeOP.h>
#include <ee3/WorldTravelOP.h>
#include <ee3/CameraDriveOP.h>
#include <ee3/SkeletonJointOP.h>

#include <node0/SceneNode.h>
#include <node2/CompMask.h>
#include <node2/CompScale9.h>
#include <node2/CompParticle3dInst.h>
#include <node3/CompModel.h>
#include <node3/CompModelInst.h>
#include <moon/Blackboard.h>
#include <moon/Context.h>
#include <painting3/PerspCam.h>
#include <blueprint/ConnectPinsOP.h>
#include <blueprint/NodeFactory.h>

#include <boost/filesystem.hpp>

namespace eone
{

WxStagePage* StagePageFactory::Create(ECS_WORLD_PARAM int page_type, WxStagePanel* stage_panel)
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
			auto obj = GameObjFactory::Create(ECS_WORLD_VAR GAME_OBJ_COMPLEX2D);
			page = new scene2d::WxStagePage(frame, library, ECS_WORLD_VAR obj);
			auto canvas = std::make_shared<WxStageCanvas2D>(page, ECS_WORLD_VAR rc);

			page->GetImpl().SetCanvas(canvas);

			auto prev_op = std::make_shared<NodeSelectOP>(
				canvas->GetCamera(), ECS_WORLD_VAR *page, rc, wc);

			auto op = std::make_shared<ee2::ArrangeNodeOP>(
				canvas->GetCamera(), *page, ECS_WORLD_VAR ee2::ArrangeNodeCfg(), prev_op);

			page->GetImpl().SetEditOP(op);

			stage_panel->AddNewPage(page, GetPageName(page->GetPageType()));
		}
		break;
	case PAGE_SCENE3D:
		{
			auto obj = GameObjFactory::Create(ECS_WORLD_VAR GAME_OBJ_COMPLEX3D);
			page = new scene3d::WxStagePage(frame, library, ECS_WORLD_VAR obj);
			auto canvas = std::make_shared<WxStageCanvas3D>(page, rc);
			page->GetImpl().SetCanvas(canvas);

			auto op = std::make_shared<ee3::NodeArrangeOP>(
				canvas->GetCamera(), *page, canvas->GetViewport());
			page->GetImpl().SetEditOP(op);

			stage_panel->AddNewPage(page, GetPageName(page->GetPageType()));
		}
		break;
	case PAGE_SCALE9:
		{
			auto obj = GameObjFactory::Create(ECS_WORLD_VAR GAME_OBJ_SCALE9);
			page = new scale9::WxStagePage(frame, library, ECS_WORLD_VAR obj);
			auto canvas = std::make_shared<scale9::WxStageCanvas>(page, ECS_WORLD_VAR rc);
			page->GetImpl().SetCanvas(canvas);

			page->GetImpl().SetEditOP(std::make_shared<NodeSelectOP>(
				canvas->GetCamera(), ECS_WORLD_VAR *page, rc, wc));

			stage_panel->AddNewPage(page, GetPageName(page->GetPageType()));
		}
		break;
	case PAGE_MASK:
		{
			auto obj = GameObjFactory::Create(ECS_WORLD_VAR GAME_OBJ_MASK);
			page = new mask::WxStagePage(frame, library, ECS_WORLD_VAR obj);
			auto canvas = std::make_shared<WxStageCanvas2D>(page, ECS_WORLD_VAR rc);
			page->GetImpl().SetCanvas(canvas);

			auto prev_op = std::make_shared<NodeSelectOP>(
				canvas->GetCamera(), ECS_WORLD_VAR *page, rc, wc);

			auto op = std::make_shared<ee2::ArrangeNodeOP>(
				canvas->GetCamera(), *page, ECS_WORLD_VAR ee2::ArrangeNodeCfg(), prev_op);

			page->GetImpl().SetEditOP(op);

			stage_panel->AddNewPage(page, GetPageName(page->GetPageType()));
		}
		break;
	case PAGE_ANIM:
		{
			auto obj = GameObjFactory::Create(ECS_WORLD_VAR GAME_OBJ_ANIM);
			page = new anim::WxStagePage(frame, library, ECS_WORLD_VAR obj);
			auto canvas = std::make_shared<WxStageCanvas2D>(page, ECS_WORLD_VAR rc);
			page->GetImpl().SetCanvas(canvas);

			auto prev_op = std::make_shared<NodeSelectOP>(
				canvas->GetCamera(), ECS_WORLD_VAR *page, rc, wc);

			auto op = std::make_shared<ee2::ArrangeNodeOP>(
				canvas->GetCamera(), *page, ECS_WORLD_VAR ee2::ArrangeNodeCfg(), prev_op);

			page->GetImpl().SetEditOP(op);

			stage_panel->AddNewPage(page, GetPageName(page->GetPageType()));
		}
		break;
	case PAGE_PARTICLE3D:
		{
			auto obj = GameObjFactory::Create(ECS_WORLD_VAR GAME_OBJ_PARTICLE3D);
			page = new particle3d::WxStagePage(frame, library, ECS_WORLD_VAR obj);
			auto canvas = std::make_shared<WxStageCanvas2D>(page, ECS_WORLD_VAR rc);
			page->GetImpl().SetCanvas(canvas);

#ifndef GAME_OBJ_ECS
			auto& p3d_inst = obj->GetUniqueComp<n2::CompParticle3dInst>();
			auto op = std::make_shared<particle3d::PlayParticlesOP>(
				canvas->GetCamera(), obj, page->GetSubjectMgr());

			page->GetImpl().SetEditOP(op);
#else
			// todo ecs
#endif // GAME_OBJ_ECS

			stage_panel->AddNewPage(page, GetPageName(page->GetPageType()));
		}
		break;

	case PAGE_MODEL:
		{
			auto obj = GameObjFactory::Create(ECS_WORLD_VAR GAME_OBJ_MODEL);
			auto mpage = new model::WxStagePage(frame, library, ECS_WORLD_VAR obj);
			page = mpage;
			auto canvas = std::make_shared<model::WxStageCanvas>(mpage, rc);
			mpage->GetImpl().SetCanvas(canvas);
			mpage->InitEditOp(canvas->GetCamera(), canvas->GetViewport());

			stage_panel->AddNewPage(page, GetPageName(page->GetPageType()));
		}
		break;
	case PAGE_ANIM3:
		{
			auto obj = GameObjFactory::Create(ECS_WORLD_VAR GAME_OBJ_MODEL);
			page = new anim3::WxStagePage(frame, library, ECS_WORLD_VAR obj);
			auto canvas = std::make_shared<anim3::WxStageCanvas>(page, rc);
			page->GetImpl().SetCanvas(canvas);

			auto prev_op = std::make_shared<ee3::CameraDriveOP>(
				canvas->GetCamera(), canvas->GetViewport(), page->GetSubjectMgr());

			auto op = std::make_shared<ee3::SkeletonJointOP>(
				canvas->GetCamera(), canvas->GetViewport(), page->GetSubjectMgr());
			op->SetPrevEditOP(prev_op);

			page->GetImpl().SetEditOP(op);

			stage_panel->AddNewPage(page, GetPageName(page->GetPageType()));
		}
		break;
	case PAGE_SHADER_GRAPH:
		{
			auto obj = GameObjFactory::Create(ECS_WORLD_VAR GAME_OBJ_COMPLEX2D);
			page = new sgraph::WxStagePage(frame, ECS_WORLD_VAR obj);
			auto canvas = std::make_shared<WxStageCanvas2D>(page, ECS_WORLD_VAR rc);
			page->GetImpl().SetCanvas(canvas);

			auto select_op = std::make_shared<NodeSelectOP>(
				canvas->GetCamera(), ECS_WORLD_VAR *page, rc, wc);

			ee2::ArrangeNodeCfg cfg;
			cfg.is_auto_align_open = false;
			cfg.is_deform_open = false;
			cfg.is_offset_open = false;
			cfg.is_rotate_open = false;
			auto arrange_op = std::make_shared<ee2::ArrangeNodeOP>(
				canvas->GetCamera(), *page, ECS_WORLD_VAR cfg, select_op);

			auto op = std::make_shared<bp::ConnectPinsOP>(
				canvas->GetCamera(), *page, bp::NodeFactory::Instance()->GetAllNodes()
			);
			op->SetPrevEditOP(arrange_op);
			page->GetImpl().SetEditOP(op);

			stage_panel->AddNewPage(page, GetPageName(page->GetPageType()));
		}
		break;

	case PAGE_SCRIPT:
		{
			std::string filter = "*.lua";
			wxFileDialog dlg(stage_panel, wxT("Choose script"), wxEmptyString, filter);
			if (dlg.ShowModal() != wxID_OK) {
				break;
			}

			auto obj = GameObjFactory::Create(ECS_WORLD_VAR GAME_OBJ_COMPLEX2D);
			page = new script::WxStagePage(frame, library, ECS_WORLD_VAR obj);
			auto canvas = std::make_shared<script::WxStageCanvas>(page, ECS_WORLD_VAR rc, dlg.GetPath().ToStdString());
			page->GetImpl().SetCanvas(canvas);

			auto dir = boost::filesystem::path(dlg.GetPath().ToStdString()).parent_path();
			moon::Blackboard::Instance()->SetWorkDir(dir.string());

			canvas->ScriptLoad();

			auto prev_op = std::make_shared<NodeSelectOP>(
				canvas->GetCamera(), ECS_WORLD_VAR *page, rc, wc);

			auto op = std::make_shared<ee2::ArrangeNodeOP>(
				canvas->GetCamera(), *page, ECS_WORLD_VAR ee2::ArrangeNodeCfg(), prev_op);

			page->GetImpl().SetEditOP(op);

			stage_panel->AddNewPage(page, GetPageName(page->GetPageType()));
		}
		break;

	case PAGE_BLUEPRINT:
		{
			auto obj = GameObjFactory::Create(ECS_WORLD_VAR GAME_OBJ_COMPLEX2D);
			page = new bprint::WxStagePage(frame, library, ECS_WORLD_VAR obj);
			auto canvas = std::make_shared<WxStageCanvas2D>(page, ECS_WORLD_VAR rc);
			page->GetImpl().SetCanvas(canvas);

			auto select_op = std::make_shared<NodeSelectOP>(
				canvas->GetCamera(), ECS_WORLD_VAR *page, rc, wc);

			ee2::ArrangeNodeCfg cfg;
			cfg.is_auto_align_open = false;
			cfg.is_deform_open     = false;
			cfg.is_offset_open     = false;
			cfg.is_rotate_open     = false;
			auto arrange_op = std::make_shared<ee2::ArrangeNodeOP>(
				canvas->GetCamera(), *page, ECS_WORLD_VAR cfg, select_op);

			auto op = std::make_shared<bp::ConnectPinsOP>(
				canvas->GetCamera(), *page, bp::NodeFactory::Instance()->GetAllNodes()
			);
			op->SetPrevEditOP(arrange_op);
			page->GetImpl().SetEditOP(op);

			stage_panel->AddNewPage(page, GetPageName(page->GetPageType()));
		}
		break;

	case PAGE_QUAKE:
		{
			auto obj = GameObjFactory::Create(ECS_WORLD_VAR GAME_OBJ_COMPLEX3D);
			auto quake_page = new quake::WxStagePage(frame, library, ECS_WORLD_VAR obj);
			page = quake_page;
			auto canvas = std::make_shared<quake::WxStageCanvas>(page, rc);
			page->GetImpl().SetCanvas(canvas);
			quake_page->InitViewports();

			quake_page->InitEditOP(canvas->GetCamera(), canvas->GetViewport());

			stage_panel->AddNewPage(page, GetPageName(page->GetPageType()));
		}
		break;
	}

	return page;
}

void StagePageFactory::CreatePreviewOP(
#ifdef GAME_OBJ_ECS
	e0::World& world
#endif // GAME_OBJ_ECS
)
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
				canvas->GetCamera(), sub_mgr);
			preview->GetImpl().SetEditOP(preview_op);
		}
		break;
	case PAGE_SCENE3D:
		{
			//auto preview_op = std::make_shared<ee3::WorldTravelOP>(
			//	canvas->GetCamera(), canvas->GetViewport(), page->GetSubjectMgr());
			//preview->GetImpl().SetEditOP(preview_op);
		}
		break;
	case PAGE_SCALE9:
		{
			auto preview_op = std::make_shared<scale9::ResizeScale9OP>(
				canvas->GetCamera(), preview, ECS_WORLD_VAR curr_page->GetEditedObj());
			preview->GetImpl().SetEditOP(preview_op);
		}
		break;
	case PAGE_MASK:
		{
			auto preview_op = std::make_shared<ee2::CamControlOP>(
				canvas->GetCamera(), sub_mgr);
			preview->GetImpl().SetEditOP(preview_op);
		}
		break;
	}
}

}