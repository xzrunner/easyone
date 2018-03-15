#pragma once

#include <ee0/Observer.h>
#include <ee0/typedef.h>

#include <node0/NodeWithPos.h>

#include <wx/panel.h>

namespace ee0 { class WxCompPanel; }

namespace eone
{

class WxDetailPanel : public wxPanel, public ee0::Observer
{
public:
	WxDetailPanel(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr,
		const n0::SceneNodePtr& root_node);

	virtual void OnNotify(ee0::MessageID msg, const ee0::VariantSet& variants) override;

private:
	void InitLayout();
	void RegisterMsg(ee0::SubjectMgr& sub_mgr);

	void InitComponents(const ee0::VariantSet& variants);
	void InitComponents(const n0::SceneNodePtr& node);
	void InitComponents();
	void ClearComponents();
	void UpdateComponents();
	void StagePageChanged(const ee0::VariantSet& variants);

	void OnAddPress(wxCommandEvent& event);

private:
	ee0::SubjectMgrPtr m_sub_mgr;
	n0::SceneNodePtr   m_root_node;

	wxSizer* m_comp_sizer;

	wxButton* m_add_btn;

	std::vector<ee0::WxCompPanel*> m_components;

	n0::NodeWithPos m_nwp;

}; // WxDetailPanel

}