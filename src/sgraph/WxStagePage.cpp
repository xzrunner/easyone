#include "sgraph/WxStagePage.h"
#include "sgraph/WxToolbarPanel.h"

#include "frame/AppStyle.h"
#include "frame/Blackboard.h"
#include "frame/WxStageSubPanel.h"
#include "frame/PanelFactory.h"
#include "frame/WxStagePanel.h"

#include <ee0/SubjectMgr.h>
#include <ee0/MsgHelper.h>
#include <ee0/WindowContext.h>
#include <ee3/WxMaterialPreview.h>
#include <blueprint/Blueprint.h>
#include <blueprint/CompNode.h>
#include <blueprint/MessageID.h>
#include <blueprint/Pins.h>
#include <blueprint/NSCompNode.h>
#include <blueprint/node/Input.h>
#include <blueprint/node/Output.h>
#include <blueprint/node/Function.h>
#include <shadergraph/ShaderGraph.h>
#include <shadergraph/NodeBuilder.h>
#include <shadergraph/ShaderWeaver.h>
#include <shadergraph/RegistNodes.h>

#include <js/RapidJsonHelper.h>
#include <unirender/VertexAttrib.h>
#include <unirender/Blackboard.h>
#include <unirender/Shader.h>
#include <unirender/RenderContext.h>
#include <painting3/Shader.h>
#include <node0/SceneNode.h>
#include <node0/CompComplex.h>
#include <node2/CompBoundingBox.h>
#include <sx/ResFileHelper.h>

#include <boost/filesystem.hpp>

namespace
{

const bool DEBUG_PRINT_SHADER = false;

}

namespace eone
{
namespace sgraph
{

const std::string WxStagePage::PAGE_TYPE = "shader_graph";

WxStagePage::WxStagePage(wxWindow* parent, ECS_WORLD_PARAM const ee0::GameObj& obj)
	: eone::WxStagePage(parent, ECS_WORLD_VAR obj, SHOW_STAGE | SHOW_TOOLBAR | TOOLBAR_LFET)
{
	static bool inited = false;
	if (!inited) {
		inited = true;
		bp::Blueprint::Instance();
	}
	sg::ShaderGraph::Instance();

	m_messages.push_back(ee0::MSG_SCENE_NODE_INSERT);
	m_messages.push_back(ee0::MSG_SCENE_NODE_DELETE);
	m_messages.push_back(ee0::MSG_SCENE_NODE_CLEAR);

    m_messages.push_back(ee0::MSG_STAGE_PAGE_NEW);

	m_messages.push_back(bp::MSG_BLUE_PRINT_CHANGED);
}

WxStagePage::~WxStagePage()
{
    if (m_parent_node) {
        if (m_parent_node->HasUniqueComp<bp::CompNode>()) {
            auto& cnode = m_parent_node->GetUniqueComp<bp::CompNode>();
            auto bp_node = cnode.GetNode();
            if (bp_node->get_type() == rttr::type::get<bp::node::Function>()) {
                auto func_node = std::static_pointer_cast<bp::node::Function>(bp_node);
                func_node->SetFilepath(m_filepath);
            }
        }
    }
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

	case bp::MSG_BLUE_PRINT_CHANGED:
		UpdateShader();
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
	if (sx::ResFileHelper::Type(filepath) != sx::RES_FILE_JSON) {
		return false;
	}

	auto dir = boost::filesystem::path(filepath).parent_path().string();
	rapidjson::Document doc;
	js::RapidJsonHelper::ReadFromFile(filepath.c_str(), doc);

	// connection
	auto& ccomplex = m_obj->GetSharedComp<n0::CompComplex>();
	auto& nodes = const_cast<std::vector<n0::SceneNodePtr>&>(ccomplex.GetAllChildren());
	bp::NSCompNode::LoadConnection(nodes, doc["nodes"]);

    return true;
}

void WxStagePage::SetModelType(ModelType model_type)
{
	if (m_model_type == model_type) {
		return;
	}

    m_model_type = model_type;

    m_model_type_str.clear();
    switch (model_type)
	{
    case ModelType::SPRITE:
        m_model_type_str = rttr::type::get<sg::node::Sprite>().get_name().to_string();
		break;
	case ModelType::PHONG:
        m_model_type_str = rttr::type::get<sg::node::Phong>().get_name().to_string();
		break;
    case ModelType::PBR:
        m_model_type_str = rttr::type::get<sg::node::PBR>().get_name().to_string();
        break;
    case ModelType::RAYMARCHING:
        m_model_type_str = rttr::type::get<sg::node::Raymarching>().get_name().to_string();
        break;
	}
}

void WxStagePage::OnPageInit()
{
    assert(!m_toolbar);
    auto toolbar_panel = Blackboard::Instance()->GetToolbarPanel();
    m_toolbar = static_cast<WxToolbarPanel*>(toolbar_panel->AddPagePanel([&](wxPanel* parent)->wxPanel* {
        return new WxToolbarPanel(toolbar_panel, this);
    }, wxVERTICAL));

    // model type init
    if (m_model_type != ModelType::UNKNOWN)
    {
        std::vector<n0::SceneNodePtr> nodes;
        auto bp_node = sg::NodeBuilder::Create(nodes, m_model_type_str);
        if (bp_node) {
            ClearSceneObj();
            for (auto& node : nodes) {
                ee0::MsgHelper::InsertNode(*m_sub_mgr, node, false);
            }
        }
    }

	UpdateShader();
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

    if (m_parent_node) {
        if (m_parent_node->HasUniqueComp<bp::CompNode>()) {
            auto& cnode = m_parent_node->GetUniqueComp<bp::CompNode>();
            auto bp_node = cnode.GetNode();
            if (bp_node->get_type() == rttr::type::get<bp::node::Function>()) {
                auto func_node = std::static_pointer_cast<bp::node::Function>(bp_node);
                if (bp::node::Function::AddChild(func_node, *obj)) {
                    UpdateParentAABB(func_node);
                }
            }
        }
    }

	return true;
}

bool WxStagePage::DeleteSceneObj(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("obj");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: obj");
    const ee0::GameObj* obj = static_cast<const ee0::GameObj*>(var.m_val.pv);
	GD_ASSERT(obj, "err scene obj");

    if (m_parent_node) {
        if (m_parent_node->HasUniqueComp<bp::CompNode>()) {
            auto& cnode = m_parent_node->GetUniqueComp<bp::CompNode>();
            auto bp_node = cnode.GetNode();
            if (bp_node->get_type() == rttr::type::get<bp::node::Function>()) {
                auto func_node = std::static_pointer_cast<bp::node::Function>(bp_node);
                if (bp::node::Function::RemoveChild(func_node, *obj)) {
                    UpdateParentAABB(func_node);
                }
            }
        }
    }

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
    if (m_parent_node) {
        if (m_parent_node->HasUniqueComp<bp::CompNode>()) {
            auto& cnode = m_parent_node->GetUniqueComp<bp::CompNode>();
            auto bp_node = cnode.GetNode();
            if (bp_node->get_type() == rttr::type::get<bp::node::Function>()) {
                auto func_node = std::static_pointer_cast<bp::node::Function>(bp_node);
                func_node->ClearChildren();
                UpdateParentAABB(func_node);
            }
        }
    }

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
        page_type = PAGE_SHADER_GRAPH;
    }
    if (page_type >= 0)
    {
        auto stage_panel = Blackboard::Instance()->GetStagePanel();
        auto stage_page = PanelFactory::CreateStagePage(page_type, stage_panel);
        static_cast<WxStagePage*>(stage_page)->SetModelType(ModelType::UNKNOWN);
        stage_panel->AddNewPage(stage_page, GetPageName(stage_page->GetPageType()));

        if (page_type == PAGE_SHADER_GRAPH)
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
                    auto sg_stage_page = static_cast<sgraph::WxStagePage*>(stage_page);

                    sg_stage_page->SetParentNode(*obj);
                    stage_page->SetFilepath(filepath);

                    auto func_node = std::static_pointer_cast<bp::node::Function>(bp_node);
                    auto& root_node = stage_page->GetEditedObj();
                    assert(root_node->HasSharedComp<n0::CompComplex>());
                    auto& root_ccomplex = root_node->GetSharedComp<n0::CompComplex>();
                    for (auto& c : func_node->GetChildren()) {
                        root_ccomplex.AddChild(c);
                    }

                    sg_stage_page->LoadNodeConnsFromFile(filepath);
                }
            }
        }
    }
}

void WxStagePage::UpdateShader()
{
	if (!m_toolbar) {
		return;
	}

	bool dirty = false;

	// use the same render context
	auto& canvas = m_toolbar->GetPreviewPanel()->GetCanvas();
	canvas->AddUpdateTask([&]()
	{
        if (m_model_type == ModelType::UNKNOWN) {
            return;
        }

		auto& ccomplex = m_obj->GetSharedComp<n0::CompComplex>();
		auto& nodes = const_cast<std::vector<n0::SceneNodePtr>&>(ccomplex.GetAllChildren());
        if (nodes.empty()) {
            return;
        }

		bp::NodePtr final_node = nullptr;
        std::vector<bp::NodePtr> all_bp_nodes;
        all_bp_nodes.reserve(nodes.size());
		for (auto& node : nodes)
		{
			assert(node->HasUniqueComp<bp::CompNode>());
			auto& bp_node = node->GetUniqueComp<bp::CompNode>().GetNode();
			assert(bp_node);
			if (bp_node->get_type().get_name().to_string() == m_model_type_str) {
				final_node = bp_node;
			}
            all_bp_nodes.push_back(bp_node);
		}
        if (!final_node) {
            return;
        }

		sg::ShaderWeaver::ShaderType shader_type;
        if (m_model_type_str == rttr::type::get<sg::node::PBR>().get_name().to_string()) {
            shader_type = sg::ShaderWeaver::SHADER_PBR;
        } else if (m_model_type_str == rttr::type::get<sg::node::Phong>().get_name().to_string()) {
            shader_type = sg::ShaderWeaver::SHADER_PHONG;
        } else if (m_model_type_str == rttr::type::get<sg::node::Raymarching>().get_name().to_string()) {
            shader_type = sg::ShaderWeaver::SHADER_RAYMARCHING;
        } else if (m_model_type_str == rttr::type::get<sg::node::Sprite>().get_name().to_string()) {
            shader_type = sg::ShaderWeaver::SHADER_SPRITE;
        } else {
			assert(0);
		}

        auto& rc = ur::Blackboard::Instance()->GetRenderContext();
        int old_vl_id = rc.GetBindedVertexLayoutID();

		sg::ShaderWeaver sw(shader_type, *final_node, DEBUG_PRINT_SHADER, all_bp_nodes);
		auto& wc = canvas->GetWidnowContext().wc3;
		auto shader = sw.CreateShader3();
        if (shader->IsValid()) {
            shader->AddNotify(std::const_pointer_cast<pt3::WindowContext>(wc));
            m_toolbar->GetPreviewPanel()->SetShader(shader);
            dirty = true;
        } else {
            rc.BindVertexLayout(old_vl_id);
            dirty = false;
        }
	});

	auto& wc = GetImpl().GetCanvas()->GetWidnowContext();
	bp::UpdateParams params(wc.wc2, wc.wc3);
	Traverse([&](const ee0::GameObj& obj)->bool
	{
		if (obj->HasUniqueComp<bp::CompNode>())
		{
			auto& bp_node = obj->GetUniqueComp<bp::CompNode>().GetNode();
			if (bp_node->Update(params)) {
				dirty = true;
			}
		}
		return true;
	});

	if (dirty) {
		m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
	}
}

void WxStagePage::UpdateParentAABB(const bp::NodePtr& node)
{
    auto& st = node->GetStyle();
    m_parent_node->GetUniqueComp<n2::CompBoundingBox>().SetSize(
        *m_parent_node, sm::rect(st.width, st.height)
    );
}

}
}