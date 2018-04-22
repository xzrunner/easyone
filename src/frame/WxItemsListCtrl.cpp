#include "frame/WxItemsListCtrl.h"
#include "frame/WxItemsListList.h"
#include "frame/WxItemsListUserData.h"
#include "frame/WxStagePage.h"
#include "frame/GameObjType.h"
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
#include <ee0/MsgHelper.h>

#ifndef GAME_OBJ_ECS
#include <node0/SceneNode.h>
#include <node0/CompAsset.h>
#else
#include <entity0/World.h>
#endif // GAME_OBJ_ECS

#include <guard/check.h>

namespace
{

static const std::vector<std::pair<uint32_t, std::string>> GAME_OBJ_LIST =
{
	std::make_pair(eone::GAME_OBJ_UNKNOWN,    "All"),
	std::make_pair(eone::GAME_OBJ_IMAGE,      "Image"),
	std::make_pair(eone::GAME_OBJ_TEXT,       "Text"),
	std::make_pair(eone::GAME_OBJ_MASK,       "Mask"),
	std::make_pair(eone::GAME_OBJ_MESH,       "Mesh"),
	std::make_pair(eone::GAME_OBJ_SCALE9,     "Scale9"),
	std::make_pair(eone::GAME_OBJ_ANIM,       "Anim"),
	std::make_pair(eone::GAME_OBJ_PARTICLE3D, "Particle3d"),
	std::make_pair(eone::GAME_OBJ_SCENE2D,    "Complex"),
};

}

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
		auto& obj = *dynamic_cast<WxItemsListUserData*>(item.GetUD().get());
		auto& name = obj.GetObj()->GetUniqueComp<ee0::CompNodeEditor>().GetName();
		if (name.find(str) != std::string::npos) {
			m_list->Select(idx);
		}
		++idx;
		return true;
	});
}

void WxItemsListCtrl::InitLayout()
{
	wxSizer* top_sizer = new wxBoxSizer(wxVERTICAL);

	// header
	{
		wxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);

		wxArrayString choices;
		for (auto& type : GAME_OBJ_LIST) {
			choices.Add(type.second);
		}
		m_filter = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, choices);
		Connect(m_filter->GetId(), wxEVT_COMMAND_CHOICE_SELECTED,
			wxCommandEventHandler(WxItemsListCtrl::OnSetFilter));
		m_filter->SetSelection(0);
		sizer->Add(m_filter);

		sizer->AddSpacer(10);

		auto select_all_btn = new wxButton(this, wxID_ANY, "Select All");
		Connect(select_all_btn->GetId(), wxEVT_COMMAND_BUTTON_CLICKED,
			wxCommandEventHandler(WxItemsListCtrl::OnSelectAll));
		sizer->Add(select_all_btn);

		top_sizer->Add(sizer);
	}

	m_list = new WxItemsListList(this, m_sub_mgr);
	top_sizer->Add(m_list, 1, wxEXPAND);

	SetSizer(top_sizer);
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

void WxItemsListCtrl::OnSetFilter(wxCommandEvent& event)
{
	auto filter_type = GAME_OBJ_LIST[event.GetSelection()].first;
	m_list->Traverse([&](const ee0::WxLibraryItem& item)->bool
	{
		const_cast<ee0::WxLibraryItem&>(item).SetHide(false);
		if (filter_type != GAME_OBJ_UNKNOWN) 
		{
			auto& obj = *dynamic_cast<WxItemsListUserData*>(item.GetUD().get());
			auto type = GetObjType(obj.GetObj());
			const_cast<ee0::WxLibraryItem&>(item).SetHide(type != filter_type);
		}
		return true;
	});
	m_list->Refresh();
}

void WxItemsListCtrl::OnSelectAll(wxCommandEvent& event)
{
	std::vector<ee0::GameObjWithPos> objs;
	m_list->Traverse([&](const ee0::WxLibraryItem& item)->bool
	{
		if (item.IsHide()) {
			return true;
		}
		auto& obj = *dynamic_cast<WxItemsListUserData*>(item.GetUD().get());
#ifndef GAME_OBJ_ECS
		objs.push_back(n0::NodeWithPos(obj.GetObj(), obj.GetRoot(), obj.GetObjID()));
#else
		objs.push_back(obj.GetObj());
#endif // GAME_OBJ_ECS
		return true;
	});
	ee0::MsgHelper::InsertSelection(*m_sub_mgr, objs);
}

void WxItemsListCtrl::InsertSceneObj(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("obj");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: obj");
	ee0::GameObj* obj = static_cast<ee0::GameObj*>(var.m_val.pv);
	GD_ASSERT(obj, "err scene obj");

	std::string filepath;
#ifndef GAME_OBJ_ECS
	filepath = (*obj)->GetUniqueComp<ee0::CompNodeEditor>().GetFilepath();
#else
	auto& ceditor = m_world.GetComponent<ee0::CompEntityEditor>(*obj);
	if (ceditor.filepath) {
		filepath = *ceditor.filepath;
	} else {
		filtpath = "";
	}
#endif // GAME_OBJ_ECS
	auto item = std::make_shared<ee0::WxLibraryItem>(filepath);
	std::unique_ptr<ee0::WxLibraryItem::UserData> ud 
		= std::make_unique<WxItemsListUserData>(*obj, *obj, 0);
	item->SetUD(ud);
	m_list->Insert(item, 0);
}

void WxItemsListCtrl::DeleteSceneObj(const ee0::VariantSet& variants)
{
	m_list->DeselectAll();

	auto var = variants.GetVariant("obj");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: obj");
	ee0::GameObj* obj = static_cast<ee0::GameObj*>(var.m_val.pv);
	GD_ASSERT(obj, "err scene obj");

	int idx = QueryItemIndex(*obj);
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

	int idx = QueryItemIndex(*obj);
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
	m_list->Traverse([&](const ee0::WxLibraryItem& item)->bool
	{
		auto& item_obj = dynamic_cast<WxItemsListUserData*>(item.GetUD().get())->GetObj();
		if (item_obj == *obj) {
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
	m_list->Traverse([&](const ee0::WxLibraryItem& item)->bool
	{
		auto& item_obj = dynamic_cast<WxItemsListUserData*>(item.GetUD().get())->GetObj();
		if (item_obj == *obj) {
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

int WxItemsListCtrl::QueryItemIndex(const ee0::GameObj& obj) const
{
	for (int i = 0, n = m_list->GetItemsSize(); i < n; ++i)
	{
		auto item = m_list->GetItemByIndex(i);
		auto& item_obj = dynamic_cast<WxItemsListUserData*>(item->GetUD().get())->GetObj();
		if (item_obj == obj) {
			return i;
		}
	}
	return -1;
}

}