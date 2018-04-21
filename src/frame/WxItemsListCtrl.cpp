#include "frame/WxItemsListCtrl.h"
#include "frame/WxStagePage.h"
#include "frame/config.h"

#include <ee0/SubjectMgr.h>
#include <ee0/WxImageVList.h>
#ifndef GAME_OBJ_ECS
#include <ee0/CompNodeEditor.h>
#else
#include <ee0/CompEntityEditor.h>
#endif // GAME_OBJ_ECS
#include <ee0/WxLibraryItem.h>
#include <ee0/WxImageVList.h>

#ifndef GAME_OBJ_ECS
#include <node0/SceneNode.h>
#else
#include <entity0/World.h>
#endif // GAME_OBJ_ECS

#include <guard/check.h>

namespace eone
{

WxItemsListCtrl::WxItemsListCtrl(wxWindow* parent, ECS_WORLD_PARAM
	                             const ee0::SubjectMgrPtr& sub_mgr)
	: wxWindow(parent, wxID_ANY)
	ECS_WORLD_SELF_ASSIGN
	, m_sub_mgr(sub_mgr)
	, m_list(nullptr)
{
	SetBackgroundColour(PANEL_COLOR);

	InitLayout();

	RegisterMsg(*m_sub_mgr);
}

void WxItemsListCtrl::OnNotify(uint32_t msg, const ee0::VariantSet& variants)
{
	switch (msg)
	{
	case ee0::MSG_INSERT_SCENE_NODE:
		InsertSceneObj(variants);
		break;
	case ee0::MSG_DELETE_SCENE_NODE:
		DeleteSceneObj(variants);
		break;
	case ee0::MSG_CLEAR_SCENE_NODE:
		ClearSceneObj();
		break;
	case ee0::MSG_REORDER_SCENE_NODE:
		ReorderSceneObj(variants);
		break;

	case ee0::MSG_NODE_SELECTION_INSERT:
		SelectSceneObj(variants);
		break;
	case ee0::MSG_NODE_SELECTION_DELETE:
		UnselectSceneObj(variants);
		break;
	case ee0::MSG_NODE_SELECTION_CLEAR:
		ClearALLSelected();
		break;
	case ee0::MSG_STAGE_PAGE_CHANGED:
		StagePageChanged(variants);
		break;
	}
}

void WxItemsListCtrl::OnSearch(const std::string& str)
{
	m_list->DeselectAll();

	int idx = 0;
	m_list->Traverse([&](const ee0::WxLibraryItem& item)->bool 
	{
		if (item.GetFilepath().find(str) != std::string::npos) {
			m_list->Select(idx);
		}
		++idx;
		return true;
	});
}

void WxItemsListCtrl::InitLayout()
{
	wxSizer* sizer = new wxBoxSizer(wxVERTICAL);

	m_list = new ee0::WxImageVList(this, "", true, false, true);
	sizer->Add(m_list, 1, wxEXPAND);

	SetSizer(sizer);
}

void WxItemsListCtrl::RegisterMsg(ee0::SubjectMgr& sub_mgr)
{
	sub_mgr.RegisterObserver(ee0::MSG_INSERT_SCENE_NODE, this);
	sub_mgr.RegisterObserver(ee0::MSG_DELETE_SCENE_NODE, this);
	sub_mgr.RegisterObserver(ee0::MSG_CLEAR_SCENE_NODE, this);
	sub_mgr.RegisterObserver(ee0::MSG_REORDER_SCENE_NODE, this);

	sub_mgr.RegisterObserver(ee0::MSG_NODE_SELECTION_INSERT, this);
	sub_mgr.RegisterObserver(ee0::MSG_NODE_SELECTION_DELETE, this);
	sub_mgr.RegisterObserver(ee0::MSG_NODE_SELECTION_CLEAR, this);
	sub_mgr.RegisterObserver(ee0::MSG_STAGE_PAGE_CHANGED, this);
}

void WxItemsListCtrl::InsertSceneObj(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("obj");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: obj");
	ee0::GameObj* obj = static_cast<ee0::GameObj*>(var.m_val.pv);
	GD_ASSERT(obj, "err scene obj");

	auto filepath = GetItemFilepath(*obj);
	m_list->Insert(std::make_shared<ee0::WxLibraryItem>(filepath), 0);
}

void WxItemsListCtrl::DeleteSceneObj(const ee0::VariantSet& variants)
{
	m_list->DeselectAll();

	auto var = variants.GetVariant("obj");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: obj");
	ee0::GameObj* obj = static_cast<ee0::GameObj*>(var.m_val.pv);
	GD_ASSERT(obj, "err scene obj");

	auto filepath = GetItemFilepath(*obj);
	int idx = m_list->QueryItemIndex(filepath);
	GD_ASSERT(idx != -1, "not find");
	m_list->Remove(idx);
}

void WxItemsListCtrl::ClearSceneObj()
{
	m_list->DeselectAll();
	m_list->Clear();
}

void WxItemsListCtrl::ReorderSceneObj(const ee0::VariantSet& variants)
{
	m_list->DeselectAll();

	auto obj_var = variants.GetVariant("obj");
	GD_ASSERT(obj_var.m_type == ee0::VT_PVOID, "no var in vars: obj");
	ee0::GameObj* obj = static_cast<ee0::GameObj*>(obj_var.m_val.pv);
	GD_ASSERT(obj, "err scene obj");

	auto up_var = variants.GetVariant("up");
	GD_ASSERT(up_var.m_type == ee0::VT_BOOL, "no var in vars: up");
	bool up = up_var.m_val.bl;

	auto filepath = GetItemFilepath(*obj);
	int idx = m_list->QueryItemIndex(filepath);
	GD_ASSERT(idx != -1, "not find");

	int n = m_list->GetItemsSize();
	if (up)
	{
		int pos = idx - 1;
		if (pos >= 0)
		{
			m_list->Swap(idx, pos);
			m_list->SetSelection(pos);
		}
	}
	else
	{
		int pos = idx + 1;
		if (pos < n)
		{
			m_list->Swap(idx, pos);
			m_list->SetSelection(pos);
		}
	}
}

void WxItemsListCtrl::SelectSceneObj(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("obj");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: obj");
	ee0::GameObj* obj = static_cast<ee0::GameObj*>(var.m_val.pv);
	GD_ASSERT(obj, "err scene obj");

	bool multiple = false;
	auto var_multi = variants.GetVariant("multiple");
	if (var_multi.m_type == ee0::VT_BOOL && var_multi.m_val.bl) {
		multiple = true;
	}

	int idx = 0;
	auto filepath = GetItemFilepath(*obj);
	m_list->Traverse([&](const ee0::WxLibraryItem& item)->bool
	{
		if (item.GetFilepath() == filepath) {
			m_list->Select(idx);
		}
		++idx;
		return true;
	});
}

void WxItemsListCtrl::UnselectSceneObj(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("obj");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: obj");
	ee0::GameObj* obj = static_cast<ee0::GameObj*>(var.m_val.pv);
	GD_ASSERT(obj, "err scene obj");

	int idx = 0;
	auto filepath = GetItemFilepath(*obj);
	m_list->Traverse([&](const ee0::WxLibraryItem& item)->bool
	{
		if (item.GetFilepath() == filepath) {
			m_list->Select(idx, false);
		}
		++idx;
		return true;
	});
}

void WxItemsListCtrl::ClearALLSelected()
{
	m_list->DeselectAll();
}

void WxItemsListCtrl::StagePageChanged(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("new_page");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: new_page");
	GD_ASSERT(var.m_val.pv, "err new_page");

	auto new_page = static_cast<WxStagePage*>(var.m_val.pv);

	m_sub_mgr = new_page->GetSubjectMgr();
	RegisterMsg(*m_sub_mgr);

	ClearSceneObj();
}

std::string WxItemsListCtrl::GetItemFilepath(const ee0::GameObj& obj)
{
#ifndef GAME_OBJ_ECS
	return obj->GetUniqueComp<ee0::CompNodeEditor>().GetFilepath();
#else
	auto& ceditor = m_world.GetComponent<ee0::CompEntityEditor>(*obj);
	if (ceditor.filepath) {
		return *ceditor.filepath;
	} else {
		return "";
	}
#endif // GAME_OBJ_ECS
}

}