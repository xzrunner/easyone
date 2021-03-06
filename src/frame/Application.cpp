#include "frame/Application.h"
#include "frame/WxLibraryPanel.h"
#include "frame/WxRecordPanel.h"
#include "frame/WxStagePanel.h"
#include "frame/WxStagePage.h"
#include "frame/WxStageSubPanel.h"
#include "frame/WxPreviewPanel.h"
#include "frame/WxPreviewCanvas.h"
#include "frame/WxWorldPanel.h"
#include "frame/WxDetailPanel.h"
#include "frame/Blackboard.h"
#include "frame/GameObjFactory.h"
#include "frame/StagePageType.h"
#include "frame/PanelFactory.h"
#include "frame/Blackboard.h"
#include "frame/typedef.h"
#include "frame/WxScriptPanel.h"
#include "frame/config.h"

#ifdef MODULE_SCENE2D
#include "scene2d/WxStagePage.h"
#endif // MODULE_SCENE2D
#ifdef MODULE_SCENE3D
#include "scene3d/WxStagePage.h"
#endif // MODULE_SCENE3D
#ifdef MODULE_SHAPE2D
#include "shape2d/WxStagePage.h"
#endif // MODULE_SHAPE2D
#ifdef MODULE_SHAPE3D
#include "shape3d/WxStagePage.h"
#endif MODULE_SHAPE3D
#ifdef MODULE_CAD
#include "cad/WxStagePage.h"
#endif MODULE_CAD
#ifdef MODULE_QUAKE
#include "quake/WxStagePage.h"
#endif // MODULE_QUAKE
#ifdef MODULE_PROTOTYPE
#include "prototype/WxStagePage.h"
#endif // MODULE_PROTOTYPE
#ifdef MODULE_SHADERGRAPH
#include "sgraph/WxStagePage.h"
#endif // MODULE_SHADERGRAPH
#ifdef MODULE_RENDERGRAPH
#include "rgraph/WxStagePage.h"
#endif // MODULE_RENDERGRAPH
#include "physics3d/WxStagePage.h"
#ifdef MODULE_RAYGRAPH
#include "raygraph/WxStagePage.h"
#endif // MODULE_RAYGRAPH
#ifdef MODULE_GIGRAPH
#include "gi/WxStagePage.h"
#endif // MODULE_GIGRAPH
#ifdef MODULE_GUIGRAPH
#include "guigraph/WxStagePage.h"
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
#ifdef MODULE_HDI_SOP
#include "hdi_sop/WxStagePage.h"
#endif // MODULE_HDI_SOP
#ifdef MODULE_ARCHITECTURAL
#include "architectural/WxStagePage.h"
#endif // MOUDLE_CITY
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

#include <ee0/MsgHelper.h>
#include <ee0/SubjectMgr.h>
#include <ee0/ConfigFile.h>
#include <ee2/WxStageCanvas.h>
#include <ee3/WxStageCanvas.h>
#include <ee3/NodeArrangeOP.h>
#include <ee3/Serializer.h>

#include <js/RapidJsonHelper.h>
#ifndef GAME_OBJ_ECS
#include <node0/SceneNode.h>
#include <ns/RegistCallback.h>
#include <ns/Blackboard.h>
#else
#include <entity2/CompTransform.h>
#include <entity2/CompBoundingBox.h>
#include <entity2/CompComplex.h>
#endif // GAME_OBJ_ECS
#include <facade/Facade.h>
#include <facade/GTxt.h>
#include <moon/moon.h>
#include <sx/ResFileHelper.h>

#include <boost/filesystem.hpp>

namespace
{

extern "C" int luaopen_moon_bp(lua_State* L);
//extern "C" int luaopen_moon_shaderlab(lua_State* L);

}

namespace eone
{

Application::Application(wxFrame* frame)
	: m_frame(frame)
	, m_mgr(frame)
	, m_stage(nullptr)
{
	Blackboard::Instance()->InitRenderContext();
	InitSubmodule();
#ifdef GAME_OBJ_ECS
	InitWorld();
#endif // GAME_OBJ_ECS
	InitLayout();
	InitCallback();
}

void Application::LoadFromFile(const std::string& filepath)
{
	if (m_stage->SwitchToPage(filepath)) {
		return;
	}

	auto page = m_stage->GetCurrentStagePage();
	int old_type = page->GetPageType();

	int new_type = PAGE_INVALID;

	auto type = sx::ResFileHelper::Type(filepath);
	switch (type)
	{
	case sx::RES_FILE_JSON:
	{
		rapidjson::Document doc;
		js::RapidJsonHelper::ReadFromFile(filepath.c_str(), doc);

		std::string new_type_str = doc[ns::CompSerializer::COMP_TYPE_NAME].GetString();
		if (new_type_str == "n0_complex") {
			assert(doc.HasMember("page_type"));
			auto type = doc["page_type"].GetString();
            if (type == "") {
                ;
#ifdef MODULE_SCENE2D
            } else if (type == scene2d::WxStagePage::PAGE_TYPE) {
				new_type = PAGE_SCENE2D;
#endif // MODULE_SCENE2D
#ifdef MODULE_SCENE3D
			} else if (type == scene3d::WxStagePage::PAGE_TYPE) {
				new_type = PAGE_SCENE3D;
#endif // MODULE_SCENE3D
#ifdef MODULE_SHAPE2D
			} else if (type == shape2d::WxStagePage::PAGE_TYPE) {
				new_type = PAGE_SHAPE2D;
#endif // MODULE_SHAPE2D
#ifdef MODULE_SHAPE3D
			} else if (type == shape3d::WxStagePage::PAGE_TYPE) {
				new_type = PAGE_SHAPE3D;
#endif // MODULE_SHAPE3D
#ifdef MODULE_CAD
			} else if (type == cad::WxStagePage::PAGE_TYPE) {
				new_type = PAGE_CAD;
#endif // MODULE_CAD
#ifdef MODULE_QUAKE
			} else if (type == quake::WxStagePage::PAGE_TYPE) {
				new_type = PAGE_QUAKE;
#endif // MODULE_QUAKE
#ifdef MODULE_PROTOTYPE
			} else if (type == prototype::WxStagePage::PAGE_TYPE) {
				new_type = PAGE_PROTOTYPING;
#endif // MODULE_PROTOTYPE
#ifdef MODULE_SHADERGRAPH
			} else if (type == sgraph::WxStagePage::PAGE_TYPE) {
                new_type = PAGE_SHADER_GRAPH2;
#endif // MODULE_SHADERGRAPH
#ifdef MODULE_RENDERGRAPH
			} else if (type == rgraph::WxStagePage::PAGE_TYPE) {
                new_type = PAGE_RENDER_GRAPH;
#endif // MODULE_RENDERGRAPH
#ifdef MODULE_PHYSICS3D
            } else if (type == physics3d::WxStagePage::PAGE_TYPE) {
                new_type = PAGE_PHYSICS3D;
#endif // MODULE_PHYSICS3D
#ifdef MODULE_RAYGRAPH
            } else if (type == raygraph::WxStagePage::PAGE_TYPE) {
                new_type = PAGE_RAY_GRAPH;
#endif // MODULE_RAYGRAPH
#ifdef MODULE_GIGRAPH
            } else if (type == gi::WxStagePage::PAGE_TYPE) {
                new_type = PAGE_GI_GRAPH;
#endif // MODULE_GIGRAPH
#ifdef MODULE_GUIGRAPH
            } else if (type == guigraph::WxStagePage::PAGE_TYPE) {
                new_type = PAGE_GUI_GRAPH;
#endif // MODULE_GUIGRAPH
#ifdef MODULE_PBRGRAPH
            } else if (type == pbrgraph::WxStagePage::PAGE_TYPE) {
                new_type = PAGE_PBR_GRAPH;
#endif // MODULE_PBRGRAPH
#ifdef MODULE_HDI_SOP
            } else if (type == hdi_sop::WxStagePage::PAGE_TYPE) {
                new_type = PAGE_HDI_SOP;
#endif // MODULE_HDI_SOP
#ifdef MODULE_HDI_VOP
            } else if (type == hdi_vop::WxStagePage::PAGE_TYPE) {
                new_type = PAGE_HDI_VOP;
#endif // MODULE_HDI_VOP
#ifdef MODULE_ARCHITECTURAL
            } else if (type == architectural::WxStagePage::PAGE_TYPE) {
                new_type = PAGE_ARCHITECTURAL;
#endif // MODULE_ARCHITECTURAL
#ifdef MODULE_TERRAIN
            } else if (type == terrain::WxStagePage::PAGE_TYPE) {
                new_type = PAGE_TERRAIN;
#endif // MODULE_TERRAIN
#ifdef MODULE_CITY
            } else if (type == city::WxStagePage::PAGE_TYPE) {
                new_type = PAGE_CITY;
#endif // MODULE_CITY
#ifdef MODULE_GEOMETRY
            } else if (type == geometry::WxStagePage::PAGE_TYPE) {
                new_type = PAGE_GEOMETRY;
#endif // MODULE_GEOMETRY
#ifdef MODULE_TOUCH_DESIGNER
            } else if (type == touchdesigner::WxStagePage::PAGE_TYPE) {
                new_type = PAGE_TOUCH_DESIGNER;
#endif // MODULE_TOUCH_DESIGNER
#ifdef MODULE_VISION_LAB
            } else if (type == visionlab::WxStagePage::PAGE_TYPE) {
                new_type = PAGE_VISION_LAB;
#endif // MODULE_VISION_LAB
#ifdef MODULE_MATERIAL
            } else if (type == material::WxStagePage::PAGE_TYPE) {
                new_type = PAGE_MATERIAL;
#endif // MODULE_MATERIAL
#ifdef MODULE_EDITOP
            } else if (type == editop::WxStagePage::PAGE_TYPE) {
                new_type = PAGE_EDITOP;
#endif // MODULE_EDITOP
#ifdef MODULE_TASK
            } else if (type == task::WxStagePage::PAGE_TYPE) {
                new_type = PAGE_TASK;
#endif // MODULE_TASK
#ifdef MODULE_VFXGRAPH
            } else if (type == vfx::WxStagePage::PAGE_TYPE) {
                new_type = PAGE_VFX_GRAPH;
#endif // MODULE_VFXGRAPH
            }
		} else if (new_type_str == "n2_scale9") {
			new_type = PAGE_SCALE9;
		} else if (new_type_str == "n2_mask") {
			new_type = PAGE_MASK;
		} else if (new_type_str == "n2_mesh") {
			new_type = PAGE_MESH;
		} else if (new_type_str == "n2_anim") {
			new_type = PAGE_ANIM;
		} else if (new_type_str == "n2_particle3d") {
			new_type = PAGE_PARTICLE3D;
		}
	}
		break;
	case sx::RES_FILE_MODEL:
		if (old_type == PAGE_MODEL) {
			new_type = PAGE_MODEL;
		} else if (old_type == PAGE_ANIM3) {
			new_type = PAGE_ANIM3;
		} else {
			new_type = PAGE_MODEL;
		}
		break;
    case sx::RES_FILE_SHADER:
    case sx::RES_FILE_ASSET:
        new_type = PAGE_SHADER_GRAPH;
        break;
    case sx::RES_FILE_PYTHON:
        // todo
        new_type = PAGE_HDI_SOP;
        break;
	case sx::RES_FILE_MAP:
		new_type = PAGE_QUAKE;
		break;
	}

	if (old_type != new_type)
    {
        auto dev = Blackboard::Instance()->GetRenderDevice();
		page = PanelFactory::CreateStagePage(*dev, ECS_WORLD_SELF_VAR new_type, m_stage);
        m_stage->AddNewPage(page, GetPageName(page->GetPageType()));
		page->GetSubjectMgr()->NotifyObservers(ee0::MSG_STAGE_PAGE_ON_SHOW);
	}

	page->SetFilepath(filepath);

    auto dev = Blackboard::Instance()->GetRenderDevice();
	page->LoadFromFile(*dev, filepath);

	m_frame->SetTitle(filepath);
}

void Application::StoreToFile(const std::string& filepath) const
{
	auto path = m_stage->StoreCurrPage(filepath);
	GD_ASSERT(!path.empty(), "err path");
	m_frame->SetTitle(path);
}

void Application::Clear()
{
	auto& sub_mgr = m_stage->GetCurrentStagePage()->GetSubjectMgr();
	sub_mgr->NotifyObservers(ee0::MSG_NODE_SELECTION_CLEAR);
	sub_mgr->NotifyObservers(ee0::MSG_SCENE_NODE_CLEAR);
}

void Application::InitSubmodule()
{
    auto dev = Blackboard::Instance()->GetRenderDevice();

	facade::Facade::Instance()->AddInitCB([] {
		moon_add_module("moon.bp", luaopen_moon_bp);
		//moon_add_module("moon.sg", luaopen_moon_shaderlab);
	});
	facade::Facade::Instance()->Init(*dev);

	auto cfg = ee0::ConfigFile::Instance();
	facade::GTxt::Instance()->LoadFonts(cfg->GetFonts(), cfg->GetUserFonts());

	//gum::Facade::Initialize();
}

void Application::InitLayout()
{
	auto library   = CreateLibraryPanel();
	auto stage     = CreateStagePanel();
	auto stage_ext = CreateStageExtPanel();
	auto preview   = CreatePreviewPanel();
	auto world     = CreateWorldPanel();
	auto detail    = CreateDetailPanel();
	auto toolbar   = CreateToolbarPanel();
	auto record    = CreateRecordPanel();
	auto script    = CreateScriptPanel();

	m_mgr.AddPane(library, wxAuiPaneInfo().Name(STR_LIBRARY_PANEL).
		Caption(STR_LIBRARY_PANEL).Left().MinSize(150, 0));
	m_mgr.AddPane(record, wxAuiPaneInfo().Name(STR_RECORD_PANEL).
		Caption(STR_RECORD_PANEL).Left());

	//m_mgr.AddPane(stage, wxAuiPaneInfo().Name(STR_STAGE_PANEL).
	//	Caption(STR_STAGE_PANEL).CenterPane().Row(1).PaneBorder(false));
	m_mgr.AddPane(stage, wxAuiPaneInfo().Name(STR_STAGE_PANEL).
		Caption(STR_STAGE_PANEL).CenterPane().PaneBorder(false));

	m_mgr.AddPane(preview, wxAuiPaneInfo().Name(STR_PREVIEW_PANEL).
		Caption(STR_PREVIEW_PANEL).CenterPane().PaneBorder(false));

	m_mgr.AddPane(world, wxAuiPaneInfo().Name(STR_WORLD_PANEL).
		Caption(STR_WORLD_PANEL).Right().Row(1).MinSize(200, 0).PaneBorder(false));

	m_mgr.AddPane(detail, wxAuiPaneInfo().Name(STR_DETAIL_PANEL).
		Caption(STR_DETAIL_PANEL).Right().Row(0).MinSize(300, 0).PaneBorder(false));

	m_mgr.AddPane(script, wxAuiPaneInfo().Name(STR_SCRIPT_PANEL).
		Caption(STR_SCRIPT_PANEL).Right().Row(0).MinSize(300, 0).PaneBorder(false));

	m_mgr.AddPane(toolbar, wxAuiPaneInfo().Name(STR_TOOLBAR_PANEL).
		Caption(STR_TOOLBAR_PANEL).Right().Row(0).MinSize(300, 0).PaneBorder(false));

    m_mgr.AddPane(stage_ext, wxAuiPaneInfo().Name(STR_STAGE_EXT_PANEL).
        Caption(STR_STAGE_EXT_PANEL).CenterPane().PaneBorder(false));

	m_mgr.Update();
}

void Application::InitCallback()
{
#ifndef GAME_OBJ_ECS
	ns::RegistCallback::Init();
	ns::Blackboard::Instance()->SetGenNodeIdFunc([]()->uint32_t {
		auto stage = static_cast<WxStagePage*>(
			Blackboard::Instance()->GetStagePanel()->GetCurrentPage());
		return stage->FetchObjID();
	});
#endif // GAME_OBJ_ECS
}

#ifdef GAME_OBJ_ECS
void Application::InitWorld()
{
	m_world.SetCompStorage<e2::CompPosition>(e0::COMP_STORAGE_DENSE);
	m_world.SetCompStorage<e2::CompAngle>(e0::COMP_STORAGE_DENSE);
	m_world.SetCompStorage<e2::CompScale>(e0::COMP_STORAGE_DENSE);
	m_world.SetCompStorage<e2::CompShear>(e0::COMP_STORAGE_DENSE);
	m_world.SetCompStorage<e2::CompOffset>(e0::COMP_STORAGE_DENSE);
	m_world.SetCompStorage<e2::CompLocalMat>(e0::COMP_STORAGE_DENSE);

	m_world.SetCompStorage<e2::CompBoundingBox>(e0::COMP_STORAGE_DENSE);
	m_world.SetCompStorage<ee0::CompEntityEditor>(e0::COMP_STORAGE_DENSE);

	m_world.SetCompStorage<e2::CompComplex>(e0::COMP_STORAGE_SPARSE);
}
#endif // GAME_OBJ_ECS

wxWindow* Application::CreateLibraryPanel()
{
	auto library = new WxLibraryPanel(m_frame);
	Blackboard::Instance()->SetLiraryPanel(library);
	return library;
}

wxWindow* Application::CreateRecordPanel()
{
	auto& sub_mgr = m_stage->GetCurrentStagePage()->GetSubjectMgr();
	return new WxRecordPanel(m_frame, sub_mgr);
}

wxWindow* Application::CreateStagePanel()
{
	m_stage = new WxStagePanel(m_frame);
	m_stage->Freeze();
	Blackboard::Instance()->SetStagePanel(m_stage);

    WxStagePage* page = nullptr;

    auto dev = Blackboard::Instance()->GetRenderDevice();

#ifdef MODULE_SCENE2D
	page = PanelFactory::CreateStagePage(ECS_WORLD_SELF_VAR PAGE_SCENE2D, m_stage);
#endif // MODULE_SCENE2D
#ifdef MODULE_SCENE3D
    page = PanelFactory::CreateStagePage(ECS_WORLD_SELF_VAR PAGE_SCENE3D, m_stage);
#endif // MODULE_SCENE3D

#ifdef MODULE_SCALE9
	page = PanelFactory::CreateStagePage(ECS_WORLD_SELF_VAR PAGE_SCALE9, m_stage);
#endif // MODULE_SCALE9
#ifdef MODULE_SCRIPT
	page = PanelFactory::CreateStagePage(ECS_WORLD_SELF_VAR PAGE_SCRIPT, m_stage);
#endif // MODULE_SCRIPT
#ifdef MODULE_ANIM
	page = PanelFactory::CreateStagePage(ECS_WORLD_SELF_VAR PAGE_ANIM, m_stage);
#endif // MODULE_ANIM
#ifdef MODULE_PARTICLE3D
	page = PanelFactory::CreateStagePage(ECS_WORLD_SELF_VAR PAGE_PARTICLE3D, m_stage);
#endif // MODULE_PARTICLE3D
#ifdef MODULE_SHAPE2D
	page = PanelFactory::CreateStagePage(ECS_WORLD_SELF_VAR PAGE_SHAPE2D, m_stage);
#endif // MODULE_SHAPE2D
#ifdef MODULE_SHAPE3D
	page = PanelFactory::CreateStagePage(ECS_WORLD_SELF_VAR PAGE_SHAPE3D, m_stage);
#endif // MODULE_SHAPE3D
#ifdef MODULE_MESH
	page = PanelFactory::CreateStagePage(ECS_WORLD_SELF_VAR PAGE_MESH, m_stage);
#endif // MODULE_MESH
#ifdef MODULE_CAD
	page = PanelFactory::CreateStagePage(ECS_WORLD_SELF_VAR PAGE_CAD, m_stage);
#endif // MODULE_CAD

#ifdef MODULE_MODEL
	page = PanelFactory::CreateStagePage(ECS_WORLD_SELF_VAR PAGE_MODEL, m_stage);
#endif // MODULE_MODEL
#ifdef MODULE_ANIM3
	page = PanelFactory::CreateStagePage(ECS_WORLD_SELF_VAR PAGE_ANIM3, m_stage);
#endif // MODULE_ANIM3
#ifdef MODULE_PROTOTYPE
	page = PanelFactory::CreateStagePage(ECS_WORLD_SELF_VAR PAGE_PROTOTYPING, m_stage);
#endif // MODULE_PROTOTYPE
#ifdef MODULE_SHADERGRAPH
    page = PanelFactory::CreateStagePage(*dev, ECS_WORLD_SELF_VAR PAGE_SHADER_GRAPH2, m_stage);
#endif // MODULE_SHADERGRAPH
#ifdef MODULE_RENDERGRAPH
    page = PanelFactory::CreateStagePage(*dev, ECS_WORLD_SELF_VAR PAGE_RENDER_GRAPH, m_stage);
#endif // MODULE_RENDERGRAPH
#ifdef MODULE_PHYSICS3D
    page = PanelFactory::CreateStagePage(ECS_WORLD_SELF_VAR PAGE_PHYSICS3D, m_stage);
#endif // MODULE_PHYSICS3D
#ifdef MODULE_RAYGRAPH
    page = PanelFactory::CreateStagePage(ECS_WORLD_SELF_VAR PAGE_RAY_GRAPH, m_stage);
#endif // MODULE_RAYGRAPH
#ifdef MODULE_GIGRAPH
    page = PanelFactory::CreateStagePage(*dev, ECS_WORLD_SELF_VAR PAGE_GI_GRAPH, m_stage);
#endif // MODULE_GIGRAPH
#ifdef MODULE_GUIGRAPH
    page = PanelFactory::CreateStagePage(ECS_WORLD_SELF_VAR PAGE_GUI_GRAPH, m_stage);
#endif // MODULE_GUIGRAPH
#ifdef MODULE_PBRGRAPH
    page = PanelFactory::CreateStagePage(ECS_WORLD_SELF_VAR PAGE_PBR_GRAPH, m_stage);
#endif // MODULE_PBRGRAPH
#ifdef MODULE_HDI_SOP
    page = PanelFactory::CreateStagePage(ECS_WORLD_SELF_VAR PAGE_HDI_SOP, m_stage);
#endif // MODULE_HDI_SOP
#ifdef MODULE_HDI_VOP
    page = PanelFactory::CreateStagePage(ECS_WORLD_SELF_VAR PAGE_HDI_VOP, m_stage);
#endif // MODULE_HDI_VOP
#ifdef MODULE_ARCHITECTURAL
    page = PanelFactory::CreateStagePage(ECS_WORLD_SELF_VAR PAGE_ARCHITECTURAL, m_stage);
#endif // MODULE_ARCHITECTURAL
#ifdef MODULE_TERRAIN
    page = PanelFactory::CreateStagePage(*dev, ECS_WORLD_SELF_VAR PAGE_TERRAIN, m_stage);
#endif // MODULE_TERRAIN
#ifdef MODULE_CITY
    page = PanelFactory::CreateStagePage(ECS_WORLD_SELF_VAR PAGE_CITY, m_stage);
#endif // MODULE_CITY
#ifdef MODULE_GEOMETRY
    page = PanelFactory::CreateStagePage(ECS_WORLD_SELF_VAR PAGE_GEOMETRY, m_stage);
#endif // MODULE_GEOMETRY
#ifdef MODULE_TOUCH_DESIGNER
    page = PanelFactory::CreateStagePage(ECS_WORLD_SELF_VAR PAGE_TOUCH_DESIGNER, m_stage);
#endif // MODULE_TOUCH_DESIGNER
#ifdef MODULE_VISION_LAB
    page = PanelFactory::CreateStagePage(ECS_WORLD_SELF_VAR PAGE_VISION_LAB, m_stage);
#endif // MODULE_VISION_LAB
#ifdef MODULE_MATERIAL
    page = PanelFactory::CreateStagePage(ECS_WORLD_SELF_VAR PAGE_MATERIAL, m_stage);
#endif // MODULE_MATERIAL
#ifdef MODULE_EDITOP
    page = PanelFactory::CreateStagePage(ECS_WORLD_SELF_VAR PAGE_EDITOP, m_stage);
#endif // MODULE_EDITOP
#ifdef MODULE_TASK
    page = PanelFactory::CreateStagePage(ECS_WORLD_SELF_VAR PAGE_TASK, m_stage);
#endif // MODULE_TASK
#ifdef MODULE_VFXGRAPH
	page = PanelFactory::CreateStagePage(*dev, ECS_WORLD_SELF_VAR PAGE_VFX_GRAPH, m_stage);
#endif // MODULE_VFXGRAPH

#ifdef MODULE_QUAKE
	page = PanelFactory::CreateStagePage(ECS_WORLD_SELF_VAR PAGE_QUAKE, m_stage);
#endif // MODULE_QUAKE

#ifdef MODULE_BLUEPRINT
	page = PanelFactory::CreateStagePage(ECS_WORLD_SELF_VAR PAGE_BLUEPRINT, m_stage);
#endif // MODULE_BLUEPRINT

    m_stage->AddNewPage(page, GetPageName(page->GetPageType()));

	m_stage->Thaw();

	return m_stage;
}

wxWindow* Application::CreateStageExtPanel()
{
	auto stage_ext = new WxStageSubPanel(m_frame);
	Blackboard::Instance()->SetStageExtPanel(stage_ext);
	return stage_ext;
}

wxWindow* Application::CreatePreviewPanel()
{
	auto& sub_mgr = m_stage->GetCurrentStagePage()->GetSubjectMgr();
	auto preview = new WxPreviewPanel(m_frame, sub_mgr, m_stage->GetCurrentStagePage());
	Blackboard::Instance()->SetPreviewPanel(preview);

    auto dev = Blackboard::Instance()->GetRenderDevice();
	auto canvas = std::make_shared<WxPreviewCanvas>(
		*dev, preview, ECS_WORLD_SELF_VAR Blackboard::Instance()->GetRenderContext());
	preview->GetImpl().SetCanvas(canvas);
	PanelFactory::CreatePreviewOP(
#ifdef GAME_OBJ_ECS
		m_world
#endif // GAME_OBJ_ECS
	);

	return preview;
}

wxWindow* Application::CreateWorldPanel()
{
    auto dev = Blackboard::Instance()->GetRenderDevice();
	auto curr_page = m_stage->GetCurrentStagePage();
	return new WxWorldPanel(*dev, m_frame, curr_page->GetSubjectMgr(),
		ECS_WORLD_SELF_VAR curr_page->GetEditedObj());
}

wxWindow* Application::CreateDetailPanel()
{
    auto dev = Blackboard::Instance()->GetRenderDevice();
	auto curr_page = m_stage->GetCurrentStagePage();
	return new WxDetailPanel(*dev, m_frame, curr_page->GetSubjectMgr(),
		ECS_WORLD_SELF_VAR curr_page->GetEditedObj(), curr_page->GetMoonCtx());
}

wxWindow* Application::CreateToolbarPanel()
{
	auto toolbar = new WxStageSubPanel(m_frame);
	Blackboard::Instance()->SetToolbarPanel(toolbar);
	return toolbar;
}

wxWindow* Application::CreateScriptPanel()
{
	return new WxScriptPanel(m_frame);
}

}