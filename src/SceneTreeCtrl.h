#pragma once

#include <ee0/Observer.h>

#include <node0/typedef.h>

#include <wx/treectrl.h>

namespace ee0 { class SubjectMgr; }

namespace eone
{

class SceneTreePanel;

class SceneTreeCtrl : public wxTreeCtrl, public ee0::Observer
{
public:
	SceneTreeCtrl(wxWindow* parent, ee0::SubjectMgr& sub_mgr);

	virtual void OnNotify(ee0::MessageID msg, const ee0::VariantSet& variants) override;

	void Traverse(wxTreeItemId id, std::function<bool(wxTreeItemId)> func) const;

private:
	void InitRoot();
	void RegisterMsg(ee0::SubjectMgr& sub_mgr);

	void OnSelChanged(wxTreeEvent& event);
	void OnLabelEdited(wxTreeEvent& event);

	void SelectSceneNode(const ee0::VariantSet& variants);
	void UnselectSceneNode(const ee0::VariantSet& variants);
	void InsertSceneNode(const ee0::VariantSet& variants);
	void InsertSceneNode(wxTreeItemId parent, const n0::SceneNodePtr& child);
	void StagePageChanging(const ee0::VariantSet& variants);

private:
	enum
	{
		ID_SCENE_TREE_CTRL = 1000
	};

private:
	ee0::SubjectMgr* m_sub_mgr;

	wxTreeItemId m_root;

	DECLARE_EVENT_TABLE()

}; // SceneTreeCtrl

}