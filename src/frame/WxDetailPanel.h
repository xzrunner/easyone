#pragma once

#include <ee0/Observer.h>
#include <ee0/typedef.h>
#include <ee0/GameObj.h>

#ifndef GAME_OBJ_ECS
#include <node0/NodeWithPos.h>
#endif // GAME_OBJ_ECS
#include <moon/typedef.h>

#include <wx/panel.h>

namespace ee0 { class WxCompPanel; }
#ifdef GAME_OBJ_ECS
namespace ecsx { class World; }
#endif // GAME_OBJ_ECS

namespace eone
{

class WxDetailPanel : public wxPanel, public ee0::Observer
{
public:
	WxDetailPanel(
		wxWindow* parent,
		const ee0::SubjectMgrPtr& sub_mgr,
#ifdef GAME_OBJ_ECS
		ecsx::World& world,
#endif // GAME_OBJ_ECS
		const ee0::GameObj& root_obj, 
		const moon::ContextPtr& moon_ctx
	);

	virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants) override;

private:
	void InitLayout();
	void RegisterMsg(ee0::SubjectMgr& sub_mgr);

	void InitComponents(const ee0::VariantSet& variants);
	void InitComponents(const ee0::GameObj& obj);
	void InitComponents();
	void ClearComponents();
	void UpdateComponents();
	void StagePageChanged(const ee0::VariantSet& variants);

	void OnAddPress(wxCommandEvent& event);

private:
	ee0::SubjectMgrPtr m_sub_mgr;
#ifdef GAME_OBJ_ECS
	ecsx::World&       m_world;
#endif // GAME_OBJ_ECS
	ee0::GameObj       m_root_obj;
	moon::ContextPtr   m_moon_ctx;

	wxSizer* m_comp_sizer;

	wxButton* m_add_btn;

	std::vector<ee0::WxCompPanel*> m_components;

	ee0::GameObjWithPos m_owp;

}; // WxDetailPanel

}