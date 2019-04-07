#include "rgraph/WxStagePage.h"
#include "rgraph/WxToolbarPanel.h"
#include "rgraph/WxPreviewPanel.h"

#include "frame/AppStyle.h"
#include "frame/Blackboard.h"
#include "frame/PanelFactory.h"
#include "frame/WxStagePanel.h"
#include "frame/WxStageSubPanel.h"

#include <ee0/SubjectMgr.h>
#include <ee0/WxStageCanvas.h>
#include <blueprint/Blueprint.h>
#include <blueprint/MessageID.h>
#include <blueprint/NSCompNode.h>
#include <blueprint/NodeHelper.h>
#include <blueprint/Node.h>
#include <blueprint/CompNode.h>
#include <renderlab/RegistNodes.h>

#include <node0/SceneNode.h>
#include <node0/CompComplex.h>
#include <sx/ResFileHelper.h>

namespace
{

const bool DEBUG_PRINT_SHADER = false;

}

namespace eone
{
namespace rgraph
{

const std::string WxStagePage::PAGE_TYPE = "render_graph";

WxStagePage::WxStagePage(wxWindow* parent, ECS_WORLD_PARAM const ee0::GameObj& obj)
	: eone::WxStagePage(parent, ECS_WORLD_VAR obj, SHOW_STAGE | SHOW_TOOLBAR | TOOLBAR_LFET)
{
	static bool inited = false;
	if (!inited) {
		inited = true;
		bp::Blueprint::Instance();
	}

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
    assert(!m_toolbar);
    auto toolbar_panel = Blackboard::Instance()->GetToolbarPanel();
    m_toolbar = static_cast<WxToolbarPanel*>(toolbar_panel->AddPagePanel([&](wxPanel* parent)->wxPanel* {
        return new WxToolbarPanel(toolbar_panel, this);
    }, wxVERTICAL));
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
    if (LoadNodeConnsFromFile(filepath)) {
        m_sub_mgr->NotifyObservers(bp::MSG_BLUE_PRINT_CHANGED);
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
        page_type = PAGE_RENDER_GRAPH;
    }
    if (page_type >= 0)
    {
        auto stage_panel = Blackboard::Instance()->GetStagePanel();
        auto stage_page = PanelFactory::CreateStagePage(page_type, stage_panel);
        stage_panel->AddNewPage(stage_page, GetPageName(stage_page->GetPageType()));

        if (page_type == PAGE_RENDER_GRAPH)
        {
            auto var = variants.GetVariant("obj");
            GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: obj");
            const ee0::GameObj* obj = static_cast<const ee0::GameObj*>(var.m_val.pv);
            GD_ASSERT(obj, "err scene obj");
        }
    }
}

void WxStagePage::UpdateBlueprint()
{
    bool dirty = false;

    bp::NodePtr output2screen = nullptr;

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
        if (bp_node->get_type() == rttr::type::get<rlab::node::OutputToScreen>()) {
            output2screen = bp_node;
        }
        return true;
    });

    if (output2screen)
    {
        auto& conns = output2screen->GetAllInput()[0]->GetConnecting();
        if (!conns.empty())
        {
            auto preview = m_toolbar->GetPreviewPanel();
            preview->RebuildDrawList(conns[0]->GetFrom()->GetParent());
        }
    }

    if (dirty) {
        m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
    }
}

}
}