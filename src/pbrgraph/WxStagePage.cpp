#include "pbrgraph/WxStagePage.h"

#ifdef MODULE_PBRGRAPH

#include "pbrgraph/WxToolbarPanel.h"
#include "pbrgraph/WxPreviewPanel.h"

#include "frame/AppStyle.h"
#include "frame/Blackboard.h"
#include "frame/PanelFactory.h"
#include "frame/WxStagePanel.h"
#include "frame/WxStageSubPanel.h"

#include <ee0/SubjectMgr.h>
#include <ee0/WxStageCanvas.h>
#include <ee0/MsgHelper.h>
#include <blueprint/Blueprint.h>
#include <blueprint/MessageID.h>
#include <blueprint/NSCompNode.h>
#include <blueprint/NodeHelper.h>
#include <blueprint/Node.h>
#include <blueprint/CompNode.h>
#include <blueprint/CommentaryNodeHelper.h>
#include <blueprint/node/Function.h>
#include <renderlab/RegistNodes.h>
#include <renderlab/Evaluator.h>
#include <renderlab/Blackboard.h>

#include <node0/SceneNode.h>
#include <node0/CompComplex.h>
#include <sx/ResFileHelper.h>
#include <js/RapidJsonHelper.h>

#include <pbr/MaterialBuilder.h>

namespace
{

const bool DEBUG_PRINT_SHADER = false;

}

namespace eone
{
namespace pbrgraph
{

const std::string WxStagePage::PAGE_TYPE = "render_graph";

WxStagePage::WxStagePage(wxWindow* parent, ECS_WORLD_PARAM const ee0::GameObj& obj)
	: eone::WxStagePage(parent, ECS_WORLD_VAR obj, SHOW_STAGE | SHOW_TOOLBAR | TOOLBAR_LFET)
    , m_eval(std::make_shared<rlab::Evaluator>())
{
	static bool inited = false;
	if (!inited) {
		inited = true;
		bp::Blueprint::Instance();
	}

    rlab::Blackboard::Instance()->SetEval(m_eval);

	m_messages.push_back(ee0::MSG_SCENE_NODE_INSERT);
	m_messages.push_back(ee0::MSG_SCENE_NODE_DELETE);
	m_messages.push_back(ee0::MSG_SCENE_NODE_CLEAR);

    m_messages.push_back(ee0::MSG_STAGE_PAGE_NEW);

    m_messages.push_back(bp::MSG_BP_CONN_INSERT);
    m_messages.push_back(bp::MSG_BP_CONN_DELETE);
    m_messages.push_back(bp::MSG_BP_CONN_REBUILD);
    m_messages.push_back(bp::MSG_BP_NODE_PROP_CHANGED);

    //
    pbr::MaterialBuilder mbuilder;
    mbuilder.RunSemanticAnalysis();
}

WxStagePage::~WxStagePage()
{
    m_func_node_helper.SetFilepath(m_filepath);
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
    case ee0::MSG_STAGE_PAGE_ON_SHOW:
        m_toolbar->Show();
        m_toolbar->GetParent()->Layout();
        break;
    case ee0::MSG_STAGE_PAGE_ON_HIDE:
        m_toolbar->Hide();
        m_toolbar->GetParent()->Layout();
        break;

    case bp::MSG_BP_CONN_INSERT:
    case bp::MSG_BP_CONN_DELETE:
    case bp::MSG_BP_CONN_REBUILD:
    case bp::MSG_BP_NODE_PROP_CHANGED:
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

void WxStagePage::OnPageInit()
{
    assert(!m_toolbar);
    auto toolbar_panel = Blackboard::Instance()->GetToolbarPanel();
    m_toolbar = new WxToolbarPanel(toolbar_panel, this);
    toolbar_panel->AddPagePanel(m_toolbar, wxVERTICAL);
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

    LoadFunctionNodes();

    if (sx::ResFileHelper::Type(filepath) == sx::RES_FILE_JSON)
    {
        rapidjson::Document doc;
        js::RapidJsonHelper::ReadFromFile(filepath.c_str(), doc);

        auto& ccomplex = m_obj->GetSharedComp<n0::CompComplex>();
        bp::NSCompNode::LoadConnection(ccomplex.GetAllChildren(), doc["nodes"]);

        m_sub_mgr->NotifyObservers(bp::MSG_BP_CONN_REBUILD);
    }
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

    m_func_node_helper.InsertSceneObj(*obj);

	return true;
}

bool WxStagePage::DeleteSceneObj(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("obj");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: obj");
    const ee0::GameObj* obj = static_cast<const ee0::GameObj*>(var.m_val.pv);
	GD_ASSERT(obj, "err scene obj");

    m_func_node_helper.DeleteSceneObj(*obj);

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
    m_func_node_helper.ClearSceneObj();

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
        page_type = PAGE_PBR_GRAPH;
    }
    if (page_type >= 0)
    {
        auto stage_panel = Blackboard::Instance()->GetStagePanel();
        auto stage_page = PanelFactory::CreateStagePage(page_type, stage_panel);
        stage_panel->AddNewPage(stage_page, GetPageName(stage_page->GetPageType()));

        if (page_type == PAGE_PBR_GRAPH)
        {
            auto var = variants.GetVariant("obj");
            GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: obj");
            const ee0::GameObj* obj = static_cast<const ee0::GameObj*>(var.m_val.pv);
            GD_ASSERT(obj, "err scene obj");

            if ((*obj)->HasUniqueComp<bp::CompNode>())
            {
                auto& cbp = (*obj)->GetUniqueComp<bp::CompNode>();
                auto& bp_node = cbp.GetNode();
                if (bp_node->get_type() == rttr::type::get<bp::node::Function>())
                {
                    auto sg_stage_page = static_cast<pbrgraph::WxStagePage*>(stage_page);
                    auto& func_node_helper = sg_stage_page->GetFuncNodeHelper();

                    func_node_helper.SetParentNode(*obj);
                    stage_page->SetFilepath(filepath);

                    auto func_node = std::static_pointer_cast<bp::node::Function>(bp_node);
                    func_node_helper.EnableInsert(false);
                    for (auto& c : func_node->GetChildren()) {
                        ee0::MsgHelper::InsertNode(*sg_stage_page->GetSubjectMgr(), c, false);
                    }
                    func_node_helper.EnableInsert(true);

                    if (sx::ResFileHelper::Type(filepath) == sx::RES_FILE_JSON)
                    {
                        rapidjson::Document doc;
                        js::RapidJsonHelper::ReadFromFile(filepath, doc);

                        auto& ccomplex = sg_stage_page->GetEditedObj()->GetSharedComp<n0::CompComplex>();
                        bp::NSCompNode::LoadConnection(ccomplex.GetAllChildren(), doc["nodes"]);
                    }
                }
            }
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
    rlab::Blackboard::Instance()->SetEval(m_eval);

    bool dirty = UpdateNodes();

    std::vector<bp::NodePtr> nodes;
    Traverse([&](const ee0::GameObj& obj)->bool
    {
        if (!obj->HasUniqueComp<bp::CompNode>()) {
            return true;
        }
        auto& bp_node = obj->GetUniqueComp<bp::CompNode>().GetNode();
        nodes.push_back(bp_node);
        return true;
    });

    if (!nodes.empty()) {
        m_eval->Rebuild(nodes);
    }

    if (dirty) {
        m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
    }
}

void WxStagePage::LoadFunctionNodes()
{
    Traverse([&](const ee0::GameObj& obj)->bool
    {
        if (obj->HasUniqueComp<bp::CompNode>()) {
            auto& bp_node = obj->GetUniqueComp<bp::CompNode>().GetNode();
            if (bp_node->get_type() == rttr::type::get<bp::node::Function>()) {
                bp::NodeHelper::LoadFunctionNode(obj, bp_node);
            }
        }
        return true;
    });
}

}
}

#endif // MODULE_RENDERGRAPH