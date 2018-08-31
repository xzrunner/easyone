#include "material/WxStagePage.h"
#include "material/WxToolbarPanel.h"
#include "material/MessageID.h"
#include "material/WxToolbarPanel.h"

#include "frame/AppStyle.h"
#include "frame/Blackboard.h"
#include "frame/WxToolbarPanel.h"

#include <ee0/SubjectMgr.h>
#include <ee3/WxMaterialPreview.h>
#include <ematerial/NodeFactory.h>
#include <ematerial/PhongModel.h>
#include <ematerial/Utility.h>
#include <ematerial/TextureObject.h>
#include <blueprint/CompNode.h>
#include <blueprint/MessageID.h>
#include <blueprint/Pins.h>
#include <blueprint/Connecting.h>

#include <painting3/Material.h>
#include <node0/SceneNode.h>
#include <node0/CompComplex.h>
#include <node0/CompIdentity.h>
#include <node2/CompTransform.h>
#include <node2/CompBoundingBox.h>
#include <model/TextureLoader.h>
#include <facade/Image.h>

namespace eone
{
namespace material
{

WxStagePage::WxStagePage(wxWindow* parent, ECS_WORLD_PARAM const ee0::GameObj& obj)
	: eone::WxStagePage(parent, ECS_WORLD_VAR obj, SHOW_STAGE | SHOW_TOOLBAR | TOOLBAR_LFET)
{
	m_messages.push_back(ee0::MSG_INSERT_SCENE_NODE);
	m_messages.push_back(ee0::MSG_DELETE_SCENE_NODE);
	m_messages.push_back(ee0::MSG_CLEAR_SCENE_NODE);
	m_messages.push_back(MSG_SET_MODEL_TYPE);
	m_messages.push_back(bp::MSG_CONNECTION_CHANGED);
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

	case bp::MSG_CONNECTION_CHANGED:
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
	sizer->Add(m_toolbar = new WxToolbarPanel(panel, m_sub_mgr));
	panel->SetSizer(sizer);
#endif // GAME_OBJ_ECS
}

#ifndef GAME_OBJ_ECS
const n0::NodeComp& WxStagePage::GetEditedObjComp() const
{
	return m_obj->GetSharedComp<n0::CompComplex>();
}
#endif // GAME_OBJ_ECS

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

	auto bp_node = ematerial::NodeFactory::Instance()->Create(model);
	if (!bp_node) {
		return false;
	}

	m_model_type = model;

	ClearSceneObj();

	auto node = std::make_shared<n0::SceneNode>();
	auto& cnode = node->AddSharedComp<bp::CompNode>(bp_node);
	cnode.GetNode()->SetParent(node);
	node->AddUniqueComp<n2::CompTransform>();
	node->AddUniqueComp<n0::CompIdentity>();
	auto& style = cnode.GetNode()->GetStyle();
	node->AddUniqueComp<n2::CompBoundingBox>(
		sm::rect(style.width, style.height)
	);

	auto& ccomplex = m_obj->GetSharedComp<n0::CompComplex>();
	ccomplex.AddChild(node);

	m_mat_node = cnode.GetNode();

	return true;
}

bool WxStagePage::CalcMaterial()
{
	if (!m_mat_node) {
		return false;
	}

	auto& mat = m_toolbar->GetPreviewMaterial();

	if (m_model_type == "mat_phong_model")
	{
		auto& pmodel = std::dynamic_pointer_cast<ematerial::PhongModel>(m_mat_node);
		mat.ambient   = ematerial::Utility::CalcNodeInputVal(*pmodel->GetAmbient());
		mat.diffuse   = ematerial::Utility::CalcNodeInputVal(*pmodel->GetDiffuse());
		mat.specular  = ematerial::Utility::CalcNodeInputVal(*pmodel->GetSpecular());
		mat.shininess = ematerial::Utility::CalcNodeInputVal(*pmodel->GetShininess()).x;

		auto& tex_pin = pmodel->GetDiffuseTex();
		auto& conn = tex_pin->GetConnecting();
		if (conn.size() == 1) {
			auto from = conn[0]->GetFrom();
			if (from) {
				auto& img = static_cast<const ematerial::TextureObject&>(from->GetParent()).GetTexture();
				if (img) {
					mat.diffuse_tex = model::TextureLoader::LoadFromFile(img->GetResPath().c_str());
				}
			}
		}
	}

	return true;
}

}
}