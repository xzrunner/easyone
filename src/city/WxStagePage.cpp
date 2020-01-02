#include "city/WxStagePage.h"

#ifdef MODULE_CITY

#include "frame/AppStyle.h"
#include "frame/Blackboard.h"
#include "frame/PanelFactory.h"
#include "frame/WxStagePanel.h"
#include "frame/WxStageSubPanel.h"
#include "frame/LeftDClickOP.h"
#include "frame/WxBlueprintCanvas.h"
#include "frame/GameObjFactory.h"

#include <ee0/SubjectMgr.h>
#include <ee0/WxStageCanvas.h>
#include <ee0/MsgHelper.h>
#include <ee2/ArrangeNodeOP.h>
#include <ee2/NodeSelectOP.h>
#include <ee3/NodeArrangeOP.h>
#include <ee3/WxStageCanvas.h>
#include <ee3/WorldTravelOP.h>
#include <ee3/CameraDriveOP.h>
#include <blueprint/NodeSelectOP.h>
#include <blueprint/ArrangeNodeOP.h>
#include <blueprint/ConnectPinOP.h>

#include <node0/SceneNode.h>
#include <node0/CompComplex.h>
#include <node0/CompIdentity.h>
#include <node2/CompTransform.h>
#include <node2/CompBoundingBox.h>
#include <ns/CompSerializer.h>
#include <sx/ResFileHelper.h>
#include <js/RapidJsonHelper.h>
#include <cgaview/CGAView.h>
#include <cgaview/WxPreviewCanvas.h>
#include <cgaview/Serializer.h>
#include <cgaview/WxEditorPanel.h>
#include <cgaview/WxToolbarPanel.h>
#include <cgaview/WxGraphPage.h>
#include <cgaview/MessageID.h>
#include <draft3/PolygonBuildOP.h>
#include <draft3/PolygonSelectOP.h>

#include <boost/filesystem.hpp>

namespace eone
{
namespace city
{

const std::string WxStagePage::PAGE_TYPE = "city_stage";

WxStagePage::WxStagePage(wxWindow* parent, ECS_WORLD_PARAM const ee0::GameObj& obj)
    : eone::WxStagePage(parent, ECS_WORLD_VAR obj, SHOW_STAGE | SHOW_TOOLBAR | SHOW_STAGE_EXT | STAGE_EXT_RIGHT)
    , m_preview_impl(*this)
{
	m_messages.push_back(ee0::MSG_SCENE_NODE_INSERT);
	m_messages.push_back(ee0::MSG_SCENE_NODE_DELETE);
	m_messages.push_back(ee0::MSG_SCENE_NODE_CLEAR);

    m_messages.push_back(ee0::MSG_STAGE_PAGE_NEW);

    m_messages.push_back(cgav::MSG_SET_VIEW_EDITOP);
    m_messages.push_back(cgav::MSG_SET_DRAW_EDITOP);

    cgav::Serializer::Init();
}

void WxStagePage::OnNotify(uint32_t msg, const ee0::VariantSet& variants)
{
	eone::WxStagePage::OnNotify(msg, variants);

	bool dirty = false;
	switch (msg)
	{
	case ee0::MSG_SCENE_NODE_INSERT:
		dirty = InsertSceneObj(variants);
		break;
	case ee0::MSG_SCENE_NODE_DELETE:
		dirty = DeleteSceneObj(variants);
		break;
	case ee0::MSG_SCENE_NODE_CLEAR:
		dirty = ClearSceneObj();
		break;

    case ee0::MSG_STAGE_PAGE_NEW:
        CreateNewPage(variants);
        break;

    case cgav::MSG_SET_VIEW_EDITOP:
        GetImpl().SetEditOP(m_view_op);
        break;
    case cgav::MSG_SET_DRAW_EDITOP:
        GetImpl().SetEditOP(m_edit_op);
        break;
	}

	if (dirty) {
		m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
	}
}

void WxStagePage::Traverse(std::function<bool(const ee0::GameObj&)> func,
	                       const ee0::VariantSet& variants, bool inverse) const
{
	auto var = variants.GetVariant("type");
	if (var.m_type == ee0::VT_EMPTY)
	{
		// todo ecs
#ifndef GAME_OBJ_ECS
		m_obj->GetSharedComp<n0::CompComplex>().Traverse(func, inverse);
#endif // GAME_OBJ_ECS
		return;
	}

	GD_ASSERT(var.m_type == ee0::VT_LONG, "err type");
	switch (var.m_val.l)
	{
	case TRAV_DRAW_PREVIEW:
		func(m_obj);
		break;
		// todo ecs
#ifndef GAME_OBJ_ECS
	default:
		m_obj->GetSharedComp<n0::CompComplex>().Traverse(func, inverse);
#else
	default:
		{
			auto& ccomplex = m_world.GetComponent<e2::CompComplex>(m_obj);
			if (!inverse) {
				for (auto& child : *ccomplex.children) {
					func(child);
				}
			} else {
				for (auto itr = ccomplex.children->rbegin(); itr != ccomplex.children->rend(); ++itr) {
					func(*itr);
				}
			}
		}
#endif // GAME_OBJ_ECS
	}
}

void WxStagePage::InitEditOP()
{
    auto canvas = GetImpl().GetCanvas();
    assert(canvas);
    auto& vp = std::static_pointer_cast<ee3::WxStageCanvas>(canvas)->GetViewport();
    m_view_op = std::make_shared<draft3::PolygonSelectOP>(
        canvas->GetCamera(), *this, vp
    );
    m_edit_op = std::make_shared<draft3::PolygonBuildOP>(
        canvas->GetCamera(), vp, GetSubjectMgr()
    );
    m_view_op->SetPrevEditOP(std::make_shared<ee3::WorldTravelOP>(
        canvas->GetCamera(), vp, GetSubjectMgr()
    ));
    m_edit_op->SetPrevEditOP(std::make_shared<ee3::CameraDriveOP>(
        canvas->GetCamera(), vp, GetSubjectMgr()
    ));
    GetImpl().SetEditOP(m_view_op);
}

void WxStagePage::OnPageInit()
{
    InitEditorPanel();
}

#ifndef GAME_OBJ_ECS
const n0::NodeComp& WxStagePage::GetEditedObjComp() const
{
	return m_obj->GetSharedComp<n0::CompComplex>();
}
#endif // GAME_OBJ_ECS

void WxStagePage::StoreToJsonExt(const std::string& dir, rapidjson::Value& val,
                                 rapidjson::MemoryPoolAllocator<>& alloc) const
{
    auto bp_page = m_editor_panel->GetGraphPage();
    cgav::Serializer::StoreToJson(bp_page->GetRootNode(), dir, val, alloc);

    val.AddMember("page_type", rapidjson::Value(PAGE_TYPE.c_str(), alloc), alloc);
}

void WxStagePage::LoadFromFileExt(const std::string& filepath)
{
    auto type = sx::ResFileHelper::Type(filepath);
    switch (type)
    {
    case sx::RES_FILE_JSON:
    {
        rapidjson::Document doc;
        js::RapidJsonHelper::ReadFromFile(filepath.c_str(), doc);

        auto dir = boost::filesystem::path(filepath).parent_path().string();

        auto bp_page = m_editor_panel->GetGraphPage();
        cgav::Serializer::LoadFromJson(*bp_page, bp_page->GetRootNode(), doc["graph"], dir);
    }
        break;
    }
}

void WxStagePage::InitEditorPanel()
{
    m_graph_obj = GameObjFactory::Create(ECS_WORLD_VAR GAME_OBJ_COMPLEX2D);

    auto stage_ext_panel = Blackboard::Instance()->GetStageExtPanel();

    m_editor_panel = new cgav::WxEditorPanel(stage_ext_panel, [&](wxWindow* parent, cga::EvalContext& ctx) -> cgav::WxGraphPage*
    {
        auto graph_page = CreateGraphPanel(parent);

        auto toolbar_panel = Blackboard::Instance()->GetToolbarPanel();

        auto toolbar = new cgav::WxToolbarPanel(
            toolbar_panel, ctx, graph_page->GetSubjectMgr(), GetSubjectMgr()
        );
        toolbar_panel->AddPagePanel(toolbar, wxVERTICAL);
        graph_page->SetToolbarPanel(toolbar);

        return graph_page;
    });
    stage_ext_panel->AddPagePanel(m_editor_panel, wxVERTICAL);
}

cgav::WxGraphPage*
WxStagePage::CreateGraphPanel(wxWindow* parent) const
{
    auto panel = new cgav::WxGraphPage(parent, nullptr, m_graph_obj);
    auto& panel_impl = panel->GetImpl();

    auto preview_canvas = std::static_pointer_cast<cgav::WxPreviewCanvas>(GetImpl().GetCanvas());
    panel->SetPreviewCanvas(preview_canvas);

    auto canvas = std::make_shared<WxBlueprintCanvas>(
        panel, Blackboard::Instance()->GetRenderContext()
    );
    panel_impl.SetCanvas(canvas);

    auto select_op = std::make_shared<bp::NodeSelectOP>(canvas->GetCamera(), *panel);

    ee2::ArrangeNodeCfg cfg;
    cfg.is_auto_align_open = false;
    cfg.is_deform_open = false;
    cfg.is_offset_open = false;
    cfg.is_rotate_open = false;
    auto arrange_op = std::make_shared<bp::ArrangeNodeOP>(
        canvas->GetCamera(), *panel, ECS_WORLD_VAR cfg, select_op);

    auto& nodes = cgav::CGAView::Instance()->GetAllNodes();
    auto op = std::make_shared<bp::ConnectPinOP>(canvas->GetCamera(), *panel, nodes);
    op->SetPrevEditOP(arrange_op);
    panel_impl.SetEditOP(op);

    return panel;
}

bool WxStagePage::InsertSceneObj(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("obj");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: obj");
    const ee0::GameObj* obj = static_cast<const ee0::GameObj*>(var.m_val.pv);
	GD_ASSERT(obj, "err scene obj");

#ifndef GAME_OBJ_ECS
	auto& ccomplex = m_obj->GetSharedComp<n0::CompComplex>();
	ccomplex.AddChild(*obj);
#else
	auto& ccomplex = m_world.GetComponent<e2::CompComplex>(m_obj);
	ccomplex.children->push_back(*obj);
#endif // GAME_OBJ_ECS

	return true;
}

bool WxStagePage::DeleteSceneObj(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("obj");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: obj");
    const ee0::GameObj* obj = static_cast<const ee0::GameObj*>(var.m_val.pv);
	GD_ASSERT(obj, "err scene obj");

#ifndef GAME_OBJ_ECS
	auto& ccomplex = m_obj->GetSharedComp<n0::CompComplex>();
	return ccomplex.RemoveChild(*obj);
#else
	auto& ccomplex = m_world.GetComponent<e2::CompComplex>(m_obj);
	auto& children = *ccomplex.children;
	for (auto itr = children.begin(); itr != children.end(); ++itr)
	{
		if (*itr == *obj) {
			ccomplex.children->erase(itr);
			return true;
		}
	}
	return false;
#endif // GAME_OBJ_ECS
}

bool WxStagePage::ClearSceneObj()
{
#ifndef GAME_OBJ_ECS
	auto& ccomplex = m_obj->GetSharedComp<n0::CompComplex>();
	bool dirty = !ccomplex.GetAllChildren().empty();
	ccomplex.RemoveAllChildren();
#else
	auto& ccomplex = m_world.GetComponent<e2::CompComplex>(m_obj);
	bool dirty = !ccomplex.children->empty();
	// todo clear components
	ccomplex.children->clear();
#endif // GAME_OBJ_ECS
	return dirty;
}

void WxStagePage::CreateNewPage(const ee0::VariantSet& variants) const
{
    auto type_var = variants.GetVariant("type");
    GD_ASSERT(type_var.m_type == ee0::VT_PCHAR, "no var in vars: type");
    auto type = type_var.m_val.pc;

    auto filepath_var = variants.GetVariant("filepath");
    GD_ASSERT(filepath_var.m_type == ee0::VT_PCHAR, "no var in vars: filepath");
    auto filepath = filepath_var.m_val.pc;

    int page_type = -1;
    if (strcmp(type, bp::PAGE_TYPE) == 0) {
        page_type = PAGE_CITY;
    }
    if (page_type >= 0)
    {
        auto stage_panel = Blackboard::Instance()->GetStagePanel();
        auto stage_page = PanelFactory::CreateStagePage(page_type, stage_panel);
        stage_panel->AddNewPage(stage_page, GetPageName(stage_page->GetPageType()));

        if (page_type == PAGE_CITY)
        {
            auto var = variants.GetVariant("obj");
            GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: obj");
            const ee0::GameObj* obj = static_cast<const ee0::GameObj*>(var.m_val.pv);
            GD_ASSERT(obj, "err scene obj");
        }
    }
}

}
}

#endif // MODULE_CITY
