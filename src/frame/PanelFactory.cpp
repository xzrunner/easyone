#include "frame/PanelFactory.h"
#include "frame/GameObjFactory.h"
#include "frame/StagePageType.h"
#include "frame/WxStagePanel.h"
#include "frame/WxStageCanvas2D.h"
#include "frame/WxStageCanvas3D.h"
#include "frame/LeftDClickOP.h"
#include "frame/Blackboard.h"
#include "frame/Application.h"
#include "frame/WxPreviewPanel.h"
#include "frame/WxPreviewCanvas.h"
#include "frame/WxLibraryPanel.h"
#include "frame/WxBlueprintCanvas.h"
#include "frame/config.h"

#ifdef MODULE_SCENE2D
#include "scene2d/WxStagePage.h"
#endif // MODULE_SCENE2D
#ifdef MODULE_SCENE3D
#include "scene3d/WxStagePage.h"
#endif // MODULE_SCENE3D
#ifdef MODULE_SCALE9
#include "scale9/WxStagePage.h"
#include "scale9/WxStageCanvas.h"
#include "scale9/ResizeScale9OP.h"
#endif // MODULE_SCALE9
#ifdef MODULE_MASK
#include "mask/WxStagePage.h"
#endif // MODULE_MASK
#ifdef MODULE_MESH
#include "mesh/WxStagePage.h"
#include "mesh/WxStageCanvas.h"
#endif // MODULE_MESH
#ifdef MODULE_SCRIPT
#include "script/WxStagePage.h"
#include "script/WxStageCanvas.h"
#endif // MODULE_SCRIPT
#ifdef MODULE_ANIM
#include "anim/WxStagePage.h"
#endif // MODULE_ANIM
#ifdef MODULE_PARTICLE3D
#include "particle3d/WxStagePage.h"
#include "particle3d/PlayParticlesOP.h"
#endif // MODULE_PARTICLE3D
#ifdef MODULE_SHAPE2D
#include "shape2d/WxStagePage.h"
#include "shape2d/WxStageCanvas.h"
#endif // MODULE_SHAPE2D
#ifdef MODULE_SHAPE3D
#include "shape3d/WxStagePage.h"
#include "shape3d/WxStageCanvas.h"
#endif // MODULE_SHAPE3D
#ifdef MODULE_CAD
#include "cad/WxStagePage.h"
#include "cad/WxStageCanvas.h"
#endif // MODULE_CAD
#ifdef MODULE_MODEL
#include "model/WxStagePage.h"
#include "model/WxStageCanvas.h"
#endif // MODULE_MODEL
#ifdef MODULE_ANIM3
#include "anim3/WxStagePage.h"
#include "anim3/WxStageCanvas.h"
#endif // MODULE_ANIM3
#ifdef MODULE_PROTOTYPE
#include "prototype/WxStagePage.h"
#endif // MODULE_PROTOTYPE
#ifdef MODULE_SHADERGRAPH
#include "sgraph/WxStagePage.h"
#endif // MODULE_SHADERGRAPH
#ifdef MODULE_RENDERGRAPH
#include "rgraph/WxStagePage.h"
#endif // MODULE_RENDERGRAPH
#ifdef MODULE_BLUEPRINT
#include "bprint/WxStagePage.h"
#endif // MODULE_BLUEPRINT
#ifdef MODULE_QUAKE
#include "quake/WxStagePage.h"
#include "quake/WxStageCanvas.h"
#endif // MODULE_QUAKE
#include "physics3d/WxStagePage.h"
#include "physics3d/WxStageCanvas2D.h"
#include "physics3d/WxStageCanvas3D.h"
#include "physics3d/PBDSceneOP.h"
#ifdef MODULE_RAYGRAPH
#include "raygraph/WxStagePage.h"
#endif // MODULE_RAYGRAPH
#ifdef MODULE_GIGRAPH
#include "gi/WxStagePage.h"
#endif // MODULE_GIGRAPH
#ifdef MODULE_GUIGRAPH
#include "guigraph/WxStagePage.h"
#include "guigraph/WxStageCanvas.h"
#endif // MODULE_GUIGRAPH
#ifdef MODULE_PBRGRAPH
#include "pbrgraph/WxStagePage.h"
#endif // MODULE_PBRGRAPH
#ifdef MODULE_HDI_SOP
#include "hdi_sop/WxStagePage.h"
#endif // MODULE_HDI_SOP
#ifdef MODULE_HDI_VOP
#include "hdi_vop/WxStagePage.h"
#endif // MODULE_HDI_VOP
#ifdef MODULE_ARCHITECTURAL
#include "architectural/WxStagePage.h"
#endif // MODULE_ARCHITECTURAL
#ifdef MODULE_TERRAIN
#include "terrain/WxStagePage.h"
#endif // MODULE_TERRAIN
#ifdef MODULE_CITY
#include "city/WxStagePage.h"
#endif // MODULE_CITY
#ifdef MODULE_GEOMETRY
#include "geometry/WxStagePage.h"
#endif // MODULE_GEOMETRY
#ifdef MODULE_TOUCH_DESIGNER
#include "touchdesigner/WxStagePage.h"
#endif // MODULE_TOUCH_DESIGNER
#ifdef MODULE_VISION_LAB
#include "visionlab/WxStagePage.h"
#endif // MODULE_VISION_LAB
#ifdef MODULE_MATERIAL
#include "material/WxStagePage.h"
#endif // MODULE_MATERIAL
#ifdef MODULE_EDITOP
#include "editop/WxStagePage.h"
#endif // MODULE_EDITOP
#ifdef MODULE_TASK
#include "task/WxStagePage.h"
#endif // MODULE_TASK
#ifdef MODULE_VFXGRAPH
#include "vfx/WxStagePage.h"
#endif // MODULE_VFXGRAPH

#include <ee0/WxListSelectDlg.h>
#include <ee0/MsgHelper.h>
#include <ee2/WxStageCanvas.h>
#include <ee2/NodeSelectOP.h>
#include <ee3/WxStageCanvas.h>
#include <ee3/NodeArrangeOP.h>
#include <ee3/WorldTravelOP.h>
#include <ee3/CameraDriveOP.h>
#include <ee3/SkeletonJointOP.h>
#include <ee3/DragRigidOP.h>

#include <node0/SceneNode.h>
#include <node2/CompMask.h>
#include <node2/CompScale9.h>
#include <node2/CompParticle3dInst.h>
#include <node3/CompModel.h>
#include <node3/CompModelInst.h>
#include <moon/Blackboard.h>
#include <moon/Context.h>
#include <painting3/PerspCam.h>
#include <blueprint/ConnectPinOP.h>
#include <blueprint/Blueprint.h>
#include <blueprint/ArrangeNodeOP.h>
#include <blueprint/NodeSelectOP.h>
#include <shaderlab/ShaderLab.h>
#include <prototyping/ArrangeNodeOP.h>
#ifdef MODULE_RENDERGRAPH
#include <renderlab/RenderLab.h>
#endif // MODULE_RENDERGRAPH
#ifdef MODULE_RAYGRAPH
#include <raylab/RayLab.h>
#endif // MODULE_RAYGRAPH
#ifdef MODULE_HDI_SOP
#include <sopview/SOPView.h>
#include <sopview/WxStageCanvas.h>
#endif // MODULE_HDI_SOP
#ifdef MODULE_HDI_VOP
#include <vopview/VOPView.h>
#include <vopview/WxStageCanvas.h>
#endif // MODULE_HDI_VOP
#ifdef MODULE_ARCHITECTURAL
#include <draft2/EditPolylineOP.h>
#include <draft2/ShapeCapture.h>
#include <archlab/ArchLab.h>
#include <archlab/WxPreviewCanvas.h>
#endif // MODULE_ARCHITECTURAL
#ifdef MODULE_TERRAIN
#include <terrainlab/WxPreviewCanvas.h>
#include <terrainlab/TerrainLab.h>
#endif // MODULE_TERRAIN
#ifdef MODULE_CITY
#include <citylab/WxPreviewCanvas.h>
#endif // MODULE_CITY
#ifdef MODULE_GEOMETRY
#include <geolab/WxPreviewCanvas.h>
#endif // MODULE_GEOMETRY
#ifdef MODULE_TOUCH_DESIGNER
#include <tdv/WxPreviewCanvas.h>
#endif // MODULE_TOUCH_DESIGNER
#ifdef MODULE_VISION_LAB
#include <visiongraphv/WxPreviewCanvas.h>
#endif // MODULE_VISION_LAB
#ifdef MODULE_MATERIAL
#include <texlab/WxPreviewCanvas.h>
#endif // MODULE_MATERIAL
#ifdef MODULE_EDITOP
#include <editoplab/WxPreviewCanvas.h>
#endif // MODULE_EDITOP
#ifdef MODULE_TASK
#include <tasklab/WxPreviewCanvas.h>
#endif // MODULE_TASK
#ifdef MODULE_VFXGRAPH
#include <vfxlab/VfxLab.h>
#endif // MODULE_VFXGRAPH

#include <boost/filesystem.hpp>

#define PHYSICS_PBD_SCENE

namespace eone
{

WxStagePage* PanelFactory::CreateStagePage(const ur::Device& dev, ECS_WORLD_PARAM int page_type, WxStagePanel* stage_panel)
{
	WxStagePage* page = nullptr;

	auto bb = Blackboard::Instance();
	auto frame = bb->GetFrame();
	auto library = bb->GetLiraryPanel();
	auto& rc = bb->GetRenderContext();
	auto& wc = bb->GetWindowContext();

	switch (page_type)
	{
#ifdef MODULE_SCENE2D
	case PAGE_SCENE2D:
	{
		auto obj = GameObjFactory::Create(ECS_WORLD_VAR GAME_OBJ_COMPLEX2D);
		page = new scene2d::WxStagePage(frame, library, ECS_WORLD_VAR obj);
		auto canvas = std::make_shared<WxStageCanvas2D>(page, ECS_WORLD_VAR rc);

		page->GetImpl().SetCanvas(canvas);

		auto prev_op = CreateNode2DSelectOP(canvas->GetCamera(), *page, rc, wc);

		auto op = std::make_shared<ee2::ArrangeNodeOP>(
			canvas->GetCamera(), *page, ECS_WORLD_VAR ee2::ArrangeNodeCfg(), prev_op);

		page->GetImpl().SetEditOP(op);
	}
		break;
#endif // MODULE_SCENE2D
#ifdef MODULE_SCENE3D
	case PAGE_SCENE3D:
	{
		auto obj = GameObjFactory::Create(ECS_WORLD_VAR GAME_OBJ_COMPLEX3D);
		page = new scene3d::WxStagePage(frame, library, ECS_WORLD_VAR obj);
		auto canvas = std::make_shared<WxStageCanvas3D>(page, rc, true);
		page->GetImpl().SetCanvas(canvas);

		auto op = std::make_shared<ee3::NodeArrangeOP>(
			canvas->GetCamera(), *page, canvas->GetViewport());
		page->GetImpl().SetEditOP(op);
	}
		break;
#endif // MODULE_SCENE3D
#ifdef MODULE_SCALE9
	case PAGE_SCALE9:
	{
		auto obj = GameObjFactory::Create(ECS_WORLD_VAR GAME_OBJ_SCALE9);
		page = new scale9::WxStagePage(frame, library, ECS_WORLD_VAR obj);
		auto canvas = std::make_shared<scale9::WxStageCanvas>(page, ECS_WORLD_VAR rc);
		page->GetImpl().SetCanvas(canvas);

		page->GetImpl().SetEditOP(CreateNode2DSelectOP(canvas->GetCamera(), *page, rc, wc));
	}
		break;
#endif // MODULE_SCALE9
#ifdef MODULE_MASK
	case PAGE_MASK:
	{
		auto obj = GameObjFactory::Create(ECS_WORLD_VAR GAME_OBJ_MASK);
		page = new mask::WxStagePage(frame, library, ECS_WORLD_VAR obj);
		auto canvas = std::make_shared<WxStageCanvas2D>(page, ECS_WORLD_VAR rc);
		page->GetImpl().SetCanvas(canvas);

		auto prev_op = CreateNode2DSelectOP(canvas->GetCamera(), *page, rc, wc);

		auto op = std::make_shared<ee2::ArrangeNodeOP>(
			canvas->GetCamera(), *page, ECS_WORLD_VAR ee2::ArrangeNodeCfg(), prev_op);

		page->GetImpl().SetEditOP(op);
	}
		break;
#endif // MODULE_MASK
#ifdef MODULE_MESH
	case PAGE_MESH:
	{
		auto obj = GameObjFactory::Create(ECS_WORLD_VAR GAME_OBJ_MESH);
		page = new mesh::WxStagePage(frame, library, ECS_WORLD_VAR obj);
		auto canvas = std::make_shared<mesh::WxStageCanvas>(page, rc);
		page->GetImpl().SetCanvas(canvas);
	}
		break;
#endif // MODULE_MESH
#ifdef MODULE_ANIM
	case PAGE_ANIM:
	{
		auto obj = GameObjFactory::Create(ECS_WORLD_VAR GAME_OBJ_ANIM);
		page = new anim::WxStagePage(frame, library, ECS_WORLD_VAR obj);
		auto canvas = std::make_shared<WxStageCanvas2D>(page, ECS_WORLD_VAR rc);
		page->GetImpl().SetCanvas(canvas);

		auto prev_op = CreateNode2DSelectOP(canvas->GetCamera(), *page, rc, wc);

		auto op = std::make_shared<ee2::ArrangeNodeOP>(
			canvas->GetCamera(), *page, ECS_WORLD_VAR ee2::ArrangeNodeCfg(), prev_op);

		page->GetImpl().SetEditOP(op);
	}
		break;
#endif // MODULE_ANIM
#ifdef MODULE_PARTICLE3D
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
	}
		break;
#endif // MODULE_PARTICLE3D
#ifdef MODULE_SHAPE2D
	case PAGE_SHAPE2D:
	{
		auto obj = GameObjFactory::Create(ECS_WORLD_VAR GAME_OBJ_COMPLEX2D);

		page = new shape2d::WxStagePage(frame, library, ECS_WORLD_VAR obj);

		auto canvas = std::make_shared<shape2d::WxStageCanvas>(page, rc);
		page->GetImpl().SetCanvas(canvas);
	}
		break;
#endif // MODULE_SHAPE2D
#ifdef MODULE_SHAPE3D
	case PAGE_SHAPE3D:
	{
		auto obj = GameObjFactory::Create(ECS_WORLD_VAR GAME_OBJ_COMPLEX3D);

		auto shape_page = new shape3d::WxStagePage(frame, library, ECS_WORLD_VAR obj);
		page = shape_page;
		auto canvas = std::make_shared<shape3d::WxStageCanvas>(page, rc, shape_page->GetEditOpMgr());
		page->GetImpl().SetCanvas(canvas);
		shape_page->InitViewports();

		shape_page->InitEditOP(canvas->GetCamera(), canvas->GetViewport());
	}
		break;
#endif // MODULE_SHAPE3D
#ifdef MODULE_CAD
	case PAGE_CAD:
	{
		auto obj = GameObjFactory::Create(ECS_WORLD_VAR GAME_OBJ_COMPLEX2D);

        auto cad_page = new cad::WxStagePage(frame, library, ECS_WORLD_VAR obj);
        page = cad_page;

		auto canvas = std::make_shared<cad::WxStageCanvas>(page, rc, cad_page->GetEditView());
		page->GetImpl().SetCanvas(canvas);
	}
		break;
#endif // MODULE_CAD

#ifdef MODULE_MODEL
	case PAGE_MODEL:
	{
		auto obj = GameObjFactory::Create(ECS_WORLD_VAR GAME_OBJ_MODEL);
		auto mpage = new model::WxStagePage(frame, library, ECS_WORLD_VAR obj);
		page = mpage;
		auto canvas = std::make_shared<model::WxStageCanvas>(mpage, rc);
		mpage->GetImpl().SetCanvas(canvas);
		mpage->InitEditOp(canvas->GetCamera(), canvas->GetViewport());
	}
		break;
#endif // MODULE_MODEL
#ifdef MODULE_ANIM3
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
	}
		break;
#endif // MODULE_ANIM3
#ifdef MODULE_PROTOTYPE
	case PAGE_PROTOTYPING:
	{
		auto obj = GameObjFactory::Create(ECS_WORLD_VAR GAME_OBJ_COMPLEX2D);
		page = new prototype::WxStagePage(frame, library, ECS_WORLD_VAR obj);
		auto canvas = std::make_shared<WxStageCanvas2D>(page, rc);
		page->GetImpl().SetCanvas(canvas);

		auto op = std::make_shared<pt::ArrangeNodeOP>(
			canvas->GetCamera(), *page);
		page->GetImpl().SetEditOP(op);
	}
		break;
#endif // MODULE_PROTOTYPE
#ifdef MODULE_SHADERGRAPH
    case PAGE_SHADER_GRAPH2:
    {
        auto obj = GameObjFactory::Create(ECS_WORLD_VAR GAME_OBJ_COMPLEX2D);
        page = new sgraph::WxStagePage(dev, frame, ECS_WORLD_VAR obj, rc);
    }
        break;
#endif // MODULE_SHADERGRAPH
#ifdef MODULE_RENDERGRAPH
    case PAGE_RENDER_GRAPH:
    {
        auto obj = GameObjFactory::Create(ECS_WORLD_VAR GAME_OBJ_COMPLEX2D);
        page = new rgraph::WxStagePage(dev, frame, ECS_WORLD_VAR obj, rc);
    }
        break;
#endif // MODULE_RENDERGRAPH
#ifdef MODULE_PHYSICS3D
    case PAGE_PHYSICS3D:
    {
        auto obj = GameObjFactory::Create(ECS_WORLD_VAR GAME_OBJ_COMPLEX3D);
        auto p3_page = new physics3d::WxStagePage(frame, library, ECS_WORLD_VAR obj);
        page = p3_page;
#ifdef PHYSICS_PBD_SCENE
        auto canvas = std::make_shared<physics3d::WxStageCanvas2D>(page, rc);
#else
        auto canvas = std::make_shared<physics3d::WxStageCanvas3D>(page, rc);
#endif // PHYSICS_PBD_SCENE
        page->GetImpl().SetCanvas(canvas);

        auto& world = p3_page->GetPhysicsMgr().GetWorld();
#ifdef PHYSICS_PBD_SCENE
        auto op = std::make_shared<physics3d::PBDSceneOP>(
            canvas->GetCamera(), page->GetSubjectMgr());
#else
        auto op = std::make_shared<ee3::DragRigidOP>(
            canvas->GetCamera(), *page, canvas->GetViewport(), world);
#endif // PHYSICS_PBD_SCENE

        page->GetImpl().SetEditOP(op);
    }
        break;
#endif // MODULE_PHYSICS3D
#ifdef MODULE_RAYGRAPH
    case PAGE_RAY_GRAPH:
    {
		auto obj = GameObjFactory::Create(ECS_WORLD_VAR GAME_OBJ_COMPLEX2D);
		page = new raygraph::WxStagePage(frame, ECS_WORLD_VAR obj);
        auto canvas = std::make_shared<WxBlueprintCanvas>(page, rc);
        page->GetImpl().SetCanvas(canvas);

        auto prev_op = std::make_shared<LeftDClickOP>(canvas->GetCamera(), *page, rc, wc);

        auto select_op = std::make_shared<bp::NodeSelectOP>(canvas->GetCamera(), *page);
        select_op->AddPrevEditOP(prev_op);

		ee2::ArrangeNodeCfg cfg;
		cfg.is_auto_align_open = false;
		cfg.is_deform_open = false;
		cfg.is_offset_open = false;
		cfg.is_rotate_open = false;
		auto arrange_op = std::make_shared<bp::ArrangeNodeOP>(
			canvas->GetCamera(), *page, ECS_WORLD_VAR cfg, select_op);

        auto& nodes = raylab::RayLab::Instance()->GetAllNodes();
		auto op = std::make_shared<bp::ConnectPinOP>(canvas->GetCamera(), *page, nodes);
		op->SetPrevEditOP(arrange_op);
		page->GetImpl().SetEditOP(op);
    }
        break;
#endif // MODULE_RAYGRAPH
#ifdef MODULE_GIGRAPH
	case PAGE_GI_GRAPH:
	{
		auto obj = GameObjFactory::Create(ECS_WORLD_VAR GAME_OBJ_COMPLEX2D);
		page = new gi::WxStagePage(dev, frame, ECS_WORLD_VAR obj, rc);
	}
	break;
#endif // MODULE_GIGRAPH
#ifdef MODULE_GUIGRAPH
    case PAGE_GUI_GRAPH:
    {
		auto obj = GameObjFactory::Create(ECS_WORLD_VAR GAME_OBJ_COMPLEX2D);
        page = new guigraph::WxStagePage(frame, ECS_WORLD_VAR obj);
		auto canvas = std::make_shared<guigraph::WxStageCanvas>(page, ECS_WORLD_VAR rc);

		page->GetImpl().SetCanvas(canvas);

		auto prev_op = CreateNode2DSelectOP(canvas->GetCamera(), *page, rc, wc);

		auto op = std::make_shared<ee2::ArrangeNodeOP>(
			canvas->GetCamera(), *page, ECS_WORLD_VAR ee2::ArrangeNodeCfg(), prev_op);

		page->GetImpl().SetEditOP(op);
    }
        break;
#endif // MODULE_GUIGRAPH
#ifdef MODULE_PBRGRAPH
    case PAGE_PBR_GRAPH:
    {
		auto obj = GameObjFactory::Create(ECS_WORLD_VAR GAME_OBJ_COMPLEX2D);
		page = new pbrgraph::WxStagePage(frame, ECS_WORLD_VAR obj);
        auto canvas = std::make_shared<WxBlueprintCanvas>(page, rc);
        page->GetImpl().SetCanvas(canvas);

        auto prev_op = std::make_shared<LeftDClickOP>(canvas->GetCamera(), *page, rc, wc);

        auto select_op = std::make_shared<bp::NodeSelectOP>(canvas->GetCamera(), *page);
        select_op->AddPrevEditOP(prev_op);

		ee2::ArrangeNodeCfg cfg;
		cfg.is_auto_align_open = false;
		cfg.is_deform_open = false;
		cfg.is_offset_open = false;
		cfg.is_rotate_open = false;
		auto arrange_op = std::make_shared<bp::ArrangeNodeOP>(
			canvas->GetCamera(), *page, ECS_WORLD_VAR cfg, select_op);

        auto& nodes = renderlab::RenderLab::Instance()->GetAllNodes();
		auto op = std::make_shared<bp::ConnectPinOP>(canvas->GetCamera(), *page, nodes);
		op->SetPrevEditOP(arrange_op);
		page->GetImpl().SetEditOP(op);
    }
        break;
#endif // MODULE_PBRGRAPH
#ifdef MODULE_HDI_SOP
    case PAGE_HDI_SOP:
    {
        auto obj = GameObjFactory::Create(ECS_WORLD_VAR GAME_OBJ_COMPLEX2D);
        page = new hdi_sop::WxStagePage(frame, ECS_WORLD_VAR obj);
        auto canvas = std::make_shared<sopv::WxStageCanvas>(page, ECS_WORLD_VAR rc);

        page->GetImpl().SetCanvas(canvas);

        auto op = std::make_shared<ee3::NodeArrangeOP>(
            canvas->GetCamera(), *page, canvas->GetViewport()
        );

        page->GetImpl().SetEditOP(op);
    }
        break;
#endif // MODULE_HDI_SOP
#ifdef MODULE_HDI_VOP
    case PAGE_HDI_VOP:
    {
        auto obj = GameObjFactory::Create(ECS_WORLD_VAR GAME_OBJ_COMPLEX2D);
        page = new hdi_vop::WxStagePage(frame, ECS_WORLD_VAR obj);
        auto canvas = std::make_shared<vopv::WxStageCanvas>(page, ECS_WORLD_VAR rc);

        page->GetImpl().SetCanvas(canvas);

        auto op = std::make_shared<ee3::NodeArrangeOP>(
            canvas->GetCamera(), *page, canvas->GetViewport()
        );

        page->GetImpl().SetEditOP(op);
    }
        break;
#endif // MODULE_HDI_VOP
#ifdef MODULE_ARCHITECTURAL
    case PAGE_ARCHITECTURAL:
    {
        auto obj = GameObjFactory::Create(ECS_WORLD_VAR GAME_OBJ_COMPLEX2D);

        auto city_stage = new architectural::WxStagePage(frame, ECS_WORLD_VAR obj);
        page = city_stage;
        auto canvas = std::make_shared<archlab::WxPreviewCanvas>(page, ECS_WORLD_VAR rc);

        page->GetImpl().SetCanvas(canvas);
        city_stage->InitEditOP();
    }
        break;
#endif // MODULE_ARCHITECTURAL
#ifdef MODULE_TERRAIN
    case PAGE_TERRAIN:
    {
        auto obj = GameObjFactory::Create(ECS_WORLD_VAR GAME_OBJ_COMPLEX2D);
		terrainlab::TerrainLab::Instance();	// init
        page = new terrain::WxStagePage(dev, frame, ECS_WORLD_VAR obj, rc);
    }
        break;
#endif // MODULE_TERRAIN
#ifdef MODULE_CITY
    case PAGE_CITY:
    {
        auto obj = GameObjFactory::Create(ECS_WORLD_VAR GAME_OBJ_COMPLEX2D);
        page = new city::WxStagePage(frame, ECS_WORLD_VAR obj, rc);
    }
        break;
#endif // MODULE_CITY
#ifdef MODULE_GEOMETRY
    case PAGE_GEOMETRY:
    {
        auto obj = GameObjFactory::Create(ECS_WORLD_VAR GAME_OBJ_COMPLEX2D);
        page = new geometry::WxStagePage(frame, ECS_WORLD_VAR obj);
        auto canvas = std::make_shared<geolab::WxPreviewCanvas>(page, ECS_WORLD_VAR rc);

        page->GetImpl().SetCanvas(canvas);

        auto op = std::make_shared<ee3::NodeArrangeOP>(
            canvas->GetCamera(), *page, canvas->GetViewport()
        );

        page->GetImpl().SetEditOP(op);
    }
        break;
#endif // MODULE_GEOMETRY
#ifdef MODULE_TOUCH_DESIGNER
    case PAGE_TOUCH_DESIGNER:
    {
        auto obj = GameObjFactory::Create(ECS_WORLD_VAR GAME_OBJ_COMPLEX2D);
        page = new touchdesigner::WxStagePage(frame, ECS_WORLD_VAR obj);
        auto canvas = std::make_shared<tdv::WxPreviewCanvas>(page, ECS_WORLD_VAR rc);

        page->GetImpl().SetCanvas(canvas);

        auto op = std::make_shared<ee3::NodeArrangeOP>(
            canvas->GetCamera(), *page, canvas->GetViewport()
        );

        page->GetImpl().SetEditOP(op);
    }
        break;
#endif // MODULE_TOUCH_DESIGNER
#ifdef MODULE_VISION_LAB
    case PAGE_VISION_LAB:
    {
        auto obj = GameObjFactory::Create(ECS_WORLD_VAR GAME_OBJ_COMPLEX2D);
        page = new visionlab::WxStagePage(frame, ECS_WORLD_VAR obj);
        auto canvas = std::make_shared<vgv::WxPreviewCanvas>(page, ECS_WORLD_VAR rc);

        page->GetImpl().SetCanvas(canvas);

        auto op = std::make_shared<ee3::NodeArrangeOP>(
            canvas->GetCamera(), *page, canvas->GetViewport()
        );

        page->GetImpl().SetEditOP(op);
    }
        break;
#endif // MODULE_VISION_LAB
#ifdef MODULE_MATERIAL
    case PAGE_MATERIAL:
    {
        auto obj = GameObjFactory::Create(ECS_WORLD_VAR GAME_OBJ_COMPLEX2D);
        page = new material::WxStagePage(frame, ECS_WORLD_VAR obj);
        auto canvas = std::make_shared<texlab::WxPreviewCanvas>(page, ECS_WORLD_VAR rc);

        page->GetImpl().SetCanvas(canvas);

        auto op = std::make_shared<ee3::NodeArrangeOP>(
            canvas->GetCamera(), *page, canvas->GetViewport()
        );

        page->GetImpl().SetEditOP(op);
    }
        break;
#endif // MODULE_MATERIAL
#ifdef MODULE_EDITOP
    case PAGE_EDITOP:
    {
        auto obj = GameObjFactory::Create(ECS_WORLD_VAR GAME_OBJ_COMPLEX2D);
        page = new editop::WxStagePage(frame, ECS_WORLD_VAR obj, rc);
    }
    break;
#endif // MODULE_EDITOP
#ifdef MODULE_TASK
    case PAGE_TASK:
    {
        auto obj = GameObjFactory::Create(ECS_WORLD_VAR GAME_OBJ_COMPLEX2D);
        page = new task::WxStagePage(frame, ECS_WORLD_VAR obj, rc);
    }
    break;
#endif // MODULE_TASK
#ifdef MODULE_VFXGRAPH
    case PAGE_VFX_GRAPH:
    {
        auto obj = GameObjFactory::Create(ECS_WORLD_VAR GAME_OBJ_COMPLEX2D);
        page = new vfx::WxStagePage(dev, frame, ECS_WORLD_VAR obj, rc);
    }
        break;
#endif // MODULE_VFXGRAPH

#ifdef MODULE_SCRIPT
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

		auto prev_op = CreateNode2DSelectOP(canvas->GetCamera(), *page, rc, wc);

		auto op = std::make_shared<ee2::ArrangeNodeOP>(
			canvas->GetCamera(), *page, ECS_WORLD_VAR ee2::ArrangeNodeCfg(), prev_op);

		page->GetImpl().SetEditOP(op);
	}
		break;
#endif // MODULE_SCRIPT

#ifdef MODULE_BLUEPRINT
	case PAGE_BLUEPRINT:
	{
		auto obj = GameObjFactory::Create(ECS_WORLD_VAR GAME_OBJ_COMPLEX2D);
		page = new bprint::WxStagePage(frame, library, ECS_WORLD_VAR obj);
		auto canvas = std::make_shared<WxStageCanvas2D>(page, ECS_WORLD_VAR rc);
		page->GetImpl().SetCanvas(canvas);

		auto select_op = CreateNode2DSelectOP(canvas->GetCamera(), *page, rc, wc);

		ee2::ArrangeNodeCfg cfg;
		cfg.is_auto_align_open = false;
		cfg.is_deform_open     = false;
		cfg.is_offset_open     = false;
		cfg.is_rotate_open     = false;
		auto arrange_op = std::make_shared<bp::ArrangeNodeOP>(
			canvas->GetCamera(), *page, ECS_WORLD_VAR cfg, select_op);

		auto op = std::make_shared<bp::ConnectPinOP>(
			canvas->GetCamera(), *page, shaderlab::ShaderLab::Instance()->GetAllNodes()
		);
		op->SetPrevEditOP(arrange_op);
		page->GetImpl().SetEditOP(op);
	}
		break;
#endif // MODULE_BLUEPRINT

#ifdef MODULE_QUAKE
	case PAGE_QUAKE:
	{
		auto obj = GameObjFactory::Create(ECS_WORLD_VAR GAME_OBJ_COMPLEX3D);
		auto quake_page = new quake::WxStagePage(frame, library, ECS_WORLD_VAR obj);
		page = quake_page;
		auto canvas = std::make_shared<quake::WxStageCanvas>(page, rc, quake_page->GetEditOpMgr());
		page->GetImpl().SetCanvas(canvas);
		quake_page->InitViewports();

		quake_page->InitEditOP(canvas->GetCamera(), canvas->GetViewport());
	}
		break;
#endif // MODULE_QUAKE
	}

	return page;
}

void PanelFactory::CreatePreviewOP(
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
#ifdef MODULE_SCENE2D
	case PAGE_SCENE2D:
	{
		auto preview_op = std::make_shared<ee2::CamControlOP>(
			canvas->GetCamera(), sub_mgr);
		preview->GetImpl().SetEditOP(preview_op);
	}
		break;
#endif // MODULE_SCENE2D
#ifdef MODULE_SCENE3D
	case PAGE_SCENE3D:
	{
		//auto preview_op = std::make_shared<ee3::WorldTravelOP>(
		//	canvas->GetCamera(), canvas->GetViewport(), page->GetSubjectMgr());
		//preview->GetImpl().SetEditOP(preview_op);
	}
		break;
#endif // MODULE_SCENE3D
#ifdef MODULE_SCALE9
	case PAGE_SCALE9:
	{
		auto preview_op = std::make_shared<scale9::ResizeScale9OP>(
			canvas->GetCamera(), preview, ECS_WORLD_VAR curr_page->GetEditedObj());
		preview->GetImpl().SetEditOP(preview_op);
	}
		break;
#endif // MODULE_SCALE9
#ifdef MODULE_MASK
	case PAGE_MASK:
	{
		auto preview_op = std::make_shared<ee2::CamControlOP>(
			canvas->GetCamera(), sub_mgr);
		preview->GetImpl().SetEditOP(preview_op);
	}
		break;
#endif // MODULE_MASK
#ifdef MODULE_MESH
	case PAGE_MESH:
	{
		auto preview_op = std::make_shared<ee2::CamControlOP>(
			canvas->GetCamera(), sub_mgr);
		preview->GetImpl().SetEditOP(preview_op);
	}
		break;
#endif // MODULE_MESH
	}
}

ee0::EditOPPtr PanelFactory::CreateNode2DSelectOP(const std::shared_ptr<pt0::Camera>& camera, ee0::WxStagePage& stage,
                                                  const ee0::RenderContext& rc, const ee0::WindowContext& wc)
{
    auto prev_op = std::make_shared<LeftDClickOP>(camera, stage, rc, wc);

    auto op = std::make_shared<ee2::NodeSelectOP>(camera, stage);
    op->AddPrevEditOP(prev_op);

    return op;
}

}