#include "prototype/WxStagePage.h"

#include "frame/AppStyle.h"
#include "frame/Blackboard.h"
#include "frame/WxToolbarPanel.h"

#include <ee0/SubjectMgr.h>
#include <ee0/WxLibraryPanel.h>
#include <ee2/WxStageDropTarget.h>

#include <node0/SceneNode.h>
#include <node0/CompComplex.h>
#include <node0/CompIdentity.h>
#include <node2/CompScript.h>
#include <js/RapidJsonHelper.h>
#include <moon/Blackboard.h>
#include <moon/Context.h>
#include <moon/ScriptHelper.h>
#include <prototyping/WxLibraryPage.h>
#include <prototyping/WxToolbarPanel.h>

#include <boost/filesystem.hpp>

namespace
{

const char* CFG_DIR  = "script/library/prototyping";
const char* CFG_FILE = "layout.json";

const char* PT_SCRIPT_DIR = "ext/prototyping/script";

}

namespace eone
{
namespace prototype
{

const std::string WxStagePage::PAGE_TYPE = "prototyping";

WxStagePage::WxStagePage(wxWindow* parent, ee0::WxLibraryPanel* library, ECS_WORLD_PARAM const ee0::GameObj& obj)
	: eone::WxStagePage(parent, ECS_WORLD_VAR obj, SHOW_LIBRARY | SHOW_STAGE | SHOW_TOOLBAR)
{
	m_messages.push_back(ee0::MSG_SCENE_NODE_INSERT);
	m_messages.push_back(ee0::MSG_SCENE_NODE_DELETE);
	m_messages.push_back(ee0::MSG_SCENE_NODE_CLEAR);
	m_messages.push_back(ee0::MSG_EDITOR_RELOAD);

	if (library) {
		SetDropTarget(new ee2::WxStageDropTarget(ECS_WORLD_VAR library, this));
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
	case ee0::MSG_EDITOR_RELOAD:
		dirty = ReloadSceneObj();
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
	InitScriptContext();
	InitLibraryPanel();
	InitToolbarPanel();
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
}

void WxStagePage::LoadFromFileExt(const std::string& filepath)
{
}

bool WxStagePage::InsertSceneObj(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("obj");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: obj");
	ee0::GameObj* obj = static_cast<ee0::GameObj*>(var.m_val.pv);
	GD_ASSERT(obj, "err scene obj");

//	auto& cid = (*obj)->GetUniqueComp<n0::CompIdentity>();
////	cid.GetFilepath();
//	auto L = moon::Blackboard::Instance()->GetContext()->GetState();
////	auto path = boost::filesystem::absolute(itr->second, CFG_DIR);
//	auto err = moon::ScriptHelper::DoFile(L, cid.GetFilepath().c_str());
//	if (err) {
//		printf("lua err: %s\n", err);
//	}

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

bool WxStagePage::ReloadSceneObj()
{
	Traverse([&](const ee0::GameObj& obj)->bool
	{
		if (obj->HasUniqueComp<n2::CompScript>())
		{
			auto& cscript = obj->GetUniqueComp<n2::CompScript>();
			cscript.Reload(obj);
			cscript.GetImpl().Init();
		}
		return true;
	});

	// gc
	auto L = moon::Blackboard::Instance()->GetContext()->GetState();
	moon::ScriptHelper::GC(L);

	return true;
}

void WxStagePage::InitScriptContext()
{
	static bool inited = false;
	if (!inited)
	{
		auto L = moon::Blackboard::Instance()->GetContext()->GetState();
		auto dir = boost::filesystem::absolute(PT_SCRIPT_DIR).string();
		moon::ScriptHelper::AddSearchPath(L, (dir + "\\?.lua" + ";" + dir + "\\?\\init.lua").c_str());

		inited = true;
	}
}

void WxStagePage::InitLibraryPanel()
{
	auto panel = Blackboard::Instance()->GetLiraryPanel();
	panel->ClearPages();

	rapidjson::Document doc;

	auto path = boost::filesystem::absolute(CFG_FILE, CFG_DIR);
	if (!js::RapidJsonHelper::ReadFromFile(path.string().c_str(), doc)) {
		return;
	}

	auto nb = panel->GetNotebook();
	for (auto& page_val : doc["pages"].GetArray())
	{
		auto name = page_val["name"].GetString();
		auto page = new pt::WxLibraryPage(nb, name, CFG_DIR, page_val["items"]);
		panel->AddPage(page, name);
	}
}

void WxStagePage::InitToolbarPanel()
{
	auto panel = Blackboard::Instance()->GetToolbarPanel();
	auto sizer = panel->GetSizer();
	if (sizer) {
		sizer->Clear(true);
	} else {
		sizer = new wxBoxSizer(wxVERTICAL);
	}
	sizer->Add(/*m_toolbar = */new pt::WxToolbarPanel(panel, GetSubjectMgr()));
	panel->SetSizer(sizer);
}

}
}