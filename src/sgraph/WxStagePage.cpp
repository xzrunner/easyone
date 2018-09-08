#include "sgraph/WxStagePage.h"
#include "sgraph/WxToolbarPanel.h"
#include "sgraph/MessageID.h"
#include "sgraph/WxToolbarPanel.h"

#include "frame/AppStyle.h"
#include "frame/Blackboard.h"
#include "frame/WxToolbarPanel.h"

#include <ee0/SubjectMgr.h>
#include <ee0/MsgHelper.h>
#include <ee3/WxMaterialPreview.h>
#include <blueprint/Blueprint.h>
#include <blueprint/CompNode.h>
#include <blueprint/MessageID.h>
#include <blueprint/Pins.h>
#include <blueprint/NSCompNode.h>
#include <shadergraph/ShaderGraph.h>
#include <shadergraph/Utility.h>
#include <shadergraph/NodeBuilder.h>
#include <shadergraph/node/Phong.h>
#include <shadergraph/node/TextureObject.h>

#include <js/RapidJsonHelper.h>
#include <node0/SceneNode.h>
#include <node0/CompComplex.h>
#include <sx/ResFileHelper.h>

#include <boost/filesystem.hpp>

namespace eone
{
namespace sgraph
{

const std::string WxStagePage::PAGE_TYPE = "shader_graph";

WxStagePage::WxStagePage(wxWindow* parent, ECS_WORLD_PARAM const ee0::GameObj& obj)
	: eone::WxStagePage(parent, ECS_WORLD_VAR obj, SHOW_STAGE | SHOW_TOOLBAR | TOOLBAR_LFET)
{
	bp::Blueprint::Init();
	shadergraph::ShaderGraph::Init();

	m_messages.push_back(ee0::MSG_INSERT_SCENE_NODE);
	m_messages.push_back(ee0::MSG_DELETE_SCENE_NODE);
	m_messages.push_back(ee0::MSG_CLEAR_SCENE_NODE);
	m_messages.push_back(MSG_SET_MODEL_TYPE);
	m_messages.push_back(bp::MSG_BLUE_PRINT_CHANGED);
}

void WxStagePage::OnNotify(uint32_t msg, const ee0::VariantSet& variants)
{
	eone::WxStagePage::OnNotify(msg, variants);

	bool dirty = false;
	switch (msg)
	{
	case ee0::MSG_INSERT_SCENE_NODE:
		dirty = InsertSceneObj(variants);
		break;
	case ee0::MSG_DELETE_SCENE_NODE:
		dirty = DeleteSceneObj(variants);
		break;
	case ee0::MSG_CLEAR_SCENE_NODE:
		dirty = ClearSceneObj();
		break;

	case MSG_SET_MODEL_TYPE:
		dirty = SetModelType(variants.GetVariant("type").m_val.pc);
		break;

	case bp::MSG_BLUE_PRINT_CHANGED:
		dirty = CalcMaterial();
		if (dirty) {
			m_toolbar->GetPreviewPanel()->RefreshCanvas();
		}
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
	auto panel = Blackboard::Instance()->GetToolbarPanel();
	auto sizer = panel->GetSizer();
	if (sizer) {
		sizer->Clear(true);
	} else {
		sizer = new wxBoxSizer(wxVERTICAL);
	}
	// todo
#ifndef GAME_OBJ_ECS
	sizer->Add(m_toolbar = new WxToolbarPanel(panel, m_sub_mgr,
		&GetImpl().GetCanvas()->GetRenderContext()));
	panel->SetSizer(sizer);
#endif // GAME_OBJ_ECS
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
	if (sx::ResFileHelper::Type(filepath) != sx::RES_FILE_JSON) {
		return;
	}

	auto dir = boost::filesystem::path(filepath).parent_path().string();
	rapidjson::Document doc;
	js::RapidJsonHelper::ReadFromFile(filepath.c_str(), doc);

	// connection
	auto& ccomplex = m_obj->GetSharedComp<n0::CompComplex>();
	auto& nodes = const_cast<std::vector<n0::SceneNodePtr>&>(ccomplex.GetAllChildren());
	bp::NSCompNode::LoadConnection(nodes, doc["nodes"]);

	m_sub_mgr->NotifyObservers(bp::MSG_BLUE_PRINT_CHANGED);
}

bool WxStagePage::InsertSceneObj(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("obj");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: obj");
	ee0::GameObj* obj = static_cast<ee0::GameObj*>(var.m_val.pv);
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
	ee0::GameObj* obj = static_cast<ee0::GameObj*>(var.m_val.pv);
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

bool WxStagePage::SetModelType(const std::string& model)
{
	if (m_model_type == model) {
		return false;
	}

	std::vector<n0::SceneNodePtr> nodes;
	auto bp_node = shadergraph::NodeBuilder::Create(model, nodes);
	if (!bp_node) {
		return false;
	}

	ClearSceneObj();
	m_model_type = model;

	for (auto& node : nodes) {
		ee0::MsgHelper::InsertNode(*m_sub_mgr, node, false);
	}

	return true;
}

bool WxStagePage::CalcMaterial()
{
	auto& ccomplex = m_obj->GetSharedComp<n0::CompComplex>();
	auto& nodes = const_cast<std::vector<n0::SceneNodePtr>&>(ccomplex.GetAllChildren());
	if (nodes.empty()) {
		return false;
	}

	std::shared_ptr<bp::Node> bp_out_node = nullptr;
	for (auto& node : nodes)
	{
		assert(node->HasUniqueComp<bp::CompNode>());
		auto& bp_node = node->GetUniqueComp<bp::CompNode>().GetNode();
		assert(bp_node);
		if (bp_node->TypeName() == m_model_type) {
			bp_out_node = bp_node;
		}
	}
	assert(bp_out_node);

	if (m_model_type == shadergraph::node::Phong::TYPE_NAME)
	{
		auto& phong = std::dynamic_pointer_cast<shadergraph::node::Phong>(bp_out_node);
		assert(phong);
		phong->CalcMaterial(m_toolbar->GetPreviewMaterial());
	}

	return true;
}

}
}