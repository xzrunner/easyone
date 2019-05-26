#include "guigraph/WxStagePage.h"
#include "guigraph/WxToolbarPanel.h"
#include "guigraph/WxGraphPage.h"

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
#include <blueprint/Blueprint.h>
#include <blueprint/MessageID.h>
#include <blueprint/NSCompNode.h>
#include <blueprint/CommentaryNodeHelper.h>
#include <blueprint/Node.h>
#include <blueprint/CompNode.h>
#include <blueprint/NodeHelper.h>
#include <blueprint/NodeSelectOP.h>
#include <blueprint/ArrangeNodeOP.h>
#include <blueprint/ConnectPinOP.h>
#include <guilab/GuiLab.h>

#include <node0/SceneNode.h>
#include <node0/CompComplex.h>
#include <node0/CompIdentity.h>
#include <node2/CompTransform.h>
#include <node2/CompBoundingBox.h>
#include <sx/ResFileHelper.h>

namespace eone
{
namespace guigraph
{

const std::string WxStagePage::PAGE_TYPE = "gui_graph";
WxStagePage::WxStagePage(wxWindow* parent, ECS_WORLD_PARAM const ee0::GameObj& obj)
	: eone::WxStagePage(parent, ECS_WORLD_VAR obj, SHOW_STAGE | SHOW_TOOLBAR | SHOW_STAGE_EXT | STAGE_EXT_RIGHT)
{
	static bool inited = false;
	if (!inited) {
		inited = true;
		bp::Blueprint::Instance();
	}

//    rlab::Blackboard::Instance()->SetEval(m_eval);

	m_messages.push_back(ee0::MSG_SCENE_NODE_INSERT);
	m_messages.push_back(ee0::MSG_SCENE_NODE_DELETE);
	m_messages.push_back(ee0::MSG_SCENE_NODE_CLEAR);

    m_messages.push_back(ee0::MSG_STAGE_PAGE_NEW);

	m_messages.push_back(bp::MSG_BLUE_PRINT_CHANGED);
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

    case bp::MSG_BLUE_PRINT_CHANGED:
        UpdateBlueprint();
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

bool WxStagePage::LoadNodeConnsFromFile(const std::string& filepath)
{
	if (sx::ResFileHelper::Type(filepath) == sx::RES_FILE_JSON) {
        auto& ccomplex = m_obj->GetSharedComp<n0::CompComplex>();
        bp::NodeHelper::LoadConnections(ccomplex.GetAllChildren(), filepath);
		return true;
	} else {
        return false;
    }
}

void WxStagePage::OnPageInit()
{
    InitToolbarPanel();
    InitGraphPanel();
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
	// connection
	auto& ccomplex = m_obj->GetSharedComp<n0::CompComplex>();
	bp::NSCompNode::StoreConnection(ccomplex.GetAllChildren(), val["nodes"], alloc);

	val.AddMember("page_type", rapidjson::Value(PAGE_TYPE.c_str(), alloc), alloc);
}

void WxStagePage::LoadFromFileExt(const std::string& filepath)
{
    bp::CommentaryNodeHelper::InsertNodeToCommentary(*this);

    if (LoadNodeConnsFromFile(filepath)) {
        m_sub_mgr->NotifyObservers(bp::MSG_BLUE_PRINT_CHANGED);
    }
}

void WxStagePage::InitToolbarPanel()
{
    assert(!m_toolbar);
    auto toolbar_panel = Blackboard::Instance()->GetToolbarPanel();
    m_toolbar = static_cast<WxToolbarPanel*>(toolbar_panel->AddPagePanel([&](wxPanel* parent)->wxPanel* {
        return new WxToolbarPanel(toolbar_panel, this);
    }, wxVERTICAL));

    //auto bp_node = std::make_shared<raylab::node::World>();
    //auto node = std::make_shared<n0::SceneNode>();
    //auto& cnode = node->AddUniqueComp<bp::CompNode>();
    //cnode.SetNode(bp_node);
    //auto& ctrans = node->AddUniqueComp<n2::CompTransform>();
    //ctrans.SetPosition(*node, sm::vec2(0, 0));
    //node->AddUniqueComp<n0::CompIdentity>();
    //auto& style = bp_node->GetStyle();
    //node->AddUniqueComp<n2::CompBoundingBox>(
    //    sm::rect(style.width, style.height)
    //);

    //ee0::MsgHelper::InsertNode(*m_sub_mgr, node, false);
}


void WxStagePage::InitGraphPanel()
{
    m_graph_obj = GameObjFactory::Create(ECS_WORLD_VAR GAME_OBJ_COMPLEX2D);

    auto stage_ext_panel = Blackboard::Instance()->GetStageExtPanel();
    m_graph_panel = stage_ext_panel->AddPagePanel([&](wxPanel* parent)->wxPanel*
    {
        auto panel = new WxGraphPage(parent, m_graph_obj);

        auto canvas = std::make_shared<WxBlueprintCanvas>(
            panel, Blackboard::Instance()->GetRenderContext()
        );
        panel->GetImpl().SetCanvas(canvas);

        auto prev_op = std::make_shared<ee2::NodeSelectOP>(canvas->GetCamera(), *panel);

        auto select_op = std::make_shared<bp::NodeSelectOP>(canvas->GetCamera(), *panel);
        select_op->AddPrevEditOP(prev_op);

        ee2::ArrangeNodeCfg cfg;
        cfg.is_auto_align_open = false;
        cfg.is_deform_open = false;
        cfg.is_offset_open = false;
        cfg.is_rotate_open = false;
        auto arrange_op = std::make_shared<bp::ArrangeNodeOP>(
            canvas->GetCamera(), *panel, ECS_WORLD_VAR cfg, select_op);

        auto& nodes = guilab::GuiLab::Instance()->GetAllNodes();
        auto op = std::make_shared<bp::ConnectPinOP>(canvas->GetCamera(), *panel, nodes);
        op->SetPrevEditOP(arrange_op);
        panel->GetImpl().SetEditOP(op);

        return panel;
    }, wxVERTICAL);
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
        page_type = PAGE_GUI_GRAPH;
    }
    if (page_type >= 0)
    {
        auto stage_panel = Blackboard::Instance()->GetStagePanel();
        auto stage_page = PanelFactory::CreateStagePage(page_type, stage_panel);
        stage_panel->AddNewPage(stage_page, GetPageName(stage_page->GetPageType()));

        if (page_type == PAGE_GUI_GRAPH)
        {
            auto var = variants.GetVariant("obj");
            GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: obj");
            const ee0::GameObj* obj = static_cast<const ee0::GameObj*>(var.m_val.pv);
            GD_ASSERT(obj, "err scene obj");
        }
    }
}

bool WxStagePage::UpdateNodes()
{
    bool dirty = false;
    auto& wc = GetImpl().GetCanvas()->GetWidnowContext();
    bp::UpdateParams params(wc.wc2, wc.wc3);
    Traverse([&](const ee0::GameObj& obj)->bool
    {
        if (!obj->HasUniqueComp<bp::CompNode>()) {
            return true;
        }
        auto& bp_node = obj->GetUniqueComp<bp::CompNode>().GetNode();
        if (bp_node->Update(params)) {
            dirty = true;
        }
        return true;
    });
    return dirty;
}

void WxStagePage::UpdateBlueprint()
{
    //rlab::Blackboard::Instance()->SetEval(m_eval);

    //bool dirty = UpdateNodes();

    //std::vector<std::shared_ptr<rlab::Node>> nodes;
    //Traverse([&](const ee0::GameObj& obj)->bool
    //{
    //    if (!obj->HasUniqueComp<bp::CompNode>()) {
    //        return true;
    //    }
    //    auto& bp_node = obj->GetUniqueComp<bp::CompNode>().GetNode();
    //    if (bp_node->get_derived_info().m_type.is_derived_from<rlab::Node>()) {
    //        nodes.push_back(std::static_pointer_cast<rlab::Node>(bp_node));
    //    }
    //    return true;
    //});

    //if (!nodes.empty()) {
    //    m_eval->Rebuild(nodes);
    //}

    //if (dirty) {
    //    m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
    //}
}

}
}